#pragma once
#include <vector>

struct Genotype {
	// surowe dane sp³aszczone do dwóch wektorów (mo¿e do jednego lepiej? idk)
    std::vector<int> student_group_assignments;
    std::vector<int> group_timeslot_room_assignments;
    double fitness = 0.0;

    // sformatowane dane (do czytelnego outputu)
    std::vector<std::vector<int>> student_group_assignments_by_student; // [student][przedmiot]
    std::vector<std::vector<int>> group_timeslot_room_assignments_by_group; // [group][2] (timeslot, room)
};
