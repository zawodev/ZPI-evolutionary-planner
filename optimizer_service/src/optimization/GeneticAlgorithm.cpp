#include "optimization/GeneticAlgorithm.hpp"
#include <random>

void SimpleGeneticAlgorithm::Init(const ProblemData& data, const Evaluator& evaluator) {
    this->problemData = &data;
    this->evaluator = &evaluator;
    bestGenotype = Individual{};
    initialized = true;
    // Inicjalizacja populacji/genotypu
    std::mt19937 rng{std::random_device{}()};
    size_t total_assignments = 0;
    for (const auto& subjects : data.students_subjects) {
        total_assignments += subjects.size();
    }
    for (size_t i = 0; i < total_assignments; ++i) {
        bestGenotype.genotype.push_back(rng() % 5);
    }
    size_t num_groups = 0;
    for (int g : data.groups_per_subject) num_groups += g;
    for (size_t i = 0; i < num_groups; ++i) {
        int timeslot = rng() % 35;
        int room = rng() % 3;
        bestGenotype.genotype.push_back(timeslot);
        bestGenotype.genotype.push_back(room);
    }

    bestGenotype.fitness = evaluator.evaluate(bestGenotype);
}

Individual SimpleGeneticAlgorithm::RunIteration(int currentIteration) {
    if (!initialized || !problemData || !evaluator) {
        throw std::runtime_error("SimpleGeneticAlgorithm not initialized");
    }

    auto [isValid, repaired] = evaluator->repair(bestGenotype);
    if (!isValid) {
        // użyj repaired jako poprawionego genotypu
    }

    // Przykładowa logika: fitness losowy, można dodać mutacje/cross cokolwiek takiego genetycznego później
    bestGenotype.fitness = evaluator->evaluate(bestGenotype);
    return bestGenotype;
}
