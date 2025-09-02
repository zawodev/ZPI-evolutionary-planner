#pragma once
#include <vector>

struct Individual {
    std::vector<int> genotype;
    double fitness = 0.0;
};
