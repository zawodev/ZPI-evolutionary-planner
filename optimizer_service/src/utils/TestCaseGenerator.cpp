#include "utils/TestCaseGenerator.hpp"
#include <random>
#include <algorithm>
#include <numeric>
#include <sstream>
#include "utils/Logger.hpp"

RawProblemData TestCaseGenerator::generate(int numStudents, int numGroups, int numSubjects, int numRooms, int numTeachers, int numTimeslots, int totalGroupCapacity) {
    const int DAYS = 7;
    try {
        Logger::info("Starting TestCaseGenerator::generate with numStudents=" + std::to_string(numStudents) +
                     ", numGroups=" + std::to_string(numGroups) + ", numSubjects=" + std::to_string(numSubjects) +
                     ", numRooms=" + std::to_string(numRooms) + ", numTeachers=" + std::to_string(numTeachers) +
                     ", numTimeslots=" + std::to_string(numTimeslots) + ", totalGroupCapacity=" + std::to_string(totalGroupCapacity));
        RawProblemData data;
        std::random_device rd;
        std::mt19937 gen(rd());

        int MGMT_PREFERENCE_DENSITY_FACTOR = 10; // higher means more preferences
        int MAX_WEIGHT = 100; // max weight for preferences

        // Common distributions
        std::uniform_int_distribution<> dist_bool(0, 1);
        std::uniform_int_distribution<> dist_num_ts(0, numTimeslots);
        std::uniform_int_distribution<> dist_num_mgmt(0, numRooms * numTimeslots / MGMT_PREFERENCE_DENSITY_FACTOR);

        // Weight distribution: logarithmic, P(k) proportional to 1/(k+1) for k=0 to 100
        std::vector<double> weights(MAX_WEIGHT);
        for (int i = 0; i < MAX_WEIGHT; ++i) {
            weights[i] = 1.0 / (i + 1);
        }
        std::discrete_distribution<> dist_weight(weights.begin(), weights.end());

        // timeslots_per_day: 50/50 chance of normal distribution on first 5 days (weekend 0) or all 7 days
        data.timeslots_per_day.assign(DAYS, 0);
        bool weekend_free = dist_bool(gen);
        int days_to_distribute = weekend_free ? 5 : DAYS;
        double mean_slots = static_cast<double>(numTimeslots) / days_to_distribute;
        double stddev_slots = mean_slots / 3.0; // adjust for spread
        std::normal_distribution<> dist_slots(mean_slots, stddev_slots);
        std::vector<int> temp_slots(days_to_distribute);
        int total_slots_assigned = 0;
        for (int d = 0; d < days_to_distribute - 1; ++d) {
            int slots = std::max(0, static_cast<int>(std::round(dist_slots(gen))));
            temp_slots[d] = slots;
            total_slots_assigned += slots;
        }
        temp_slots.back() = std::max(0, numTimeslots - total_slots_assigned);
        for (int d = 0; d < days_to_distribute; ++d) {
            data.timeslots_per_day[d] = temp_slots[d];
        }

        std::uniform_int_distribution<> dist_days(0, DAYS - 1);

        // groups_per_subject: distribute numGroups among numSubjects with normal distribution
        double mean = static_cast<double>(numGroups) / numSubjects;
        double stddev = 1.0; // adjust as needed
        std::normal_distribution<> dist_normal(mean, stddev);
        data.groups_per_subject.resize(numSubjects);
        Logger::info("groups_per_subject resized to " + std::to_string(data.groups_per_subject.size()));
        int total_assigned = 0;
        for (int i = 0; i < numSubjects - 1; ++i) {
            int g = std::max(1, static_cast<int>(std::round(dist_normal(gen))));
            data.groups_per_subject[i] = g;
            total_assigned += g;
        }
        data.groups_per_subject.back() = std::max(1, numGroups - total_assigned);

        // groups_soft_capacity: distribute totalGroupCapacity among groups with normal distribution
        double mean_capacity = static_cast<double>(totalGroupCapacity) / numGroups;
        double stddev_capacity = mean_capacity / 3.0; // adjust for spread
        std::normal_distribution<> dist_capacity(mean_capacity, stddev_capacity);
        data.groups_soft_capacity.resize(numGroups);
        int total_capacity_assigned = 0;
        for (int i = 0; i < numGroups - 1; ++i) {
            int cap = std::max(1, static_cast<int>(std::round(dist_capacity(gen))));
            data.groups_soft_capacity[i] = cap;
            total_capacity_assigned += cap;
        }
        data.groups_soft_capacity.back() = std::max(1, totalGroupCapacity - total_capacity_assigned);

        // students_subjects: for each student, random subset of subjects
        std::uniform_int_distribution<> dist_num_subj(1, numSubjects);
        data.students_subjects.resize(numStudents);
        for (auto& subs : data.students_subjects) {
            int num = dist_num_subj(gen);
            std::vector<int> all_subj(numSubjects);
            std::iota(all_subj.begin(), all_subj.end(), 0);
            std::shuffle(all_subj.begin(), all_subj.end(), gen);
            subs.assign(all_subj.begin(), all_subj.begin() + num);
        }

        // teachers_groups: assign each group to exactly one teacher, with normal distribution of groups per teacher
        double mean_groups = static_cast<double>(numGroups) / numTeachers;
        double stddev_groups = 1.0;
        std::normal_distribution<> dist_groups_per_teacher(mean_groups, stddev_groups);
        data.teachers_groups.resize(numTeachers);
        std::vector<int> group_assignments(numGroups, -1); // -1 means unassigned
        int teacher_idx = 0;
        for (int t = 0; t < numTeachers; ++t) {
            int num_groups_for_teacher = std::max(1, static_cast<int>(std::round(dist_groups_per_teacher(gen))));
            if (t == numTeachers - 1) {
                // Last teacher gets remaining groups
                num_groups_for_teacher = numGroups - teacher_idx;
            }
            for (int i = 0; i < num_groups_for_teacher && teacher_idx < numGroups; ++i) {
                data.teachers_groups[t].push_back(teacher_idx);
                group_assignments[teacher_idx] = t;
                teacher_idx++;
            }
        }
        // Ensure all groups are assigned
        for (int g = 0; g < numGroups; ++g) {
            if (group_assignments[g] == -1) {
                // Assign to last teacher
                data.teachers_groups.back().push_back(g);
            }
        }

        // rooms_unavailability_timeslots: for each room, random unavailable timeslots
        std::uniform_int_distribution<> dist_num_unav(0, numTimeslots / 4);
        data.rooms_unavailability_timeslots.resize(numRooms);
        for (auto& unav : data.rooms_unavailability_timeslots) {
            int num = dist_num_unav(gen);
            std::vector<int> all_ts(numTimeslots);
            std::iota(all_ts.begin(), all_ts.end(), 0);
            std::shuffle(all_ts.begin(), all_ts.end(), gen);
            unav.assign(all_ts.begin(), all_ts.begin() + num);
            std::sort(unav.begin(), unav.end());
        }

        // Preferences

        // Students preferences
        data.students_preferences.resize(numStudents);
        for (int s = 0; s < numStudents; ++s) {
            auto& sp = data.students_preferences[s];
            // free_days, busy_days: always size DAYS, mostly 0s, logarithmic weights
            sp.free_days.assign(DAYS, 0);
            sp.busy_days.assign(DAYS, 0);
            for (int d = 0; d < DAYS; ++d) {
                if (dist_bool(gen)) { // 50% chance to have preference
                    int weight = dist_weight(gen);
                    if (dist_bool(gen)) { // 50% free or busy
                        sp.free_days[d] = weight;
                    } else {
                        sp.busy_days[d] = weight;
                    }
                }
            }

            // no_gaps
            sp.no_gaps = dist_weight(gen);

            // preferred_groups, avoid_groups: for each subject of student
            int num_subj = static_cast<int>(data.students_subjects[s].size());
            for (int subj_idx = 0; subj_idx < num_subj; ++subj_idx) {
                int subj = data.students_subjects[s][subj_idx];
                // Get groups for this subject
                int start_group = std::accumulate(data.groups_per_subject.begin(), data.groups_per_subject.begin() + subj, 0);
                int num_grps = data.groups_per_subject[subj];
                // preferred: random groups with weights
                std::uniform_int_distribution<> dist_num_pref(0, num_grps);
                int num_pref = dist_num_pref(gen);
                std::vector<int> grps(num_grps);
                std::iota(grps.begin(), grps.end(), start_group);
                std::shuffle(grps.begin(), grps.end(), gen);
                for (int i = 0; i < num_pref; ++i) {
                    int weight = dist_weight(gen);
                    if (weight > 0) sp.preferred_groups[grps[i]] = weight;
                }
                // avoid: similar
                int num_avoid = dist_num_pref(gen);
                std::shuffle(grps.begin(), grps.end(), gen);
                for (int i = 0; i < num_avoid; ++i) {
                    int weight = dist_weight(gen);
                    if (weight > 0) sp.avoid_groups[grps[i]] = weight;
                }
            }

            // preferred_timeslots, avoid_timeslots: similar
            for (int subj_idx = 0; subj_idx < num_subj; ++subj_idx) {
                int num_pref_ts = dist_num_ts(gen) % (numTimeslots / 2);
                std::vector<int> ts(numTimeslots);
                std::iota(ts.begin(), ts.end(), 0);
                std::shuffle(ts.begin(), ts.end(), gen);
                for (int i = 0; i < num_pref_ts; ++i) {
                    int weight = dist_weight(gen);
                    if (weight > 0) sp.preferred_timeslots[ts[i]] = weight;
                }
                int num_avoid_ts = dist_num_ts(gen) % (numTimeslots / 2);
                std::shuffle(ts.begin(), ts.end(), gen);
                for (int i = 0; i < num_avoid_ts; ++i) {
                    int weight = dist_weight(gen);
                    if (weight > 0) sp.avoid_timeslots[ts[i]] = weight;
                }
            }
        }

        // Teachers preferences
        data.teachers_preferences.resize(numTeachers);
        for (int t = 0; t < numTeachers; ++t) {
            auto& tp = data.teachers_preferences[t];
            // free_days, busy_days
            tp.free_days.assign(DAYS, 0);
            tp.busy_days.assign(DAYS, 0);
            for (int d = 0; d < DAYS; ++d) {
                if (dist_bool(gen)) { // 50% chance to have preference
                    int weight = dist_weight(gen);
                    if (dist_bool(gen)) { // 50% free or busy
                        tp.free_days[d] = weight;
                    } else {
                        tp.busy_days[d] = weight;
                    }
                }
            }

            // no_gaps
            tp.no_gaps = dist_weight(gen);

            // preferred_timeslots, avoid_timeslots: for each group they teach
            int num_grps = static_cast<int>(data.teachers_groups[t].size());
            for (int grp_idx = 0; grp_idx < num_grps; ++grp_idx) {
                int num_pref_ts = dist_num_ts(gen) % (numTimeslots / 2);
                std::vector<int> ts(numTimeslots);
                std::iota(ts.begin(), ts.end(), 0);
                std::shuffle(ts.begin(), ts.end(), gen);
                for (int i = 0; i < num_pref_ts; ++i) {
                    int weight = dist_weight(gen);
                    if (weight > 0) tp.preferred_timeslots[ts[i]] = weight;
                }
                int num_avoid_ts = dist_num_ts(gen) % (numTimeslots / 2);
                std::shuffle(ts.begin(), ts.end(), gen);
                for (int i = 0; i < num_avoid_ts; ++i) {
                    int weight = dist_weight(gen);
                    if (weight > 0) tp.avoid_timeslots[ts[i]] = weight;
                }
            }
        }

        // Management preferences
        // preferred_room_timeslots, avoid_room_timeslots: random
        int num_pref_rt = dist_num_mgmt(gen);
        for (int i = 0; i < num_pref_rt; ++i) {
            RoomTimeslotPreference rp;
            rp.room = std::uniform_int_distribution<>(0, numRooms - 1)(gen);
            rp.timeslot = std::uniform_int_distribution<>(0, numTimeslots - 1)(gen);
            int weight = dist_weight(gen);
            if (weight > 0) {
                rp.weight = weight;
                data.management_preferences.preferred_room_timeslots.push_back(rp);
            }
        }
        int num_avoid_rt = dist_num_mgmt(gen);
        for (int i = 0; i < num_avoid_rt; ++i) {
            RoomTimeslotPreference rp;
            rp.room = std::uniform_int_distribution<>(0, numRooms - 1)(gen);
            rp.timeslot = std::uniform_int_distribution<>(0, numTimeslots - 1)(gen);
            int weight = dist_weight(gen);
            if (weight > 0) {
                rp.weight = weight;
                data.management_preferences.avoid_room_timeslots.push_back(rp);
            }
        }
        // group_max_overflow
        data.management_preferences.group_max_overflow.value = std::uniform_int_distribution<>(0, 10)(gen);
        data.management_preferences.group_max_overflow.weight = dist_weight(gen);

        Logger::info("Finished TestCaseGenerator::generate successfully");
        return data;
    } 
    catch (const std::exception& e) {
        Logger::error("Error in TestCaseGenerator::generate: " + std::string(e.what()));
        return RawProblemData{};
    }
}

RawJobData TestCaseGenerator::generateJob(int numStudents, int numGroups, int numSubjects, int numRooms, int numTeachers, int numTimeslots, int totalGroupCapacity, int maxExecutionTime) {
    try {
        // generate problem data
        RawProblemData problemData = generate(numStudents, numGroups, numSubjects, numRooms, numTeachers, numTimeslots, totalGroupCapacity);
        
        // generate random job id
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist_id(1000, 9999);
        
        std::stringstream ss;
        ss << "test_job_" << std::time(nullptr) << "_" << dist_id(gen);
        std::string jobId = ss.str();
        
        Logger::info("Generated job with ID: " + jobId + " and max execution time: " + std::to_string(maxExecutionTime) + " seconds");
        
        return RawJobData(jobId, problemData, maxExecutionTime);
    }
    catch (const std::exception& e) {
        Logger::error("Error in TestCaseGenerator::generateJob: " + std::string(e.what()));
        return RawJobData{};
    }
}
