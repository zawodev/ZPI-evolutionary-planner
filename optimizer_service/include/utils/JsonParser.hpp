#pragma once
#include "model/Genotype.hpp"
#include "model/ProblemData.hpp"

class JsonParser {
public:
    static ProblemData parseInput(const std::string& filename);
    static void writeOutput(const std::string& filename, const Genotype& genotype);
};
