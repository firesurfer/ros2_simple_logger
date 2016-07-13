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



#define LOG(level) simpleLogger::getInstance().getStream(level)
#define INIT_LOGGER simpleLogger::getInstance().initLogger
#define LOGLEVEL(level) simpleLogger::getInstance().setLogLevel(level)

class simpleLogger : public std::ostream, std::streambuf
{
public:

    static simpleLogger& getInstance();
    static void set_now(builtin_interfaces::msg::Time & time);

    void setLogLevel(LogLevel level);
    void setLogFilePath(std::string path);
    void initLogger(rclcpp::node::Node::SharedPtr _node);
    simpleLogger& getStream(LogLevel level = LogLevel::Info);

    virtual ~simpleLogger();

private:

    rclcpp::publisher::Publisher<ros2_simple_logger::msg::LoggingMessage>::SharedPtr publisher;
    static simpleLogger instance;

    static std::mutex globalLogger_mutex;

    //rclcpp::node::Node::SharedPtr node;
    //ros2_simple_logger::msg::LoggingMessage::SharedPtr msg;

    std::stringstream log_stream;
    std::stringstream msg_stream;
    std::string logFilePath = "";
    std::ofstream* logFileWriter = NULL;
    LogLevel currentLogLevel = LogLevel::Info;

    bool emptyLog = false;
    simpleLogger();

    builtin_interfaces::msg::Time time;
    LogLevel msg_log_level = LogLevel::Info;
    int overflow(int c);

};

