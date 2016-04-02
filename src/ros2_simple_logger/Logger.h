#pragma once

#include <mutex>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <ostream>
#include <streambuf>
#include "ros2_simple_logger/msg/logging_message.hpp"
#include "rclcpp/rclcpp.hpp"
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
    static std::shared_ptr<simpleLogger> getInstance();

    void setLogLevel(LogLevel level)
    {
        currentLogLevel = level;
    }

    simpleLogger& getStream(LogLevel level = LogLevel::Info)
    {
        //Some thread safety - not perfect
        std::lock_guard<std::mutex> lock(globalLogger_mutex);

        emptyLog = false;
        if(level < currentLogLevel)
        {
            emptyLog = true;
            return *this;
        }
        //clear log_stream
        log_stream.str("");

        //Prepare ros message
        builtin_interfaces::msg::Time time;
        set_now(time);
        msg->level = level;
        msg->stamp = time;

        //Get time
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);

        //Generate loglevel output
        std::string levelStr = "";
        switch(level)
        {
        case Debug:
            levelStr = printInColor("Debug", ConsoleColor::FG_DEFAULT, ConsoleColor::BG_GREEN);
            break;
        case Info:
            levelStr = printInColor("Info ", ConsoleColor::FG_GREEN);
            break;
        case Important:
            levelStr = printInColor("Important ", ConsoleColor::FG_BLUE);
            break;
        case Warning:
            levelStr = printInColor("Warning ", ConsoleColor::FG_RED);
            break;
        case Error:
            levelStr = printInColor("Error ", ConsoleColor::FG_RED, ConsoleColor::BG_YELLOW);
            break;

        case Fatal:
            levelStr = printInColor("Fatal ", ConsoleColor::FG_RED, ConsoleColor::BG_WHITE);
            break;
        }
        //Write preset into log_stream
#if __GNUC__ >= 5
        log_stream<< std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X") << " : " << levelStr << " : ";
#endif
        //Return this as stream
        return *this;
    }


    void set_now(builtin_interfaces::msg::Time & time)
    {

        std::chrono::nanoseconds now = std::chrono::high_resolution_clock::now().time_since_epoch();
        if (now <= std::chrono::nanoseconds(0)) {
            time.sec = time.nanosec = 0;
        } else {
            time.sec = static_cast<builtin_interfaces::msg::Time::_sec_type>(now.count() / 1000000000);
            time.nanosec = now.count() % 1000000000;
        }
    }

private:

    rclcpp::publisher::Publisher<ros2_simple_logger::msg::LoggingMessage>::SharedPtr publisher;
    static std::shared_ptr<simpleLogger> instance;
    std::mutex globalLogger_mutex;

    rclcpp::node::Node::SharedPtr node;
    ros2_simple_logger::msg::LoggingMessage::SharedPtr msg;

    std::stringstream log_stream;
    std::stringstream msg_stream;

    LogLevel currentLogLevel = LogLevel::Info;

    bool emptyLog = false;
    simpleLogger(rclcpp::node::Node::SharedPtr _node):std::ostream(this)
    {
        node = _node;
        publisher = node->create_publisher<ros2_simple_logger::msg::LoggingMessage>("ros2_log", rmw_qos_profile_sensor_data);
        msg = std::make_shared<ros2_simple_logger::msg::LoggingMessage>();
    }

    int overflow(int c)
    {

        char ch = (char)c;
        if(ch == '\n')
        {
            if(emptyLog)
                return 0;
            std::cout << log_stream.str()  << std::endl;
            log_stream.str("");
            msg->message = msg_stream.str();
            publisher->publish(msg);
            msg_stream.str("");
        }
        else
        {
            if(emptyLog)
                return 0;
            log_stream << ch;
            msg_stream << ch;
        }
        return 0;
    }

};

