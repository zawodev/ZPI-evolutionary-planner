#include "event/EventReceiver.hpp"
#include "utils/JsonParser.hpp"

FileEventReceiver::FileEventReceiver(const std::string& filename) : filename_(filename) {}

ProblemData FileEventReceiver::receive() {
    return JsonParser::parseInput(filename_);
}
