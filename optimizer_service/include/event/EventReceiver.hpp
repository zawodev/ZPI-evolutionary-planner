#pragma once
#include "model/ProblemData.hpp"
#include "model/EventModels.hpp"
#include <memory>
#include <string>
#include <atomic>
#include <vector>
#include <nlohmann/json.hpp>

class EventReceiver {
public:
    virtual ~EventReceiver() = default;
    virtual RawJobData receive() = 0;
    virtual bool checkForCancellation() = 0;
    virtual std::string getCurrentJobId() const = 0;
    virtual bool hasMoreJobs() = 0;
};

class FileEventReceiver : public EventReceiver {
public:
    explicit FileEventReceiver(const std::string& directoryPath);
    RawJobData receive() override;
    bool checkForCancellation() override;
    std::string getCurrentJobId() const override;
    bool hasMoreJobs() override;
    
private:
    std::string directoryPath_;
    std::string currentJobId_;
    std::vector<std::string> jobFiles_;
    size_t currentJobIndex_;
    
    void loadJobFiles();
};

class RabbitMQEventReceiver : public EventReceiver {
public:
    explicit RabbitMQEventReceiver(const std::string& connectionString, 
                                   const std::string& jobQueue = "optimizer_jobs",
                                   const std::string& controlQueue = "optimizer_control");
    ~RabbitMQEventReceiver();
    
    RawJobData receive() override;
    bool checkForCancellation() override;
    std::string getCurrentJobId() const override;
    bool hasMoreJobs() override;
    
private:
    std::string connectionString_;
    std::string jobQueue_;
    std::string controlQueue_;
    void* connection_;  // AMQP connection (forward declaration)
    void* channel_; // AMQP channel
    std::atomic<bool> cancelRequested_;
    std::string currentJobId_;
    
    void connect();
    void disconnect();
    void setupControlMessageListener();
    void handleControlMessage(const RawControlData& message);
};
