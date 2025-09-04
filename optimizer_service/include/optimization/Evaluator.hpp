#pragma once
#include "model/Individual.hpp"
#include "model/ProblemData.hpp"

class Evaluator {
public:
    explicit Evaluator(const ProblemData& data);
    double evaluate(const Individual& genotype) const;
    int getMaxGeneValue(int geneIdx) const;
    int getTotalGenes() const { return (int)maxValues.size(); }
    std::pair<bool, Individual> repair(const Individual& individual) const;
    void initRandom(Individual& individual, std::mt19937& rng) const;
private:
    void buildMaxValues();
    const ProblemData& problemData;
    std::vector<int> maxValues;
};
