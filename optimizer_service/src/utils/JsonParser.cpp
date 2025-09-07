#include "utils/JsonParser.hpp"
#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>
#include <stdexcept>
#include <filesystem>
#include "utils/Logger.hpp"
#include "optimization/Evaluator.hpp"

using json = nlohmann::json;
namespace fs = std::filesystem;

static std::string formatVectorOfVectors(const json& arr, int indent_level = 6) {
    std::string indent_inner = std::string(indent_level, ' ');
    std::string indent_close = std::string(indent_level - 2, ' ');
    std::string result = "[\n";
    for (size_t i = 0; i < arr.size(); ++i) {
        result += indent_inner + arr[i].dump();
        if (i < arr.size() - 1) result += ",";
        result += "\n";
    }
    result += indent_close + "]";
    return result;
}

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

static std::map<int, int> parse_pref_map(const json& obj) {
    std::map<int, int> m;
    for (auto it = obj.begin(); it != obj.end(); ++it) {
        m[std::stoi(it.key())] = it.value().get<int>();
    }
    return m;
}

static std::vector<RoomTimeslotPreference> parse_management_room_timeslots(const json& arr) {
    std::vector<RoomTimeslotPreference> out;
    for (const auto& m : arr) {
        RoomTimeslotPreference mp;
        mp.room = m["room"].get<int>();
        mp.timeslot = m["timeslot"].get<int>();
        mp.weight = m["weight"].get<int>();
        out.push_back(mp);
    }
    return out;
}

RawProblemData JsonParser::parseInput(const std::string& filename) {
    try {
        std::ifstream in(filename);
        if (!in) {
            throw std::runtime_error("Cannot open file: " + filename);
        }
        json j;
        in >> j;
        RawProblemData data;
        // constraints
        const auto& c = j.at("constraints");
        data.timeslots_per_day = c.at("timeslots_per_day").get<std::vector<int>>();
        data.groups_per_subject = c.at("groups_per_subject").get<std::vector<int>>();
        data.groups_soft_capacity = c.at("groups_soft_capacity").get<std::vector<int>>();
        data.students_subjects = c.at("students_subjects").get<std::vector<std::vector<int>>>();
        data.teachers_groups = c.at("teachers_groups").get<std::vector<std::vector<int>>>();
        data.rooms_unavailability_timeslots = c.at("rooms_unavailability_timeslots").get<std::vector<std::vector<int>>>();
        // preferences
        const auto& p = j.at("preferences");
        if (p.contains("students")) {
            for (const auto& s : p.at("students")) {
                StudentPreference sp;
                sp.free_days = s.at("free_days").get<std::vector<int>>();
                sp.busy_days = s.at("busy_days").get<std::vector<int>>();
                sp.gaps.value = s.at("gaps").at("value").get<bool>();
                sp.gaps.weight = s.at("gaps").at("weight").get<int>();
                sp.preferred_groups = parse_pref_map(s.at("preferred_groups"));
                sp.avoid_groups = parse_pref_map(s.at("avoid_groups"));
                sp.preferred_timeslots = parse_pref_map(s.at("preferred_timeslots"));
                sp.avoid_timeslots = parse_pref_map(s.at("avoid_timeslots"));
                data.students_preferences.push_back(sp);
            }
        }
        if (p.contains("teachers")) {
            for (const auto& t : p.at("teachers")) {
                TeacherPreference tp;
                tp.free_days = t.at("free_days").get<std::vector<int>>();
                tp.busy_days = t.at("busy_days").get<std::vector<int>>();
                tp.gaps.value = t.at("gaps").at("value").get<bool>();
                tp.gaps.weight = t.at("gaps").at("weight").get<int>();
                tp.preferred_timeslots = parse_pref_map(t.at("preferred_timeslots"));
                tp.avoid_timeslots = parse_pref_map(t.at("avoid_timeslots"));
                data.teachers_preferences.push_back(tp);
            }
        }
        if (p.contains("management")) {
            if (p.at("management").contains("preferred_room_timeslots"))
                data.management_preferences.preferred_room_timeslots = parse_management_room_timeslots(p.at("management").at("preferred_room_timeslots"));
            if (p.at("management").contains("avoid_room_timeslots"))
                data.management_preferences.avoid_room_timeslots = parse_management_room_timeslots(p.at("management").at("avoid_room_timeslots"));
            if (p.at("management").contains("group_max_overflow")) {
                data.management_preferences.group_max_overflow.value = p.at("management").at("group_max_overflow").at("value").get<int>();
                data.management_preferences.group_max_overflow.weight = p.at("management").at("group_max_overflow").at("weight").get<int>();
            }
        }
        return data;
    } catch (const nlohmann::json::exception& e) {
        throw std::runtime_error(std::string("JSON structure error: ") + e.what());
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("JSON parse error: ") + e.what());
    }
}

