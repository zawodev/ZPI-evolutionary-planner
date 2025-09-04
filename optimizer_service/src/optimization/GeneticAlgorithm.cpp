#include "optimization/GeneticAlgorithm.hpp"
#include "utils/Logger.hpp"
#include <random>

void SimpleGeneticAlgorithm::Init(const ProblemData& data, const Evaluator& evaluator) {
    this->problemData = &data;
    this->evaluator = &evaluator;
    bestIndividual = Individual{};
    initialized = true;

    // random number generator
    std::mt19937 rng{std::random_device{}()};

    // inicjalizacja genotypu (rozmiar na evaluator.getTotalGenes() oraz wartości na rng() % evaluator.getMaxGeneValue(i))
    bestIndividual.genotype.clear();
    for (int i = 0; i < evaluator.getTotalGenes(); ++i) {
        bestIndividual.genotype.push_back(rng() % (evaluator.getMaxGeneValue(i) + 1));
    }

    // obliczanie fitnessu
    bestIndividual.fitness = evaluator.evaluate(bestIndividual);
}

Individual SimpleGeneticAlgorithm::RunIteration(int currentIteration) {
    if (!initialized || !problemData || !evaluator) {
        throw std::runtime_error("SimpleGeneticAlgorithm not initialized properly");
    }
    
    auto [isValid, repaired] = evaluator->repair(bestIndividual);
    if (!isValid) {
        bestIndividual.printDebugInfo("old");
        repaired.printDebugInfo("repaired");
        
        // użyj repaired jako poprawionego genotypu
    }

    // Przykładowa logika: fitness losowy, można dodać mutacje/cross cokolwiek takiego genetycznego później
    bestIndividual.fitness = evaluator->evaluate(bestIndividual);
    return bestIndividual;
}
