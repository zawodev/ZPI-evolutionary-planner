#include "optimization/GeneticAlgorithm.hpp"
#include "utils/Logger.hpp"
#include <random>

void SimpleGeneticAlgorithm::Init(const ProblemData& data, const Evaluator& evaluator, int seed) {
    this->problemData = &data;
    this->evaluator = &evaluator;
    bestIndividual = Individual{};
    initialized = true;

    // inicjalizacja RNG z seed
    rng.seed(seed);

    // inicjalizacja genotypu (rozmiar na evaluator.getTotalGenes() oraz wartości na rng() % evaluator.getMaxGeneValue(i))
    evaluator.initRandom(bestIndividual);

    // obliczanie fitnessu
    bestIndividual.fitness = evaluator.evaluate(bestIndividual);
}

Individual SimpleGeneticAlgorithm::RunIteration(int currentIteration) {
    if (!initialized || !problemData || !evaluator) {
        throw std::runtime_error("SimpleGeneticAlgorithm not initialized properly");
    }
    
    auto [isValid, repaired] = evaluator->repair(bestIndividual);
    if (!isValid) {
        Logger::debug("Best individual was invalid and has been repaired.");
        bestIndividual.printDebugInfo("old");
        repaired.printDebugInfo("repaired");
        // użyj repaired jako poprawionego genotypu
    }

    // Przykładowa logika: fitness losowy, można dodać mutacje/cross cokolwiek takiego genetycznego później
    evaluator->initRandom(bestIndividual);
    bestIndividual.fitness = evaluator->evaluate(bestIndividual);

    return bestIndividual;
}
