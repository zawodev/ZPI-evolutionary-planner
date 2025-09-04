#pragma once
#include "model/ProblemData.hpp"

class TestCaseGenerator {
public:
    // Future: Generate random test cases based on parameters
    // Parameters: numStudents, numGroups, numSubjects, numRooms, numTimeslots
    // For now, this class is empty and will be implemented later.
    RawProblemData generate(int numStudents, int numGroups, int numSubjects, int numRooms, int numTimeslots) {
        // TODO: Implement random generation logic
        return RawProblemData{}; // Placeholder
    }
};
