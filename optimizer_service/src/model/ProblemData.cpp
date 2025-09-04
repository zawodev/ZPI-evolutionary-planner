#include "model/ProblemData.hpp"
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
        // sum gaps weight
        _student_weights_sums[s] += pref.gaps.weight;
        // sum preferred_groups values
        for (const auto& map : pref.preferred_groups) {
            for (const auto& pair : map) {
                _student_weights_sums[s] += pair.second;
            }
        }
        // sum avoid_groups values
        for (const auto& map : pref.avoid_groups) {
            for (const auto& pair : map) {
                _student_weights_sums[s] += pair.second;
            }
        }
        // sum preferred_timeslots values
        for (const auto& map : pref.preferred_timeslots) {
            for (const auto& pair : map) {
                _student_weights_sums[s] += pair.second;
            }
        }
        // sum avoid_timeslots values
        for (const auto& map : pref.avoid_timeslots) {
            for (const auto& pair : map) {
                _student_weights_sums[s] += pair.second;
            }
        }
    }
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
