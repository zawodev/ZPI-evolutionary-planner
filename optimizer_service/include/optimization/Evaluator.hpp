#pragma once
#include "model/Individual.hpp"
#include "model/ProblemData.hpp"
#include <random>

class Evaluator {
public:
    Evaluator(const ProblemData& data, int seed = std::random_device{}());
    double evaluate(const Individual& genotype) const;
    int getMaxGeneValue(int geneIdx) const;
    int getTotalGenes() const { return (int)maxValues.size(); }
    std::pair<bool, Individual> repair(const Individual& individual) const;
    void initRandom(Individual& individual) const;
    const std::vector<double>& getLastStudentFitnesses() const { return lastStudentFitnesses; }
    const std::vector<double>& getLastTeacherFitnesses() const { return lastTeacherFitnesses; }
    double getLastManagementFitness() const { return lastManagementFitness; }
private:
    void buildMaxValues();
    const ProblemData& problemData;
    std::vector<int> maxValues;
    mutable std::mt19937 rng;
    mutable std::vector<double> lastStudentFitnesses;
    mutable std::vector<double> lastTeacherFitnesses;
    mutable double lastManagementFitness;
};
