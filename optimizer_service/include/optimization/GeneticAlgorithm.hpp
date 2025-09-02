#pragma once
#include "model/Individual.hpp"
#include "model/ProblemData.hpp"
#include "optimization/Evaluator.hpp"

class IGeneticAlgorithm {
public:
    virtual ~IGeneticAlgorithm() = default;
    virtual void Init(const ProblemData& data, const Evaluator& evaluator) = 0;
    virtual Individual RunIteration(int currentIteration) = 0;
};

class SimpleGeneticAlgorithm : public IGeneticAlgorithm {
public:
    void Init(const ProblemData& data, const Evaluator& evaluator) override;
    Individual RunIteration(int currentIteration) override;
private:
    const ProblemData* problemData = nullptr;
    const Evaluator* evaluator = nullptr;
    bool initialized = false;

    Individual bestGenotype;
};
