#include "event/EventSender.hpp"
#include "utils/JsonParser.hpp"
#include "utils/Logger.hpp"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <filesystem>
#include <sstream>

using json = nlohmann::json;



// -------------------------- File Event Sender -------------------------- 



FileEventSender::FileEventSender(const std::string& directoryPath)
    : directoryPath_(directoryPath) {
    
    // create directory if not exists
    if (!std::filesystem::exists(directoryPath_)) {
        std::filesystem::create_directories(directoryPath_);
    }
    
    Logger::info("FileEventSender initialized with directory: " + directoryPath_);
}

void FileEventSender::sendProgress(const RawProgressData& progress) {
    Logger::info("Sending progress to file for job " + progress.job_id + 
                ", iteration " + std::to_string(progress.iteration));
    
    std::string filename = directoryPath_ + "/" + progress.job_id + "_iter_" + std::to_string(progress.iteration) + ".json";
    writeToFile(progress, filename);
}

void FileEventSender::writeToFile(const RawProgressData& message, const std::string& filename) {
    try {
        std::ofstream file(filename);
        
        if (!file) {
            throw std::runtime_error("Cannot open file for writing: " + filename);
        }
        
        json output = JsonParser::toJson(message);
        file << output.dump(2) << std::endl;
        file.close();
        
        Logger::info("Progress written to file: " + filename);
        
    } catch (const std::exception& e) {
        Logger::error("Failed to write to file: " + std::string(e.what()));
        throw;
    }
}


// ------------------------ RabbitMQ Event Sender ------------------------


RabbitMQEventSender::RabbitMQEventSender(const std::string& connectionString, const std::string& queueName)
    : connectionString_(connectionString), queueName_(queueName), connection_(nullptr), channel_(nullptr) {
    Logger::info("RabbitMQEventSender initialized with queue: " + queueName_);
    connect();
}

RabbitMQEventSender::~RabbitMQEventSender() {
    disconnect();
}

void RabbitMQEventSender::sendProgress(const RawProgressData& progress) {
    Logger::info("Sending progress to RabbitMQ for job " + progress.job_id + 
                ", iteration " + std::to_string(progress.iteration));
    sendMessage(progress);
}

void RabbitMQEventSender::connect() {
    Logger::info("Connecting to RabbitMQ for sending: " + connectionString_);
    
    // TODO: Implement RabbitMQ connection when rabbitmq-c library is available
    // This is a placeholder implementation
    
    /*
    // Future implementation with rabbitmq-c:
    connection_ = amqp_new_connection();
    amqp_socket_t *socket = amqp_tcp_socket_new(static_cast<amqp_connection_state_t>(connection_));
    
    if (!socket) {
        throw std::runtime_error("Failed to create TCP socket for RabbitMQ");
    }
    
    // Parse connection string and connect
    // Format: amqp://username:password@host:port/vhost
    int status = amqp_socket_open(socket, host.c_str(), port);
    if (status) {
        throw std::runtime_error("Failed to open TCP socket to RabbitMQ");
    }
    
    amqp_rpc_reply_t r = amqp_login(static_cast<amqp_connection_state_t>(connection_),
                                   vhost.c_str(), 0, 131072, 0, AMQP_SASL_METHOD_PLAIN,
                                   username.c_str(), password.c_str());
    if (r.reply_type != AMQP_RESPONSE_NORMAL) {
        throw std::runtime_error("Failed to login to RabbitMQ");
    }
    
    amqp_channel_open(static_cast<amqp_connection_state_t>(connection_), 1);
    r = amqp_get_rpc_reply(static_cast<amqp_connection_state_t>(connection_));
    if (r.reply_type != AMQP_RESPONSE_NORMAL) {
        throw std::runtime_error("Failed to open RabbitMQ channel");
    }
    */
    
    Logger::warn("RabbitMQ sender connection not implemented yet - using placeholder");
}

void RabbitMQEventSender::disconnect() {
    if (connection_) {
        Logger::info("Disconnecting from RabbitMQ sender");
        
        // TODO: Implement proper disconnection
        /*
        amqp_channel_close(static_cast<amqp_connection_state_t>(connection_), 1, AMQP_REPLY_SUCCESS);
        amqp_connection_close(static_cast<amqp_connection_state_t>(connection_), AMQP_REPLY_SUCCESS);
        amqp_destroy_connection(static_cast<amqp_connection_state_t>(connection_));
        */
        
        connection_ = nullptr;
        channel_ = nullptr;
    }
}

void RabbitMQEventSender::sendMessage(const RawProgressData& message) {
    try {
        if (!connection_) {
            throw std::runtime_error("RabbitMQ connection not established");
        }
        
        json messageJson = JsonParser::toJson(message);
        std::string messageBody = messageJson.dump();
        
        Logger::info("Preparing to send message to queue: " + queueName_);
        Logger::debug("Message body: " + messageBody);
        
        // TODO: Implement actual message sending when rabbitmq-c is available
        /*
        amqp_basic_properties_t props;
        props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG;
        props.content_type = amqp_cstring_bytes("application/json");
        props.delivery_mode = 2; // persistent
        
        amqp_bytes_t message_bytes;
        message_bytes.len = messageBody.length();
        message_bytes.bytes = const_cast<char*>(messageBody.c_str());
        
        int result = amqp_basic_publish(
            static_cast<amqp_connection_state_t>(connection_),
            1,
            amqp_cstring_bytes(""),  // exchange
            amqp_cstring_bytes(queueName_.c_str()),  // routing key
            0,  // mandatory
            0,  // immediate
            &props,
            message_bytes
        );
        
        if (result < 0) {
            throw std::runtime_error("Failed to publish message to RabbitMQ");
        }
        */
        
        Logger::warn("RabbitMQ message sending not implemented yet - message would be: " + messageBody);
        
    } catch (const std::exception& e) {
        Logger::error("Failed to send message to RabbitMQ: " + std::string(e.what()));
        throw;
    }
}
