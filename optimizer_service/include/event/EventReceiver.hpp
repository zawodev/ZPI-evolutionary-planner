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

class RedisEventReceiver : public EventReceiver {
public:
    explicit RedisEventReceiver(const std::string& connectionString, 
                                const std::string& jobQueue = "optimizer:jobs",
                                const std::string& cancelKeyPrefix = "optimizer:cancel:");
    ~RedisEventReceiver();
    
    RawJobData receive() override;
    bool checkForCancellation() override;
    std::string getCurrentJobId() const override;
    bool hasMoreJobs() override;
    
private:
    std::string connectionString_;
    std::string jobQueue_;
    std::string cancelKeyPrefix_;
    void* redisConnection_;
    std::atomic<bool> cancelRequested_;
    std::string currentJobId_;
    std::string host_;
    std::string port_;
    
    void connect();
    void disconnect();
    bool checkCancelFlag();
    void parseConnectionString();
};
