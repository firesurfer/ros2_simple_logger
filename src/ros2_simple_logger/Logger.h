#pragma once

#include "ros2_simple_logger/msg/logging_message.hpp"
#include "rclcpp/rclcpp.hpp"
#include <mutex>
#include <chrono>  // chrono::system_clock
#include <ctime>   // localtime

#include <iomanip>
#include <ros2_simple_logger/ConsoleColor.h>
typedef enum
{
    Debug = 0,
    Info = 1,
    Important = 2,
    Warning = 3,
    Exception = 4,
    Error = 5,
    Fatal = 6

}LogLevel;


#define LOG_DEBUG(x) simpleLogger::getInstance()->writeEntry(x, LogLevel::Debug);
#define LOG_INFO(x) simpleLogger::getInstance()->writeEntry(x, LogLevel::Info);
#define LOG_IMPORTANT(x) simpleLogger::getInstance()->writeEntry(x, LogLevel::Important);
#define LOG_WARNING(x) simpleLogger::getInstance()->writeEntry(x, LogLevel::Warning);
#define LOG_EXCEPTION(x) simpleLogger::getInstance()->writeEntry(x, LogLevel::Exception);
#define LOG_ERROR(x) simpleLogger::getInstance()->writeEntry(x, LogLevel::Error);
#define LOG_FATAL(x) simpleLogger::getInstance()->writeEntry(x, LogLevel::Fatal);


class simpleLogger
{
public:
    static void initLogger(rclcpp::node::Node::SharedPtr node);

    static std::shared_ptr<simpleLogger> getInstance();
    void enableDebug(bool state)
    {
        debugOn = state;
    }

    void writeEntry(std::string message, LogLevel level = LogLevel::Info)
    {
        std::lock_guard<std::mutex> lock(globalLogger_mutex);
        builtin_interfaces::msg::Time time;
        set_now(time);

        msg->level = level;
        msg->message = message;
        msg->stamp = time;
        publisher->publish(msg);

        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);



        std::string levelStr = "";
        switch(level)
        {
        case Debug:
        {
           levelStr = "Debug ";
           if(!debugOn)
                return;
        }
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
        case Exception:
            levelStr = printInColor("Exception ", ConsoleColor::FG_RED);
            break;
        case Error:
            levelStr = printInColor("Error ", ConsoleColor::FG_RED);
            break;

        case Fatal:
            levelStr = printInColor("Fatal ", ConsoleColor::FG_RED);
            break;
        }

        std::cout<< std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X") << " : " << level << " : " << message << std::endl;
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
    simpleLogger(rclcpp::node::Node::SharedPtr _node)
    {
        node = _node;
        publisher = node->create_publisher<ros2_simple_logger::msg::LoggingMessage>("ros2_log", rmw_qos_profile_sensor_data);
        msg = std::make_shared<ros2_simple_logger::msg::LoggingMessage>();
    }
    static std::shared_ptr<simpleLogger> instance;
    std::mutex globalLogger_mutex;

    rclcpp::node::Node::SharedPtr node;
    ros2_simple_logger::msg::LoggingMessage::SharedPtr msg;
    bool debugOn = true;

};

class simpleLoggerSubscriber
{
public:
    simpleLoggerSubscriber(rclcpp::node::Node::SharedPtr _node)
    {
        node = _node;
        using namespace std::placeholders;
        subscription = node->create_subscription<ros2_simple_logger::msg::LoggingMessage>("ros2_log", std::bind(&simpleLoggerSubscriber::internalListenerCallback, this,_1), rmw_qos_profile_sensor_data);
    }
    void setLoggingCallback(std::function<void(ros2_simple_logger::msg::LoggingMessage::SharedPtr)> func)
    {
        loggingCallback = func;
    }

private:
    void internalListenerCallback(ros2_simple_logger::msg::LoggingMessage::SharedPtr msg)
    {
        std::cout << msg->message << std::endl;
        if(loggingCallback)
            loggingCallback(msg);
    }

    rclcpp::node::Node::SharedPtr node;
    rclcpp::subscription::Subscription<ros2_simple_logger::msg::LoggingMessage>::SharedPtr subscription;
    std::function<void(ros2_simple_logger::msg::LoggingMessage::SharedPtr)> loggingCallback;

};

