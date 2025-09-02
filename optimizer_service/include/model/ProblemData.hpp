#pragma once
#include <vector>
#include <map>

struct StudentPreference {
    std::vector<int> free_days;
    std::vector<int> busy_days;
    struct { bool value; int weight; } gaps;
    std::vector<std::map<int, int>> preferred_groups;
    std::vector<std::map<int, int>> avoid_groups;
    std::vector<std::map<int, int>> preferred_timeslots;
    std::vector<std::map<int, int>> avoid_timeslots;
};

struct TeacherPreference {
    std::vector<int> free_days;
    std::vector<int> busy_days;
    struct { bool value; int weight; } gaps;
    std::vector<std::map<int, int>> preferred_timeslots;
    std::vector<std::map<int, int>> avoid_timeslots;
};

struct ManagementRoomTimeslot {
    int room;
    int timeslot;
    int weight;
};

struct ManagementPreferences {
    std::vector<ManagementRoomTimeslot> preferred_room_timeslots;
    std::vector<ManagementRoomTimeslot> avoid_room_timeslots;
    struct { int value; int weight; } group_max_overflow;
};

struct ProblemData {
    // Constraints
    std::vector<int> timeslots_per_day;
    std::vector<int> groups_per_subject;
    std::vector<int> groups_soft_capacity;
    std::vector<std::vector<int>> students_subjects;
    std::vector<std::vector<int>> teachers_groups;
    std::vector<std::vector<int>> rooms_unavailability_timeslots;

    // Preferences
    std::vector<StudentPreference> students_preferences;
    std::vector<TeacherPreference> teachers_preferences;
    ManagementPreferences management_preferences;

    // Calculated fields
    int getDaysNum() const { return timeslots_per_day.size(); }
    int getSubjectsNum() const { return groups_per_subject.size(); }
    int getGroupsNum() const { return groups_soft_capacity.size(); }
    int getStudentsNum() const { return students_subjects.size(); }
    int getTeachersNum() const { return teachers_groups.size(); }
    int getRoomsNum() const { return rooms_unavailability_timeslots.size(); }
    int getGroupsForStudent(int studentId) const { return students_subjects[studentId].size(); }

    // Lazy calculation of total timeslots
    mutable int total_timeslots = 0;
    mutable bool total_timeslots_calculated = false;

    int totalTimeslots() const {
        if (!total_timeslots_calculated) {
            total_timeslots = 0;
            for (int t : timeslots_per_day) total_timeslots += t;
            total_timeslots_calculated = true;
        }
        return total_timeslots;
    }

    // Lazy calculation of subject total capacities
    mutable std::vector<int> subject_total_capacity;
    mutable bool subject_total_capacity_calculated = false;

    const std::vector<int>& getSubjectTotalCapacity() const {
        if (!subject_total_capacity_calculated) {
            subject_total_capacity.resize(getSubjectsNum(), 0);
            int group_idx = 0;
            for (int p = 0; p < getSubjectsNum(); ++p) {
                for (int g = 0; g < groups_per_subject[p]; ++g) {
                    subject_total_capacity[p] += groups_soft_capacity[group_idx++];
                }
            }
            subject_total_capacity_calculated = true;
        }
        return subject_total_capacity;
    }
};