void JsonParser::writeOutput(const std::string& filename, const Individual& individual, const ProblemData& data, const Evaluator& evaluator) {
    try {
        fs::path outPath(filename);
        if (!outPath.parent_path().empty() && !fs::exists(outPath.parent_path())) {
            fs::create_directories(outPath.parent_path());
        }
        std::ofstream out(filename);
        if (!out) {
            throw std::runtime_error("Cannot open file for writing: " + filename);
        }

        double fitness = evaluator.evaluate(individual);

        json j;
        int studentsNum = data.getStudentsNum();
        int groupsNum = data.getGroupsNum();

        // Validate genotype size
        int expected_size = evaluator.getTotalGenes();
        if (individual.genotype.size() != expected_size) {
            throw std::runtime_error("Invalid genotype size: " + std::to_string(individual.genotype.size()) + ", expected: " + std::to_string(expected_size));
        }

        std::vector<std::vector<int>> by_student;
        int idx = 0;
        for (int s = 0; s < studentsNum; ++s) {
            std::vector<int> student_groups;
            int num_groups_for_student = data.getGroupsForStudent(s);
            for (int g = 0; g < num_groups_for_student; ++g) {
                student_groups.push_back(individual.genotype[idx++]);
            }
            by_student.push_back(student_groups);
        }

        std::vector<std::vector<int>> by_group;
        for (int g = 0; g < groupsNum; ++g) {
            int timeslot = individual.genotype[idx++];
            int room = individual.genotype[idx++];
            by_group.push_back({timeslot, room});
        }

        j["genotype"] = individual.genotype;
        j["fitness"] = fitness;
        j["by_student"] = by_student;
        j["by_group"] = by_group;
        j["student_fitnesses"] = evaluator.getLastStudentFitnesses();
        j["teacher_fitnesses"] = evaluator.getLastTeacherFitnesses();
        j["management_fitness"] = evaluator.getLastManagementFitness();

        //old style (ugly print)
        //out << j.dump(2);

        //manual pretty print to avoid issues with large arrays in one line
        std::string json_str = "{\n";
        json_str += "  \"genotype\": " + j["genotype"].dump() + ",\n";
        json_str += "  \"fitness\": " + std::to_string(j["fitness"].get<double>()) + ",\n";
        json_str += "  \"by_student\": " + formatVectorOfVectors(json(by_student), 4) + ",\n";
        json_str += "  \"by_group\": " + formatVectorOfVectors(json(by_group), 4) + ",\n";
        json_str += "  \"student_fitnesses\": " + j["student_fitnesses"].dump() + ",\n";
        json_str += "  \"teacher_fitnesses\": " + j["teacher_fitnesses"].dump() + ",\n";
        json_str += "  \"management_fitness\": " + std::to_string(j["management_fitness"].get<double>()) + "\n";
        json_str += "}\n";
        out << json_str;
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("JSON write error: ") + e.what());
    }
}

