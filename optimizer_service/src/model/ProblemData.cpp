#include "model/ProblemData.hpp"
#include "utils/Logger.hpp"
#include <stdexcept>
#include <string>

ProblemData::ProblemData(const RawProblemData& input_data) : _rawData(input_data) {
    // calculate _total_timeslots
    _total_timeslots = 0;
    for (int t : _rawData.timeslots_per_day) _total_timeslots += t;

    // calculate _subject_total_capacity
    _subject_total_capacity.resize(getSubjectsNum(), 0);
    int group_idx = 0;
    for (int p = 0; p < getSubjectsNum(); ++p) {
        for (int g = 0; g < _rawData.groups_per_subject[p]; ++g) {
            _subject_total_capacity[p] += _rawData.groups_soft_capacity[group_idx++];
        }
    }

    // calculate _total_student_subjects
    _total_student_subjects = 0;
    for (const auto& subjects : _rawData.students_subjects) {
        _total_student_subjects += (int)subjects.size();
    }

    // calculate _cumulative_groups
    _cumulative_groups.resize(getSubjectsNum() + 1, 0);
    for (int i = 1; i <= getSubjectsNum(); ++i) {
        _cumulative_groups[i] = _cumulative_groups[i - 1] + _rawData.groups_per_subject[i - 1];
    }

    // calculate _student_weights_sums
    _student_weights_sums.resize(getStudentsNum(), 0);
    for (int s = 0; s < getStudentsNum(); ++s) {
        const auto& pref = _rawData.students_preferences[s];
        // sum free_days
        for (int val : pref.free_days) {
            _student_weights_sums[s] += val;
        }
        // sum busy_days
        for (int val : pref.busy_days) {
            _student_weights_sums[s] += val;
        }
        // sum no_gaps weight
        _student_weights_sums[s] += pref.no_gaps;
        // sum preferred_groups values
        for (const auto& pair : pref.preferred_groups) {
            _student_weights_sums[s] += pair.second;
        }
        // sum avoid_groups values
        for (const auto& pair : pref.avoid_groups) {
            _student_weights_sums[s] += pair.second;
        }
        // sum preferred_timeslots values
        for (const auto& pair : pref.preferred_timeslots) {
            _student_weights_sums[s] += pair.second;
        }
        // sum avoid_timeslots values
        for (const auto& pair : pref.avoid_timeslots) {
            _student_weights_sums[s] += pair.second;
        }
    }

    // calculate _subject_student_count
    _subject_student_count.resize(getSubjectsNum(), 0);
    for (int s = 0; s < getStudentsNum(); ++s) {
        for (int subj : _rawData.students_subjects[s]) {
            _subject_student_count[subj]++;
        }
    }

    // print subject student counts and capacities
    std::string str = "[";
    std::string str2 = "[";
    for (int p = 0; p < getSubjectsNum(); ++p) {
        str += std::to_string(_subject_student_count[p]) + (p == getSubjectsNum() - 1 ? "" : ", ");
    }
    for (int p = 0; p < getSubjectsNum(); ++p) {
        str2 += std::to_string(_subject_total_capacity[p]) + (p == getSubjectsNum() - 1 ? "" : ", ");
    }
    str += "]";
    str2 += "]";
    Logger::info("Subject student counts: " + str + ", capacities: " + str2);

    _isFeasible = checkFeasibility();
}

int ProblemData::getAbsoluteGroupIndex(int idx_genu, int rel_group) const {
    //rel_group == gene value (relative group index for the subject from 0 to groups_per_subject[subject]-1)
    if (idx_genu < 0 || idx_genu >= getTotalStudentSubjects()) {
        throw std::runtime_error("Invalid gene index: " + std::to_string(idx_genu) + ", total: " + std::to_string(getTotalStudentSubjects()));
    }
    //szukamy przedmiotu dla danego idx_genu
    int cumulative = 0;
    int student_idx = 0;
    int local_idx = 0;
    for (; student_idx < getStudentsNum(); ++student_idx) {
        int student_size = (int)_rawData.students_subjects[student_idx].size();
        if (cumulative + student_size > idx_genu) {
            local_idx = idx_genu - cumulative;
            break;
        }
        cumulative += student_size;
    }
    int subject = _rawData.students_subjects[student_idx][local_idx];
    if (rel_group < 0 || rel_group >= _rawData.groups_per_subject[subject]) {
        throw std::runtime_error("Invalid relative group: " + std::to_string(rel_group) + " for subject " + std::to_string(subject) + ", max: " + std::to_string(_rawData.groups_per_subject[subject] - 1));
    }
    const auto& cum_groups = getCumulativeGroups();
    return cum_groups[subject] + rel_group;
}

int ProblemData::getDayFromTimeslot(int timeslot) const {
    int cum = 0;
    for (int d = 0; d < getDaysNum(); ++d) {
        cum += _rawData.timeslots_per_day[d];
        if (timeslot < cum) return d;
    }
    return -1; // invalid
}

int ProblemData::getSubjectFromGroup(int group) const {
    int cum = 0;
    for (int p = 0; p < getSubjectsNum(); ++p) {
        cum += _rawData.groups_per_subject[p];
        if (group < cum) return p;
    }
    return -1; // invalid
}

bool ProblemData::checkFeasibility() const {
    // check capacity for each subject
    for (int p = 0; p < getSubjectsNum(); ++p) {
        if (_subject_total_capacity[p] < _subject_student_count[p]) {
            Logger::warn("Subject " + std::to_string(p) + " has " + std::to_string(_subject_student_count[p]) + " students but only " + std::to_string(_subject_total_capacity[p]) + " capacity. Problem is unsolvable.");
            return false;
        }
    }

    // check if total groups <= total timeslots * rooms
    int total_groups = 0;
    for (int g : _rawData.groups_per_subject) total_groups += g;
    int num_rooms = getRoomsNum();
    if (total_groups > _total_timeslots * num_rooms) {
        Logger::warn("Total groups (" + std::to_string(total_groups) + ") exceed available timeslots * rooms (" + std::to_string(_total_timeslots) + " * " + std::to_string(num_rooms) + " = " + std::to_string(_total_timeslots * num_rooms) + "). Problem is unsolvable.");
        return false;
    }
    return true;
}
