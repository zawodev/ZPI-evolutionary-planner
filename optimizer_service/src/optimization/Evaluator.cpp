#include "optimization/Evaluator.hpp"
#include "utils/Logger.hpp"
#include <random>

Evaluator::Evaluator(const ProblemData& data) : problemData(data) {
    buildMaxValues();
}

void Evaluator::buildMaxValues() {
    maxValues.clear();
    int studentsNum = problemData.getStudentsNum();
    int groupsNum = problemData.getGroupsNum();
    int roomsNum = problemData.getRoomsNum();
    int timeslotsNum = problemData.totalTimeslots();

    // Student part: for each subject of each student, max = groups_per_subject[p] - 1
    for (int s = 0; s < studentsNum; ++s) {
        for (int p : problemData.students_subjects[s]) {
            int max_val = problemData.groups_per_subject[p] - 1;
            maxValues.push_back(max_val);
        }
    }

    // Group part
    for (int g = 0; g < groupsNum; ++g) {
        maxValues.push_back(timeslotsNum - 1); // timeslot
        maxValues.push_back(roomsNum - 1); // room
    }

    //print maxValues for debugging (with logger info)
    Logger::info("Evaluator initialized. Max gene values:");
    for (size_t i = 0; i < maxValues.size(); ++i) {
        Logger::info(std::to_string(maxValues[i]));
    }
    Logger::info("Total genes: " + std::to_string(maxValues.size()));
}

double Evaluator::evaluate(const Individual& genotype) const {
    // Mock: random fitness for now we will change it later (maybe)
    static std::mt19937 rng{std::random_device{}()};
    static std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(rng);
}

int Evaluator::getMaxGeneValue(int geneIdx) const {
    return maxValues[geneIdx];
}

std::pair<bool, Individual> Evaluator::repair(const Individual& individual) const {
    Individual repaired = individual;
    bool wasRepaired = false;

    // Calculate subject student counts
    std::vector<int> subject_student_counts(problemData.getSubjectsNum(), 0);
    int idx = 0;
    for (int s = 0; s < problemData.getStudentsNum(); ++s) {
        for (int p : problemData.students_subjects[s]) {
            int rel_gen = repaired.genotype[idx++];
            subject_student_counts[p]++;
        }
    }

    // Check and repair capacity violations per subject
    const auto& subject_total_capacity = problemData.getSubjectTotalCapacity();
    for (int p = 0; p < problemData.getSubjectsNum(); ++p) {
        if (subject_student_counts[p] > subject_total_capacity[p]) {
            // Simple repair: reduce to capacity
            subject_student_counts[p] = subject_total_capacity[p];
            wasRepaired = true;
            // Note: Actual genotype modification would require tracking which genes to change (sounds complex to me lol)
        }
    }

    // For now, return repaired as is (since actual repair is pretty complex at least for current state of this framework)
    return {wasRepaired, repaired};
}
