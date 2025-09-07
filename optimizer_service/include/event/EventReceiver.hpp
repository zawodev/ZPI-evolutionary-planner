#pragma once
#include "model/ProblemData.hpp"
#include <memory>
#include <string>

class EventReceiver {
public:
    virtual ~EventReceiver() = default;
    virtual RawProblemData receive() = 0;
};

class FileEventReceiver : public EventReceiver {
public:
    explicit FileEventReceiver(const std::string& filename);
    RawProblemData receive() override;
private:
    std::string filename_;
};
