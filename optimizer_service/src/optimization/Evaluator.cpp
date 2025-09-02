#include "optimization/Evaluator.hpp"
#include <random>

double SimpleEvaluator::evaluate(const Genotype& genotype, const ProblemData& data) const {
    // mock: random fitness for demonstration and basic structure for now
    static std::mt19937 rng{std::random_device{}()};
    static std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(rng);
}
