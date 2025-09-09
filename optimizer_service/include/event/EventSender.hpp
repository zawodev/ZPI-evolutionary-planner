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

class RabbitMQEventSender : public EventSender {
public:
    explicit RabbitMQEventSender(const std::string& connectionString, const std::string& queueName = "optimizer_progress");
    ~RabbitMQEventSender();
    
    void sendProgress(const RawProgressData& progress) override;
    
private:
    std::string connectionString_;
    std::string queueName_;
    void* connection_;  // AMQP connection (forward declaration to avoid header dependency)
    void* channel_; // AMQP channel
    
    void connect();
    void disconnect();
    void sendMessage(const RawProgressData& message);
};
