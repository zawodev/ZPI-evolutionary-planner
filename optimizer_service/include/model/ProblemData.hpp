#pragma once
#include <vector>
#include <map>
#include <string>

struct GapsPreference {
    bool value = false;
    int weight = 0;
};

struct StudentPreference {
    std::vector<std::map<int, int>> free_days; // [{day_id: weight}]
    std::vector<std::map<int, int>> busy_days; // [{day_id: weight}]
    GapsPreference gaps;
    std::vector<std::map<int, int>> preferred_groups; // [{group_id: weight}]
    std::vector<std::map<int, int>> avoid_groups; // [{group_id: weight}]
    std::vector<std::map<int, int>> preferred_timeslots; // [{timeslot_id: weight}]
    std::vector<std::map<int, int>> avoid_timeslots; // [{timeslot_id: weight}]
};

struct TeacherPreference {
    std::vector<std::map<int, int>> free_days;
    std::vector<std::map<int, int>> busy_days;
    GapsPreference gaps;
    std::vector<std::map<int, int>> preferred_timeslots;
    std::vector<std::map<int, int>> avoid_timeslots;
};

struct ManagementPreference {
    int room;
    int timeslot;
    int weight;
};

struct ManagementPreferences {
    std::vector<ManagementPreference> prefer;
    std::vector<ManagementPreference> avoid;
};

struct ProblemData {
    // Constraints
    std::vector<int> timeslots_per_day;
    std::vector<int> groups_per_subject;
    std::vector<std::vector<int>> students_subjects;
    std::vector<std::vector<int>> teachers_groups;
    std::vector<std::vector<int>> rooms_unavailability;

    // Preferences
    std::vector<StudentPreference> students_preferences;
    std::vector<TeacherPreference> teachers_preferences;
    ManagementPreferences management_preferences;
};
