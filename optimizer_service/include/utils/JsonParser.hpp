#pragma once
#include "model/Individual.hpp"
#include "model/ProblemData.hpp"
#include "model/EventModels.hpp"
#include "optimization/Evaluator.hpp"
#include <string>
#include <nlohmann/json.hpp>

class JsonParser {
public:
    // input and output json data conversion functions
    static RawProblemData toRawProblemData(const nlohmann::json& jsonData);
    static nlohmann::json toJson(const RawProblemData& data);
    
    static RawSolutionData toRawSolutionData(const nlohmann::json& jsonData);
    static nlohmann::json toJson(const RawSolutionData& data);
    
    // event based json data conversion functions
    static RawJobData toRawJobData(const nlohmann::json& jsonData);
    static nlohmann::json toJson(const RawJobData& jobData);
    
    static RawControlData toRawControlData(const nlohmann::json& jsonData);
    static nlohmann::json toJson(const RawControlData& controlData);
    
    static RawProgressData toRawProgressData(const nlohmann::json& jsonData);
    static nlohmann::json toJson(const RawProgressData& progressData);
    
    // TODO: remove these below
    // if you find a will within yourself to do one like template kind of function to handle all of printing that would be great
    // but right now i cant be bothered to touch templates
    // because they all can be like toJson + write to file (same thing)
    static void writeInput(const std::string& filename, const RawProblemData& data);
    static void writeJobInput(const std::string& filename, const RawJobData& jobData);
    static void writeOutput(const std::string& filename, const Individual& individual, const ProblemData& data, const Evaluator& evaluator);
};
