#pragma once
#include "model/ProblemData.hpp"
#include <memory>

class EventReceiver {
public:
    virtual ~EventReceiver() = default;
    virtual ProblemData receive() = 0;
};

class FileEventReceiver : public EventReceiver {
public:
    explicit FileEventReceiver(const std::string& filename);
    ProblemData receive() override;
private:
    std::string filename_;
};
