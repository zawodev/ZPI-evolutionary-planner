#include "event/EventReceiver.hpp"
#include "optimization/Evaluator.hpp"
#include "optimization/GeneticAlgorithm.hpp"
#include "utils/JsonParser.hpp"
#include <iostream>

int main() {
    try {
        std::cout << "Starting optimization...\n";
		// read input from file (later from event receiver like RabbitMQ broker)
        FileEventReceiver receiver("data/input.json");
        ProblemData data = receiver.receive();

        // use simple evaluator and genetic algorithm for now (later interface for any algorithm of choice)
        SimpleEvaluator evaluator;
        SimpleGeneticAlgorithm ga;
        Genotype result = ga.run(data, evaluator, 10);

		// write output to file (later send it back via event sender like RabbitMQ broker)
        JsonParser::writeOutput("data/output.json", result);

        std::cout << "Optimization complete. Output written to data/output.json\n";
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
