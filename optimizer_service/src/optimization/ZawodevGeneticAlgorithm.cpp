#include "optimization/ZawodevGeneticAlgorithm.hpp"
#include "utils/Logger.hpp"
#include <random>
#include <algorithm>
#include <numeric>

void ZawodevGeneticAlgorithm::Init(const ProblemData& data, const Evaluator& evaluator, int seed) {
    this->problemData = &data;
    this->evaluator = &evaluator;
    bestIndividual = Individual{};
    initialized = true;

    // inicjalizacja RNG z seed
    rng.seed(seed);

    // inicjalizacja genotypu (rozmiar na evaluator.getTotalGenes() oraz wartości na rng() % evaluator.getMaxGeneValue(i))
    this->initRandom(bestIndividual);

    //init population
    population.clear();
    for (int i = 0; i < populationSize; ++i) {
        Individual ind;
        this->initRandom(ind);
        population.push_back(ind);
    }

    // debug info
    Logger::debug("Start fitness: " + std::to_string(bestIndividual.fitness));
}

Individual ZawodevGeneticAlgorithm::RunIteration(int currentIteration) {
    if (!initialized || !problemData || !evaluator) {
        throw std::runtime_error("SimpleGeneticAlgorithm not initialized properly");
    }
    /*
    for (int i = 0; i < 1000; ++i) {
        //this->initRandom(individual);
        //this->fihc(individual);

        //assert if individual fitness == evaluator.evaluate(individual) (remove rounding errors?)
        //if (std::abs(individual.fitness - evaluator->evaluate(individual)) > 1e-6) {
            //Logger::error("Fitness mismatch after FIHC: " + std::to_string(individual.fitness) + " vs " + std::to_string(evaluator->evaluate(individual)));
            //exit(1);
        //}

        //exit(1);
        if (population[0].fitness > bestIndividual.fitness) {
            bestIndividual = population[0];
        }
    }
    */
    for (int i = 0; i < 100; ++i) {
        for (int j = 0; j < populationSize; ++j) {
            this->fihc(population[j]);
        }
        this->selection();
        if (population[0].fitness > bestIndividual.fitness) {
            bestIndividual = population[0];
            Logger::debug("New best fitness: " + std::to_string(bestIndividual.fitness));
        }
    }

    return bestIndividual;
}

void ZawodevGeneticAlgorithm::initRandom(Individual& individual) const {
    individual.genotype.clear();
    for (int i = 0; i < evaluator->getTotalGenes(); ++i) {
        std::uniform_int_distribution<int> dist(0, evaluator->getMaxGeneValue(i));
        individual.genotype.push_back(dist(rng));
    }

    bool wasRepaired = evaluator->repair(individual);
    if (wasRepaired) {
        //Logger::debug("Individual's genotype was repaired. Now solution is valid.");
    }

    // still debating myself if we should evaluate here or not
    individual.fitness = evaluator->evaluate(individual);
}

void ZawodevGeneticAlgorithm::fihc(Individual &individual) {
    // to be improved, very basic very bad definition of FIHC
    // now with random order of genes
    //Logger::debug("Before FIHC: ");
    //individual.printDebugInfo();

    // Create a vector of gene indices and shuffle it for random order
    std::vector<size_t> geneIndices(individual.genotype.size());
    std::iota(geneIndices.begin(), geneIndices.end(), 0);
    std::shuffle(geneIndices.begin(), geneIndices.end(), rng);

    for (size_t geneIdx : geneIndices) {
        int originalValue = individual.genotype[geneIdx];
        double originalFitness = individual.fitness;

        int bestVal = originalValue;
        double bestFitness = originalFitness;

        for (int val = 0; val <= evaluator->getMaxGeneValue((int)geneIdx); ++val) {
            if (val == originalValue) continue;

            individual.genotype[geneIdx] = val;
            double newFitness = evaluator->evaluate(individual);
            if (newFitness > bestFitness) {
                bestFitness = newFitness;
                bestVal = val;
            }
        }

        // Set to the best found value
        individual.genotype[geneIdx] = bestVal;
        individual.fitness = bestFitness;
    }
    //Logger::debug("After FIHC: ");
    //individual.printDebugInfo();
}

void ZawodevGeneticAlgorithm::selection() {
    // sort population by fitness descending
    std::sort(population.begin(), population.end(), [](const Individual& a, const Individual& b) {
        return a.fitness > b.fitness;
    });

    // keep only the top crossSize individuals, initialize the rest randomly
    population.resize(crossSize);
    while ((int)population.size() < populationSize) {
        Individual ind;
        this->initRandom(ind);
        population.push_back(ind);
    }
}
