#include "model/Individual.hpp"

void Individual::printDebugInfo(const std::string& name) const {
    std::string output = "[" + (name.empty() ? "Individual" : name) + "]\n";
    output += "Genotype size: " + std::to_string(genotype.size()) + "\n";
    output += "Genotype: [";
    for (size_t i = 0; i < genotype.size(); ++i) {
        output += std::to_string(genotype[i]);
        if (i < genotype.size() - 1) output += ", ";
    }
    output += "]\n";
    output += "Fitness: " + std::to_string(fitness);
    Logger::debug(output);
}
