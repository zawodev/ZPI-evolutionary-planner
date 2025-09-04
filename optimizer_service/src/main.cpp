#include "event/EventReceiver.hpp"
#include "optimization/Evaluator.hpp"
#include "optimization/GeneticAlgorithm.hpp"
#include "utils/JsonParser.hpp"
#include "utils/Logger.hpp"
#include <iostream>
#include <memory>

int main() {
    try {
        Logger::info("Starting Optimizer Service...");
        // Example usage of TestCaseGenerator (for future random test case generation):
        // #include "utils/TestCaseGenerator.hpp"
        // TestCaseGenerator generator;
        // ProblemData testData = generator.generate(100, 50, 20, 10, 5); // numStudents, numGroups, numSubjects, numRooms, numTimeslots
        // Then use testData instead of receiver.receive()

        FileEventReceiver receiver("data/input.json");
        ProblemData data = receiver.receive();
        std::string debugMsg = "Received ProblemData with " + std::to_string(data.getStudentsNum()) + " students, " +
                               std::to_string(data.getGroupsNum()) + " groups, " +
                               std::to_string(data.getSubjectsNum()) + " subjects, " +
                               std::to_string(data.getRoomsNum()) + " rooms, and " +
                               std::to_string(data.totalTimeslots()) + " total timeslots.";
        Logger::info(debugMsg);

        Evaluator evaluator(data);
        std::unique_ptr<IGeneticAlgorithm> geneticAlgorithm = std::make_unique<SimpleGeneticAlgorithm>();
        Individual bestIndividual;

        geneticAlgorithm->Init(data, evaluator);
        Logger::info("Genetic algorithm initialization complete. Starting iterations...");
        
        for (int i = 0; i < 10; ++i) {
            bestIndividual = geneticAlgorithm->RunIteration(i);
            Logger::info("Iteration " + std::to_string(i) + ", fitness: " + std::to_string(bestIndividual.fitness));
        }

        JsonParser::writeOutput("data/output.json", bestIndividual, data, evaluator);
        Logger::info("Optimization complete. Output written to data/output.json");
    } 
    catch (const std::exception& e) {
        Logger::error(e.what());
        return 1;
    }
    return 0;
}
