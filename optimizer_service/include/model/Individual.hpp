#pragma once
#include <vector>
#include <random>
#include "utils/Logger.hpp"

struct Individual {
    std::vector<int> genotype;
    double fitness = 0.0;

    void printDebugInfo(const std::string& name = "") const;
};
