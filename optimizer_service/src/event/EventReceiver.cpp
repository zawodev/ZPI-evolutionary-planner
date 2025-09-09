#include "event/EventReceiver.hpp"
#include "utils/JsonParser.hpp"
#include "utils/Logger.hpp"
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>

using json = nlohmann::json;



// -------------------------- File Event Receiver --------------------------



FileEventReceiver::FileEventReceiver(const std::string& directoryPath)
    : directoryPath_(directoryPath), currentJobIndex_(0) {
    loadJobFiles();
}

void FileEventReceiver::loadJobFiles() {
    Logger::info("Loading job files from directory: " + directoryPath_);
    
    if (!std::filesystem::exists(directoryPath_)) {
        throw std::runtime_error("Directory does not exist: " + directoryPath_);
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(directoryPath_)) {
        if (entry.is_regular_file() && entry.path().extension() == ".json") {
            jobFiles_.push_back(entry.path().string());
        }
    }
    
    Logger::info("Found " + std::to_string(jobFiles_.size()) + " job files");
}

RawJobData FileEventReceiver::receive() {
    if (currentJobIndex_ >= jobFiles_.size()) {
        throw std::runtime_error("No more jobs available in file queue");
    }
    
    std::string filename = jobFiles_[currentJobIndex_];
    currentJobIndex_++;
    
    Logger::info("Processing job file: " + filename);
    
    std::ifstream in(filename);
    if (!in) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    
    json j;
    in >> j;
    
    RawJobData jobData = JsonParser::toRawJobData(j);
    
    currentJobId_ = jobData.job_id;
    return jobData;
}

bool FileEventReceiver::checkForCancellation() {
    // for file-based input, no cancellation mechanism (never will be)
    return false;
}

std::string FileEventReceiver::getCurrentJobId() const {
    return currentJobId_;
}

bool FileEventReceiver::hasMoreJobs() {
    return currentJobIndex_ < jobFiles_.size();
}



// ------------------------ RabbitMQ Event Receiver ------------------------




RabbitMQEventReceiver::RabbitMQEventReceiver(const std::string& connectionString,
                                           const std::string& jobQueue,
                                           const std::string& controlQueue)
    : connectionString_(connectionString), jobQueue_(jobQueue), controlQueue_(controlQueue),
      connection_(nullptr), channel_(nullptr), cancelRequested_(false) {
    connect();
}

RabbitMQEventReceiver::~RabbitMQEventReceiver() {
    disconnect();
}

RawJobData RabbitMQEventReceiver::receive() {
    Logger::info("Starting to receive job data from RabbitMQ: " + connectionString_);
    
    if (!connection_) {
        throw std::runtime_error("RabbitMQ connection not established");
    }
    
    // TODO: Implement RabbitMQ job receiving when rabbitmq-c library is available
    /*
    // Future implementation will:
    // 1. Set up consumer on jobQueue_
    // 2. Wait for message
    // 3. Parse JSON message
    // 4. Extract job_id, problem_data, max_execution_time
    // 5. Return JobData
    
    amqp_rpc_reply_t res;
    amqp_envelope_t envelope;
    
    // Set up consumer
    amqp_basic_consume(static_cast<amqp_connection_state_t>(connection_), 1,
                      amqp_cstring_bytes(jobQueue_.c_str()),
                      amqp_empty_bytes, 0, 1, 0, amqp_empty_table);
    res = amqp_get_rpc_reply(static_cast<amqp_connection_state_t>(connection_));
    if (res.reply_type != AMQP_RESPONSE_NORMAL) {
        throw std::runtime_error("Failed to set up consumer");
    }
    
    // Wait for message
    amqp_maybe_release_buffers(static_cast<amqp_connection_state_t>(connection_));
    res = amqp_consume_message(static_cast<amqp_connection_state_t>(connection_), &envelope, NULL, 0);
    
    if (res.reply_type != AMQP_RESPONSE_NORMAL) {
        throw std::runtime_error("Failed to consume message");
    }
    
    // Parse message
    std::string messageBody((char*)envelope.message.body.bytes, envelope.message.body.len);
    json messageJson = json::parse(messageBody);
    
    std::string job_id = messageJson["job_id"];
    json problem_data = messageJson["problem_data"];
    int max_execution_time = messageJson.value("max_execution_time", 300);
    
    currentJobId_ = job_id;
    cancelRequested_ = false;
    
    RawProblemData rawData = JsonParser::toRawProblemData(problem_data);
    
    amqp_destroy_envelope(&envelope);
    
    return RawJobData(job_id, rawData, max_execution_time);
    */
    
    // Placeholder implementation
    throw std::runtime_error("RabbitMQ job receiver not implemented yet");
}

bool RabbitMQEventReceiver::checkForCancellation() {
    if (cancelRequested_.load()) {
        Logger::info("Cancellation requested for job: " + currentJobId_);
        return true;
    }
    
    // TODO: Check for control messages non-blockingly
    /*
    // Future implementation will check control queue for cancel messages
    amqp_envelope_t envelope;
    amqp_rpc_reply_t res;
    
    // Try to consume message with timeout (non-blocking)
    struct timeval timeout = {0, 100000}; // 100ms timeout
    res = amqp_consume_message(static_cast<amqp_connection_state_t>(connection_), &envelope, &timeout, 0);
    
    if (res.reply_type == AMQP_RESPONSE_NORMAL) {
        std::string messageBody((char*)envelope.message.body.bytes, envelope.message.body.len);
        json controlJson = json::parse(messageBody);
        
        RawControlData controlMsg = JsonParser::toRawControlData(controlJson);
        
        handleControlMessage(controlMsg);
        
        amqp_destroy_envelope(&envelope);
    }
    */
    
    return cancelRequested_.load();
}

std::string RabbitMQEventReceiver::getCurrentJobId() const {
    return currentJobId_;
}

bool RabbitMQEventReceiver::hasMoreJobs() {
    // For RabbitMQ, we always assume there might be more jobs
    // This will cause the service to keep running and waiting
    return true;
}

void RabbitMQEventReceiver::connect() {
    Logger::info("Connecting to RabbitMQ for receiving: " + connectionString_);
    
    // TODO: Implement RabbitMQ connection when rabbitmq-c library is available
    /*
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
    
    // Declare queues
    amqp_queue_declare(static_cast<amqp_connection_state_t>(connection_), 1,
                      amqp_cstring_bytes(jobQueue_.c_str()),
                      0, 1, 0, 0, amqp_empty_table);
    amqp_queue_declare(static_cast<amqp_connection_state_t>(connection_), 1,
                      amqp_cstring_bytes(controlQueue_.c_str()),
                      0, 1, 0, 0, amqp_empty_table);
    */
    
    Logger::warn("RabbitMQ receiver connection not implemented yet - using placeholder");
}

void RabbitMQEventReceiver::disconnect() {
    if (connection_) {
        Logger::info("Disconnecting from RabbitMQ receiver");
        
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

void RabbitMQEventReceiver::setupControlMessageListener() {
    // TODO: Set up background listener for control messages
    Logger::info("Setting up control message listener for queue: " + controlQueue_);
}

void RabbitMQEventReceiver::handleControlMessage(const RawControlData& message) {
    Logger::info("Received control message: " + message.action + " for job: " + message.job_id);
    
    if (message.action == "cancel" && message.job_id == currentJobId_) {
        Logger::warn("Cancel requested for current job: " + currentJobId_);
        cancelRequested_.store(true);
    }
}
