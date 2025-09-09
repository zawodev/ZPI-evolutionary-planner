#include "utils/JsonParser.hpp"
#include "model/EventModels.hpp"
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




// --------------- input and output json data conversion functions ---------------




RawProblemData JsonParser::toRawProblemData(const nlohmann::json& j) {
    try {
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
                sp.no_gaps = s.at("no_gaps").get<int>();
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
                tp.no_gaps = t.at("no_gaps").get<int>();
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

nlohmann::json JsonParser::toJson(const RawProblemData& data) {
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
            s["no_gaps"] = sp.no_gaps;
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
            t["no_gaps"] = tp.no_gaps;
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
    return j;
}

RawSolutionData JsonParser::toRawSolutionData(const nlohmann::json& jsonData) {
    try {
        RawSolutionData solutionData;
        solutionData.genotype = jsonData.at("genotype").get<std::vector<int>>();
        solutionData.fitness = jsonData.at("fitness").get<double>();
        solutionData.by_student = jsonData.at("by_student").get<std::vector<std::vector<int>>>();
        solutionData.by_group = jsonData.at("by_group").get<std::vector<std::vector<int>>>();
        solutionData.student_fitnesses = jsonData.at("student_fitnesses").get<std::vector<double>>();
        solutionData.teacher_fitnesses = jsonData.at("teacher_fitnesses").get<std::vector<double>>();
        solutionData.management_fitness = jsonData.at("management_fitness").get<double>();
        return solutionData;
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to parse solution data: " + std::string(e.what()));
    }
}

nlohmann::json JsonParser::toJson(const RawSolutionData& data) {
    json j;
    j["genotype"] = data.genotype;
    j["fitness"] = data.fitness;
    j["by_student"] = data.by_student;
    j["by_group"] = data.by_group;
    j["student_fitnesses"] = data.student_fitnesses;
    j["teacher_fitnesses"] = data.teacher_fitnesses;
    j["management_fitness"] = data.management_fitness;
    return j;
}




// --------------- event based json data conversion functions ---------------




RawJobData JsonParser::toRawJobData(const nlohmann::json& jsonData) {
    try {
        RawJobData jobData;
        jobData.job_id = jsonData.at("job_id").get<std::string>();
        jobData.max_execution_time = jsonData.value("max_execution_time", 300);
        jobData.problem_data = toRawProblemData(jsonData.at("problem_data"));
        return jobData;
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to parse job data: " + std::string(e.what()));
    }
}

nlohmann::json JsonParser::toJson(const RawJobData& jobData) {
    json j;
    j["job_id"] = jobData.job_id;
    j["max_execution_time"] = jobData.max_execution_time;
    j["problem_data"] = toJson(jobData.problem_data);
    return j;
}

RawControlData JsonParser::toRawControlData(const nlohmann::json& jsonData) {
    try {
        RawControlData controlData;
        controlData.job_id = jsonData.at("job_id").get<std::string>();
        controlData.action = jsonData.at("action").get<std::string>();
        controlData.data = jsonData.value("data", json::object());
        return controlData;
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to parse control data: " + std::string(e.what()));
    }
}

nlohmann::json JsonParser::toJson(const RawControlData& controlData) {
    json j;
    j["job_id"] = controlData.job_id;
    j["action"] = controlData.action;
    j["data"] = controlData.data;
    return j;
}

RawProgressData JsonParser::toRawProgressData(const nlohmann::json& jsonData) {
    try {
        RawProgressData progressData;
        progressData.job_id = jsonData.at("job_id").get<std::string>();
        progressData.iteration = jsonData.at("iteration").get<int>();
        progressData.best_solution = toRawSolutionData(jsonData.at("best_solution"));
        return progressData;
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to parse progress data: " + std::string(e.what()));
    }
}

nlohmann::json JsonParser::toJson(const RawProgressData& progressData) {
    json j;
    j["job_id"] = progressData.job_id;
    j["iteration"] = progressData.iteration;
    j["best_solution"] = toJson(progressData.best_solution);
    return j;
}




// --------------- below to be removed ---------------




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
        
        json j = toJson(data);
        
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

void JsonParser::writeJobInput(const std::string& filename, const RawJobData& jobData) {
    try {
        fs::path outPath(filename);
        if (!outPath.parent_path().empty() && !fs::exists(outPath.parent_path())) {
            fs::create_directories(outPath.parent_path());
        }
        std::ofstream out(filename);
        if (!out) {
            throw std::runtime_error("Cannot open file for writing: " + filename);
        }
        
        json j = toJson(jobData);
        out << j.dump(2);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("JSON write error: ") + e.what());
    }
}

void JsonParser::writeOutput(const std::string& filename, const Individual& individual, const ProblemData& data, const Evaluator& evaluator) {
    try {
        // Create RawSolutionData from Individual
        RawSolutionData solutionData(individual, data, evaluator);
        
        fs::path outPath(filename);
        if (!outPath.parent_path().empty() && !fs::exists(outPath.parent_path())) {
            fs::create_directories(outPath.parent_path());
        }
        std::ofstream out(filename);
        if (!out) {
            throw std::runtime_error("Cannot open file for writing: " + filename);
        }

        json j = toJson(solutionData);

        //manual pretty print to avoid issues with large arrays in one line
        std::string json_str = "{\n";
        json_str += "  \"genotype\": " + j["genotype"].dump() + ",\n";
        json_str += "  \"fitness\": " + std::to_string(j["fitness"].get<double>()) + ",\n";
        json_str += "  \"by_student\": " + formatVectorOfVectors(j["by_student"], 4) + ",\n";
        json_str += "  \"by_group\": " + formatVectorOfVectors(j["by_group"], 4) + ",\n";
        json_str += "  \"student_fitnesses\": " + j["student_fitnesses"].dump() + ",\n";
        json_str += "  \"teacher_fitnesses\": " + j["teacher_fitnesses"].dump() + ",\n";
        json_str += "  \"management_fitness\": " + std::to_string(j["management_fitness"].get<double>()) + "\n";
        json_str += "}\n";
        out << json_str;
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("JSON write error: ") + e.what());
    }
}
