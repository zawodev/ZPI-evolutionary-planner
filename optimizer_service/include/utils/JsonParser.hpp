#pragma once
#include "model/Individual.hpp"
#include "model/ProblemData.hpp"
#include "optimization/Evaluator.hpp"
#include <string>

class JsonParser {
public:
    static RawProblemData parseInput(const std::string& filename);
    static void writeInput(const std::string& filename, const RawProblemData& data);
    static void writeOutput(const std::string& filename, const Individual& individual, const ProblemData& data, const Evaluator& evaluator);
};
