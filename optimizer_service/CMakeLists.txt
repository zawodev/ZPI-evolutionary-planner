# CMakeList.txt : CMake project for optimizer_service, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.10)

project(optimizer_service LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# nlohmann_json (header-only)
include(FetchContent)
FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

include_directories(include)

# Explicitly list sources to ensure all are included
set(SOURCES
    src/main.cpp
    src/event/EventReceiver.cpp
    src/model/Individual.cpp
    src/model/ProblemData.cpp
    src/optimization/Evaluator.cpp
    src/optimization/ExampleGeneticAlgorithm.cpp
    src/optimization/ZawodevGeneticAlgorithm.cpp
    src/utils/JsonParser.cpp
    src/utils/Logger.cpp
    src/utils/TestCaseGenerator.cpp
)

add_executable(optimizer_service ${SOURCES})

target_include_directories(optimizer_service PRIVATE include)

target_link_libraries(optimizer_service PRIVATE nlohmann_json::nlohmann_json)

