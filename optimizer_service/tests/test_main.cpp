#include "optimization/GeneticAlgorithm.hpp"
#include "optimization/Evaluator.hpp"
#include "model/ProblemData.hpp"
#include "model/Genotype.hpp"
#include <cassert>
#include <iostream>

// stare nieaktualne, najlepiej wyrzuciæ

void test_simple_algorithm() {
    ProblemData data;
    data.users = { {1, 1.0, { {"08:00-10:00"}, {"A"}, {"101"} } } };
    data.rooms = {"101"};
    data.groups = {"A"};
    data.timeslots = {"08:00-10:00"};
    SimpleEvaluator evaluator;
    SimpleGeneticAlgorithm ga;
    Genotype g = ga.run(data, evaluator, 1);
    assert(!g.assignments.empty());
    std::cout << "Test passed!\n";
}

int main() {
    test_simple_algorithm();
    return 0;
}
