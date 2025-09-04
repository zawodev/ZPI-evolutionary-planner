#pragma once
#include "model/Individual.hpp"
#include "model/ProblemData.hpp"
#include "optimization/Evaluator.hpp"
#include <string>

class JsonParser {
public:
    static ProblemData parseInput(const std::string& filename);
    static void writeOutput(const std::string& filename, const Individual& individual, const ProblemData& data, const Evaluator& evaluator);
};
