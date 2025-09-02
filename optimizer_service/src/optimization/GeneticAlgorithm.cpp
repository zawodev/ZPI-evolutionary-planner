#include "optimization/GeneticAlgorithm.hpp"
#include <random>

Genotype SimpleGeneticAlgorithm::run(const ProblemData& data, const Evaluator& evaluator, int maxGenerations) {
    Genotype best;
    std::mt19937 rng{std::random_device{}()};

    // part 1 student_group_assignments
    size_t total_assignments = 0;
    best.student_group_assignments_by_student.clear();
    for (const auto& subjects : data.students_subjects) {
        std::vector<int> student_assignments;
        for (size_t i = 0; i < subjects.size(); ++i) {
            int group = rng() % 5; // przyk³adowy zakres
            best.student_group_assignments.push_back(group);
            student_assignments.push_back(group);
            ++total_assignments;
        }
        best.student_group_assignments_by_student.push_back(student_assignments);
    }

    // parr2 group_timeslot_room_assignments
    size_t num_groups = 0;
    for (int g : data.groups_per_subject) num_groups += g;
    best.group_timeslot_room_assignments_by_group.clear();
    for (size_t i = 0; i < num_groups; ++i) {
        int timeslot = rng() % 35; // przyk³adowo 35 godzin w tygodniu
        int room = rng() % 3; // przyk³adowo 3 sale
        best.group_timeslot_room_assignments.push_back(timeslot);
        best.group_timeslot_room_assignments.push_back(room);
        best.group_timeslot_room_assignments_by_group.push_back({timeslot, room});
    }

    best.fitness = evaluator.evaluate(best, data);
    return best;
}
