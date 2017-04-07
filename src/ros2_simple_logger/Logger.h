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


/*
 * Easy access to the simpleLogger Singleton: Usage: LOG(<LogLevel>)<< "message" << "other message" << std:endl;
 */
#define LOG(level) simpleLogger::getInstance()->getStream(level)
#define INIT_LOGGER simpleLogger::getInstance()->initLogger
#define LOGLEVEL(level) simpleLogger::getInstance()->setLogLevel(level)

class simpleLogger : public std::ostream, std::streambuf
{
public:
    /**
     * @brief getInstance
     * @return Instance of the logger singleton
     */
    static simpleLogger* getInstance();
    /**
     * @brief set_now
     * @param time
     * Fills the builtin ros2 time messagge with the current time
     */
    static void set_now(builtin_interfaces::msg::Time & time);
    /**
     * @brief setLogLevel
     * @param level
     * Sets the maximum loglevel that will be printed
     */
    void setLogLevel(LogLevel level);
    /**
     * @brief setLogFilePath
     * @param path
     * Set a path for a logfile -> This will enable logging to the given file
     */
    void setLogFilePath(std::string path);
    /**
     * @brief initLogger
     * @param _node
     * Inits the logger with the ros2 node. Needs to be called before any log messages can be send via ros2
     */
    void initLogger(rclcpp::node::Node::SharedPtr _node);
    /**
     * @brief getStream
     * @param level
     * @return The stream that is used for logging
     * See LOG(level) above for usage
     */
    simpleLogger& getStream(LogLevel level = LogLevel::Info);
    /**
     * @brief ~simpleLogger
     * Clean up of file descriptors
     */
    virtual ~simpleLogger();

private:
    /**
     * @brief publisher used for communication via ros2
     */
    rclcpp::publisher::Publisher<ros2_simple_logger::msg::LoggingMessage>::SharedPtr publisher;
    /**
     * @brief singleton instance
     */
    static simpleLogger* instance;

    static std::mutex globalLogger_mutex;
    /**
     * @brief private constructor for simpleLogger
     */
    simpleLogger();
    /**
     * Internal streams
     */
    std::stringstream log_stream;
    std::stringstream msg_stream;
    std::string logFilePath = "";
    std::ofstream logFileWriter;
    LogLevel currentLogLevel = LogLevel::Info;
    /**
     * @brief emptyLog - used if the current message is beneath the set log level
     */
    bool emptyLog = false;

   

    builtin_interfaces::msg::Time time;
    LogLevel msg_log_level = LogLevel::Info;
    int overflow(int c);

};

