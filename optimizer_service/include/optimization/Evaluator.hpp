#pragma once
#include "model/Individual.hpp"
#include "model/ProblemData.hpp"

class Evaluator {
public:
    explicit Evaluator(const ProblemData& data);
    double evaluate(const Individual& genotype) const;
    int getMaxGeneValue(int geneIdx) const;
    std::pair<bool, Individual> repair(const Individual& individual) const;
private:
    void buildMaxValues();
    const ProblemData& problemData;
    std::vector<int> maxValues;
};
