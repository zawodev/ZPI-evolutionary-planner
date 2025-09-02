#include "event/EventReceiver.hpp"
#include "optimization/Evaluator.hpp"
#include "optimization/GeneticAlgorithm.hpp"
#include "utils/JsonParser.hpp"
#include "utils/Logger.hpp"
#include <iostream>
#include <memory>

int main() {
    try {
        Logger::info("Starting optimization...");
        FileEventReceiver receiver("data/input.json");
        ProblemData data = receiver.receive();

        Evaluator evaluator(data);
        std::unique_ptr<IGeneticAlgorithm> geneticAlgorithm = std::make_unique<SimpleGeneticAlgorithm>();
        Individual bestIndividual;

        geneticAlgorithm->Init(data, evaluator);
        for (int i = 0; i < 10; ++i) {
            bestIndividual = geneticAlgorithm->RunIteration(i);
            //Logger::info("Iteration " + std::to_string(i) + ", fitness: " + std::to_string(result.fitness));
        }

        JsonParser::writeOutput("data/output.json", bestIndividual, data);
        Logger::info("Optimization complete. Output written to data/output.json");
    } 
    catch (const std::exception& e) {
        Logger::error(e.what());
        return 1;
    }
    return 0;
}
