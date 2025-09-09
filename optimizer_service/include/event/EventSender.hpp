#pragma once
#include "model/Individual.hpp"
#include "model/EventModels.hpp"
#include <memory>
#include <string>
#include <nlohmann/json.hpp>

class EventSender {
public:
    virtual ~EventSender() = default;
    virtual void sendProgress(const RawProgressData& progress) = 0;
};

class FileEventSender : public EventSender {
public:
    explicit FileEventSender(const std::string& directoryPath);
    void sendProgress(const RawProgressData& progress) override;
    
private:
    std::string directoryPath_;
    void writeToFile(const RawProgressData& message, const std::string& filename);
};

class RedisEventSender : public EventSender {
public:
    explicit RedisEventSender(const std::string& connectionString,
                             const std::string& progressKeyPrefix = "optimizer:progress:",
                             const std::string& progressChannel = "optimizer:progress:updates");
    ~RedisEventSender();
    
    void sendProgress(const RawProgressData& progress) override;
    
private:
    std::string connectionString_;
    std::string progressKeyPrefix_;
    std::string progressChannel_;
    void* redisConnection_;
    std::string host_;
    std::string port_;
    
    void connect();
    void disconnect();
    void parseConnectionString();
};