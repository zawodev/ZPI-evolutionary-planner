#include "utils/JsonParser.hpp"
#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>
#include <stdexcept>
#include <filesystem>

using json = nlohmann::json;
namespace fs = std::filesystem;

static std::vector<std::map<int, int>> parse_pref_array(const json& arr) {
    std::vector<std::map<int, int>> out;
    for (const auto& el : arr) {
        std::map<int, int> m;
        for (auto it = el.begin(); it != el.end(); ++it) {
            m[std::stoi(it.key())] = it.value().get<int>();
        }
        out.push_back(m);
    }
    return out;
}

static std::vector<ManagementPreference> parse_management(const json& arr) {
    std::vector<ManagementPreference> out;
    for (const auto& m : arr) {
        ManagementPreference mp;
        mp.room = m["room"].get<int>();
        mp.timeslot = m["timeslot"].get<int>();
        mp.weight = m["weight"].get<int>();
        out.push_back(mp);
    }
    return out;
}

ProblemData JsonParser::parseInput(const std::string& filename) {
    try {
        std::ifstream in(filename);
        if (!in) {
            throw std::runtime_error("Cannot open file: " + filename);
        }
        json j;
        in >> j;
        ProblemData data;
        // constraints
        const auto& c = j["constraints"];
        data.timeslots_per_day = c["timeslots_per_day"].get<std::vector<int>>();
        data.groups_per_subject = c["groups_per_subject"].get<std::vector<int>>();
        data.students_subjects = c["students_subjects"].get<std::vector<std::vector<int>>>();
        data.teachers_groups = c["teachers_groups"].get<std::vector<std::vector<int>>>();
        data.rooms_unavailability = c["rooms_unavailability"].get<std::vector<std::vector<int>>>();
        // preferences
        const auto& p = j["preferences"];
        if (p.contains("students")) {
            for (const auto& s : p["students"]) {
                StudentPreference sp;
                sp.free_days = parse_pref_array(s["free_days"]);
                sp.busy_days = parse_pref_array(s["busy_days"]);
                sp.gaps.value = s["gaps"]["value"].get<bool>();
                sp.gaps.weight = s["gaps"]["weight"].get<int>();
                sp.preferred_groups = parse_pref_array(s["preferred_groups"]);
                sp.avoid_groups = parse_pref_array(s["avoid_groups"]);
                sp.preferred_timeslots = parse_pref_array(s["preferred_timeslots"]);
                sp.avoid_timeslots = parse_pref_array(s["avoid_timeslots"]);
                data.students_preferences.push_back(sp);
            }
        }
        if (p.contains("teachers")) {
            for (const auto& t : p["teachers"]) {
                TeacherPreference tp;
                tp.free_days = parse_pref_array(t["free_days"]);
                tp.busy_days = parse_pref_array(t["busy_days"]);
                tp.gaps.value = t["gaps"]["value"].get<bool>();
                tp.gaps.weight = t["gaps"]["weight"].get<int>();
                tp.preferred_timeslots = parse_pref_array(t["preferred_timeslots"]);
                tp.avoid_timeslots = parse_pref_array(t["avoid_timeslots"]);
                data.teachers_preferences.push_back(tp);
            }
        }
        if (p.contains("management")) {
            if (p["management"].contains("prefer"))
                data.management_preferences.prefer = parse_management(p["management"]["prefer"]);
            if (p["management"].contains("avoid"))
                data.management_preferences.avoid = parse_management(p["management"]["avoid"]);
        }
        return data;
    } catch (const std::exception& e) {
        std::cerr << "JSON parse error in file '" << filename << "': " << e.what() << std::endl;
        throw;
    }
}

void JsonParser::writeOutput(const std::string& filename, const Genotype& genotype) {
	// ensure parent directory exists (if any)
    fs::path outPath(filename);
    if (!outPath.parent_path().empty() && !fs::exists(outPath.parent_path())) {
        try {
            fs::create_directories(outPath.parent_path());
        } catch (const std::exception& e) {
            std::cerr << "Cannot create directory for output file: " << outPath.parent_path() << " - " << e.what() << std::endl;
            return;
        }
    }
    std::ofstream out(filename);
    if (!out) {
        std::cerr << "Cannot open file for writing: " << filename << std::endl;
        return;
    }
    json j;
    // surowe dane
    j["raw"] = {
        {"student_group_assignments", genotype.student_group_assignments},
        {"group_timeslot_room_assignments", genotype.group_timeslot_room_assignments}
    };
    // sformatowane dane
    j["by_student"] = genotype.student_group_assignments_by_student;
    j["by_group"] = genotype.group_timeslot_room_assignments_by_group;
    j["fitness"] = genotype.fitness;
    out << j.dump(2);
}