void JsonParser::writeInput(const std::string& filename, const RawProblemData& data) {
    try {
        fs::path outPath(filename);
        if (!outPath.parent_path().empty() && !fs::exists(outPath.parent_path())) {
            fs::create_directories(outPath.parent_path());
        }
        std::ofstream out(filename);
        if (!out) {
            throw std::runtime_error("Cannot open file for writing: " + filename);
        }
        json j;
        // constraints
        j["constraints"]["timeslots_per_day"] = data.timeslots_per_day;
        j["constraints"]["groups_per_subject"] = data.groups_per_subject;
        j["constraints"]["groups_soft_capacity"] = data.groups_soft_capacity;
        j["constraints"]["students_subjects"] = data.students_subjects;
        j["constraints"]["teachers_groups"] = data.teachers_groups;
        j["constraints"]["rooms_unavailability_timeslots"] = data.rooms_unavailability_timeslots;
        // preferences
        if (!data.students_preferences.empty()) {
            for (const auto& sp : data.students_preferences) {
                json s;
                s["free_days"] = sp.free_days;
                s["busy_days"] = sp.busy_days;
                s["gaps"]["value"] = sp.gaps.value;
                s["gaps"]["weight"] = sp.gaps.weight;
                s["preferred_groups"] = json::object();
                for (const auto& p : sp.preferred_groups) {
                    s["preferred_groups"][std::to_string(p.first)] = p.second;
                }
                s["avoid_groups"] = json::object();
                for (const auto& p : sp.avoid_groups) {
                    s["avoid_groups"][std::to_string(p.first)] = p.second;
                }
                s["preferred_timeslots"] = json::object();
                for (const auto& p : sp.preferred_timeslots) {
                    s["preferred_timeslots"][std::to_string(p.first)] = p.second;
                }
                s["avoid_timeslots"] = json::object();
                for (const auto& p : sp.avoid_timeslots) {
                    s["avoid_timeslots"][std::to_string(p.first)] = p.second;
                }
                j["preferences"]["students"].push_back(s);
            }
        }
        if (!data.teachers_preferences.empty()) {
            for (const auto& tp : data.teachers_preferences) {
                json t;
                t["free_days"] = tp.free_days;
                t["busy_days"] = tp.busy_days;
                t["gaps"]["value"] = tp.gaps.value;
                t["gaps"]["weight"] = tp.gaps.weight;
                t["preferred_timeslots"] = json::object();
                for (const auto& p : tp.preferred_timeslots) {
                    t["preferred_timeslots"][std::to_string(p.first)] = p.second;
                }
                t["avoid_timeslots"] = json::object();
                for (const auto& p : tp.avoid_timeslots) {
                    t["avoid_timeslots"][std::to_string(p.first)] = p.second;
                }
                j["preferences"]["teachers"].push_back(t);
            }
        }
        if (!data.management_preferences.preferred_room_timeslots.empty() ||
            !data.management_preferences.avoid_room_timeslots.empty() ||
            data.management_preferences.group_max_overflow.weight != 0) {
            if (!data.management_preferences.preferred_room_timeslots.empty()) {
                for (const auto& mp : data.management_preferences.preferred_room_timeslots) {
                    json obj;
                    obj["room"] = mp.room;
                    obj["timeslot"] = mp.timeslot;
                    obj["weight"] = mp.weight;
                    j["preferences"]["management"]["preferred_room_timeslots"].push_back(obj);
                }
            }
            if (!data.management_preferences.avoid_room_timeslots.empty()) {
                for (const auto& mp : data.management_preferences.avoid_room_timeslots) {
                    json obj;
                    obj["room"] = mp.room;
                    obj["timeslot"] = mp.timeslot;
                    obj["weight"] = mp.weight;
                    j["preferences"]["management"]["avoid_room_timeslots"].push_back(obj);
                }
            }
            if (data.management_preferences.group_max_overflow.weight != 0) {
                j["preferences"]["management"]["group_max_overflow"]["value"] = data.management_preferences.group_max_overflow.value;
                j["preferences"]["management"]["group_max_overflow"]["weight"] = data.management_preferences.group_max_overflow.weight;
            }
        }
        //old style (ugly print)
        //out << j.dump(2);

        //manual pretty print to avoid issues with large arrays in one line
        std::string json_str = "{\n";
        json_str += "  \"constraints\": {\n";
        json_str += "    \"timeslots_per_day\": " + j["constraints"]["timeslots_per_day"].dump() + ",\n";
        json_str += "    \"groups_per_subject\": " + j["constraints"]["groups_per_subject"].dump() + ",\n";
        json_str += "    \"groups_soft_capacity\": " + j["constraints"]["groups_soft_capacity"].dump() + ",\n";
        json_str += "    \"students_subjects\": " + formatVectorOfVectors(j["constraints"]["students_subjects"]) + ",\n";
        json_str += "    \"teachers_groups\": " + formatVectorOfVectors(j["constraints"]["teachers_groups"]) + ",\n";
        json_str += "    \"rooms_unavailability_timeslots\": " + formatVectorOfVectors(j["constraints"]["rooms_unavailability_timeslots"]) + "\n";
        json_str += "  }";
        if (j.contains("preferences")) {
            json_str += ",\n  \"preferences\": " + j["preferences"].dump(2);
        }
        json_str += "\n}\n";
        out << json_str;
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("JSON write error: ") + e.what());
    }
}
