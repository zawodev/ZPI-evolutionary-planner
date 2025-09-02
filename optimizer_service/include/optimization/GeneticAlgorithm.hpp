#pragma once
#include "model/Genotype.hpp"
#include "model/ProblemData.hpp"
#include "optimization/Evaluator.hpp"
#include <memory>

class IGeneticAlgorithm {
public:
    virtual ~IGeneticAlgorithm() = default;
    virtual Genotype run(const ProblemData& data, const Evaluator& evaluator, int maxGenerations) = 0;
};

class SimpleGeneticAlgorithm : public IGeneticAlgorithm {
public:
    Genotype run(const ProblemData& data, const Evaluator& evaluator, int maxGenerations) override;
};
