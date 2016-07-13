#pragma once

#include <mutex>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <ostream>
#include <streambuf>
#include <fstream>
#include "rclcpp/rclcpp.hpp"
#include "ros2_simple_logger/msg/logging_message.hpp"
#include <ros2_simple_logger/ConsoleColor.h>

typedef enum
{
    Debug = 0,
    Info = 1,
    Important = 2,
    Warning = 3,
    Error = 4,
    Fatal = 5

}LogLevel;


#define LOG(level) simpleLogger::getInstance()->getStream(level)
#define INIT_LOGGER simpleLogger::initLogger

class simpleLogger : public std::ostream, std::streambuf
{
public:
    static void initLogger(rclcpp::node::Node::SharedPtr node);
    static simpleLogger* getInstance();

    void setLogLevel(LogLevel level);
    void setLogFilePath(std::string path);

    simpleLogger& getStream(LogLevel level = LogLevel::Info);


    static void set_now(builtin_interfaces::msg::Time & time);
    virtual ~simpleLogger();


private:

    rclcpp::publisher::Publisher<ros2_simple_logger::msg::LoggingMessage>::SharedPtr publisher;
    static simpleLogger* instance;
    static bool logger_destroyed;
    static std::mutex globalLogger_mutex;

    rclcpp::node::Node::SharedPtr node;
    ros2_simple_logger::msg::LoggingMessage::SharedPtr msg;

    std::stringstream log_stream;
    std::stringstream msg_stream;
    std::string logFilePath = "";
    std::ofstream* logFileWriter = NULL;
    LogLevel currentLogLevel = LogLevel::Info;

    bool emptyLog = false;
    simpleLogger(rclcpp::node::Node::SharedPtr _node);


    int overflow(int c);

};

