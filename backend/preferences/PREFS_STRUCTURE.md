DEFAULT_USER_PREFERENCES = {
    "WidthHeightInfo": 0, # weight, positive means prefer wider, negative means prefer taller
    "GapsInfo": [0, 0, 0], # minGaps, maxGaps, weight
    "PreferredTimeslots": [0, 0, 0, 0, 0, 0, 0], # for each timeslot in cycle, weight
    "PreferredGroups": [0, 0, 0, 0, 0] # for each group, weight
}

DEFAULT_HOST_PREFERENCES = {
    "WidthHeightInfo": 0, # weight, positive means prefer wider, negative means prefer taller
    "GapsInfo": [0, 0, 0], # minGaps, maxGaps, weight
    "PreferredTimeslots": [0, 0, 0, 0, 0, 0, 0], # for each timeslot in cycle, weight
}

DEFAULT_CONSTRAINTS = {
    "TimeslotsDaily": 0, # 4 x hours (15min timeslots)
    "DaysInCycle": 0, # 7, 14 or 28
    "MinStudentsPerGroup": 0, # for each group, student count requirement (or group no start)
    "GroupsPerSubject": [0, 0, 0], # for each subject, number of groups
    "GroupsSoftCapacity": [0, 0, 0, 0, 0, 0], # for each group, soft capacity
    "StudentsSubjects": [
        [0, 0, 0], # subjectIds list for student 0
        [0, 0] # subjectIds list for student 1
    ],
    "TeachersGroups": [
        [0, 0, 0, 0], # groupIds list for teacher 0
        [0, 0, 0, 0, 0, 0] # groupIds list for teacher 1
    ],
    "RoomsUnavailabilityTimeslots": [
        [], # roomId 0, list of timeslot ids
        [12], # roomId 1, list of timeslot ids
    ],
    "StudentsUnavailabilityTimeslots": [
        [], # studentId 0, list of timeslot ids
        [5, 6, 7], # studentId 1, list of timeslot ids
    ],
    "TeachersUnavailabilityTimeslots": [
        [], # teacherId 0, list of timeslot ids
        [1, 2, 3], # teacherId 1, list of timeslot ids
    ],
    "RoomsCapacity": [
        0, # roomId 0, capacity
        0  # roomId 1, capacity
    ],
}

DEFAULT_MANAGEMENT_PREFERENCES = {
    "RoomMaxOverflow": [
        [0, 0, 0], # roomId, maxOverflow, weight
        [0, 0, 0]
    ],
}