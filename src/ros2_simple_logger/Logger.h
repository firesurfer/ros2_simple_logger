#pragma once

#include <mutex>
#include <chrono>
#include <ctime>
#include <stdio.h>
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

/**
 * @brief The simpleLogger class is a very simple implementation of a streambased logger.
 * It can either be used with a rosnode or without. In case the @ref initLogger function was called, all log messages will be published to a ros logger topic.
 *
 * Usage:
 * Use the defined macros:
 *
 * @ref INIT_LOGGER - call at the beginning with a valid rosnode as an argument.
 * @ref LOG(level) - used for the real logging: Example: LOG(Debug) << "Test << std::endl.
 * @ref LOGLEVEL(level) - Sets the maximum loglevel that should be printed. Standard is Info.
 *
 * For the rest of the functions use the getInstance function of the singleton:
 *
 * simpleLogger::getInstance()->....
 */
class simpleLogger : public std::ostream, std::streambuf
{
public:
    /**
     * @brief getInstance
     * @return Instance of the logger singleton
     */
    static simpleLogger* getInstance();
    /**
     * @brief set_now -  Fills the builtin ros2 time messagge with the current time.
     * @param time - Ros2 Time message.
     */
    static void setNow(builtin_interfaces::msg::Time & time);
    /**
     * @brief setLogLevel
     * @param level
     * Sets the maximum loglevel that will be printed
     */
    void setLogLevel(LogLevel level);
    /**
     * @brief setLogFilePath - Set a path for a logfile -> This will enable logging to the given file.
     * @param path
     * Also it will move in case of an existing logfile the exisiting logfile to path.1.
     * An existing logfile backup will be deleted.
     */
    void setLogFilePath(std::string path);
    /**
     * @brief initLogger -Inits the logger with the ros2 node.
     * @param _node
     * Needs to be called before any log messages can be send via ros2.
     */
    void initLogger(rclcpp::Node::SharedPtr _node);
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

    /**
     * @brief setLogfileSizeLimit
     * @param limit in bytes
     */
    void setLogfileSizeLimit(size_t limit);

    LogLevel fromString(std::string str)
    {
        if(str == "Debug")
            return LogLevel::Debug;
        else if(str == "Info")
            return LogLevel::Info;
        else if(str == "Important")
            return LogLevel::Important;
        else if(str == "Warning")
            return LogLevel::Warning;
        else if(str == "Error")
            return LogLevel::Error;
        else if(str == "Fatal")
            return LogLevel::Error;
        return LogLevel::Info;

    }

private:
    /**
     * @brief private constructor for simpleLogger
     */
    simpleLogger();

    /**
     * @brief publisher used for communication via ros2
     */
    rclcpp::Publisher<ros2_simple_logger::msg::LoggingMessage>::SharedPtr publisher;
    /**
     * @brief singleton instance
     */
    static simpleLogger* instance;
    /**
     * @brief globalLogger_mutex - Used for implementing some kind of threadsafety. (Not perfect)
     */
    static std::mutex globalLogger_mutex;

    /**
     * Internal streams
     */
    std::stringstream log_stream;
    std::stringstream msg_stream;
    std::string logFilePath = "";
    std::ofstream logFileWriter;
    /**
     * Default loglevels.
     * TODO implement getter and setters
     */
    LogLevel printLogLevel = LogLevel::Info;
    LogLevel fileLogLevel = LogLevel::Debug;
    LogLevel messageLogLevel = LogLevel::Info;


    /**
     * @brief logfileSizeLimit - defaults to 1MB
     */
    size_t logfileSizeLimit = 1048576;

   

    builtin_interfaces::msg::Time time;
    /**
     * @brief currentLogLevel - The current set loglevel
     */
    LogLevel currentLogLevel = LogLevel::Info;
    /**
     * @brief overflow
     * @param c
     * @return
     * TODO write documentation
     */
    int overflow(int c);

    /**
     * @brief check_if_file_exists  - Helperfunction for determining if a given file exists.
     * @param filename
     * @return true in case file exists.
     * //TODO Move into extra file
     */
    bool checkIfFileExists(const std::string filename);

    bool checkFileSizeExceedsLimit(const std::string filename);
    void checkLogfile();
    std::string node_name;
};

