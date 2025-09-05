#include "event/EventReceiver.hpp"
#include "optimization/Evaluator.hpp"
#include "optimization/IGeneticAlgorithm.hpp"
#include "optimization/ExampleGeneticAlgorithm.hpp"
#include "optimization/ZawodevGeneticAlgorithm.hpp"
#include "utils/JsonParser.hpp"
#include "utils/Logger.hpp"
#include "utils/TestCaseGenerator.hpp"
#include <iostream>
#include <memory>

void testGenerateAndSave() {
    TestCaseGenerator generator;
    int numStudents = 100;
    int numGroups = 9;
    int numSubjects = 3;
    int numRooms = 1;
    int numTeachers = 1;
    int numTimeslots = 25;
    int totalGroupCapacity = 260;
    RawProblemData testData = generator.generate(numStudents, numGroups, numSubjects, numRooms, numTeachers, numTimeslots, totalGroupCapacity);
    JsonParser::writeInput("data/generated_input.json", testData);
    Logger::info("Generated test data saved to data/generated_input.json");
}

int main() {
    try {
        Logger::info("Starting Optimizer Service...");
        //testGenerateAndSave();
        //return 0;

        // Data from input.json
        FileEventReceiver receiver("data/input.json"); // path to input file
        RawProblemData rawFileData = receiver.receive();

        // Randomly generated data for testing
        // TestCaseGenerator generator;
        // RawProblemData rawGenData = generator.generate(100, 50, 10, 20, 10, 25, 2000); // example values

        ProblemData data(rawFileData);
        if (!data.isFeasible()) {
            Logger::error("Problem is not solvable. Exiting.");
            return 1;
        }

        std::string debugMsg = "Received ProblemData with " + std::to_string(data.getStudentsNum()) + " students, " +
                               std::to_string(data.getGroupsNum()) + " groups, " +
                               std::to_string(data.getSubjectsNum()) + " subjects, " +
                               std::to_string(data.getRoomsNum()) + " rooms, " +
                               std::to_string(data.getTeachersNum()) + " teachers, and " +
                               std::to_string(data.totalTimeslots()) + " total timeslots.";
        Logger::info(debugMsg);

        Evaluator evaluator(data);
        std::unique_ptr<IGeneticAlgorithm> geneticAlgorithm = std::make_unique<ExampleGeneticAlgorithm>();
        Logger::info("Using genetic algorithm: " + std::string(typeid(*geneticAlgorithm).name()));
        
        geneticAlgorithm->Init(data, evaluator);
        Logger::info("Genetic algorithm initialization complete. Starting iterations...");

        Individual bestIndividual;
        for (int i = 0; i < 10; ++i) {
            bestIndividual = geneticAlgorithm->RunIteration(i);
            Logger::info("Iteration " + std::to_string(i) + ", best fitness: " + std::to_string(bestIndividual.fitness));
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
