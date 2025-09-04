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

struct RoomTimeslotPreference {
    int room;
    int timeslot;
    int weight;
};

struct ManagementPreferences {
    std::vector<RoomTimeslotPreference> preferred_room_timeslots;
    std::vector<RoomTimeslotPreference> avoid_room_timeslots;
    struct { int value; int weight; } group_max_overflow;
};

struct RawProblemData {
    //Constraints
    std::vector<int> timeslots_per_day;
    std::vector<int> groups_per_subject;
    std::vector<int> groups_soft_capacity;
    std::vector<std::vector<int>> students_subjects;
    std::vector<std::vector<int>> teachers_groups;
    std::vector<std::vector<int>> rooms_unavailability_timeslots;

    //Preferences
    std::vector<StudentPreference> students_preferences;
    std::vector<TeacherPreference> teachers_preferences;
    ManagementPreferences management_preferences;
};

class ProblemData {
private:
    RawProblemData _rawData;

    int _total_timeslots;
    int _total_student_subjects;
    std::vector<int> _subject_total_capacity;
    std::vector<int> _cumulative_groups;
    std::vector<int> _student_weights_sums;

public:
    ProblemData(const RawProblemData& input_data);

    // getters for raw data
    const std::vector<int>& getTimeslotsPerDay() const { return _rawData.timeslots_per_day; }
    const std::vector<int>& getGroupsPerSubject() const { return _rawData.groups_per_subject; }
    const std::vector<int>& getGroupsSoftCapacity() const { return _rawData.groups_soft_capacity; }
    const std::vector<std::vector<int>>& getStudentsSubjects() const { return _rawData.students_subjects; }
    const std::vector<std::vector<int>>& getTeachersGroups() const { return _rawData.teachers_groups; }
    const std::vector<std::vector<int>>& getRoomsUnavailabilityTimeslots() const { return _rawData.rooms_unavailability_timeslots; }
    const std::vector<StudentPreference>& getStudentsPreferences() const { return _rawData.students_preferences; }
    const std::vector<TeacherPreference>& getTeachersPreferences() const { return _rawData.teachers_preferences; }
    const ManagementPreferences& getManagementPreferences() const { return _rawData.management_preferences; }

    // calculated fields
    int getDaysNum() const { return (int)_rawData.timeslots_per_day.size(); }
    int getSubjectsNum() const { return (int)_rawData.groups_per_subject.size(); }
    int getGroupsNum() const { return (int)_rawData.groups_soft_capacity.size(); }
    int getStudentsNum() const { return (int)_rawData.students_subjects.size(); }
    int getTeachersNum() const { return (int)_rawData.teachers_groups.size(); }
    int getRoomsNum() const { return (int)_rawData.rooms_unavailability_timeslots.size(); }
    int getGroupsForStudent(int studentId) const { return (int)_rawData.students_subjects[studentId].size(); }

    int totalTimeslots() const { return _total_timeslots; }
    int getTotalStudentSubjects() const { return _total_student_subjects; }
    const std::vector<int>& getSubjectTotalCapacity() const { return _subject_total_capacity; }
    const std::vector<int>& getCumulativeGroups() const { return _cumulative_groups; }
    const std::vector<int>& getStudentWeightsSums() const { return _student_weights_sums; }

    // more complex functions
    int getAbsoluteGroupIndex(int idx_genu, int rel_group) const;
    int getDayFromTimeslot(int timeslot) const;
};
