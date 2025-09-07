#pragma once
#include "model/Individual.hpp"
#include "model/ProblemData.hpp"
#include "optimization/Evaluator.hpp"
#include "optimization/IGeneticAlgorithm.hpp"

class ZawodevGeneticAlgorithm : public IGeneticAlgorithm {
public:
    void Init(const ProblemData& data, const Evaluator& evaluator, int seed = std::random_device{}()) override;
    Individual RunIteration(int currentIteration) override;
private:
    // system variables
    const ProblemData* problemData = nullptr;
    const Evaluator* evaluator = nullptr;
    mutable std::mt19937 rng;
    bool initialized = false;

    // algorithm variables
    Individual bestIndividual;
    std::vector<Individual> population;
    int populationSize = 10;
    int crossSize = 5;
    
    // functions
    void initRandom(Individual& individual) const;
    void fihc(Individual& individual);
    void selection();

};