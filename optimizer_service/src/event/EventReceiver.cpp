#include "event/EventReceiver.hpp"
#include "utils/JsonParser.hpp"
#include "utils/Logger.hpp"

FileEventReceiver::FileEventReceiver(const std::string& filename)
    : filename_(filename) {}

ProblemData FileEventReceiver::receive() {
    Logger::info("Starting to read input file: " + filename_);
    return JsonParser::parseInput(filename_);
}
