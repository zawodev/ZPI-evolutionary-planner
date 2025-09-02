#pragma once
#include "model/Genotype.hpp"
#include "model/ProblemData.hpp"

class Evaluator {
public:
    virtual ~Evaluator() = default;
    virtual double evaluate(const Genotype& genotype, const ProblemData& data) const = 0;
};

class SimpleEvaluator : public Evaluator {
public:
    double evaluate(const Genotype& genotype, const ProblemData& data) const override;
};
