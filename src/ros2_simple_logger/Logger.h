#pragma once

#include "ros2_simple_logger/msg/logging_message.hpp"
#include "rclcpp/rclcpp.hpp"
#include <mutex>
#include <chrono>  // chrono::system_clock
#include <ctime>   // localtime
#include <sstream>
#include <iomanip>
#include <ros2_simple_logger/ConsoleColor.h>
#include <ostream>
#include <streambuf>
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




#define LOG(level) simpleLogger::getInstance()->getStream(level)

class simpleLogger : public std::ostream, std::streambuf
{
public:
    static void initLogger(rclcpp::node::Node::SharedPtr node);

    static std::shared_ptr<simpleLogger> getInstance();


    void enableDebug(bool state)
    {
        debugOn = state;
    }
    simpleLogger& getStream(LogLevel level = LogLevel::Info)
    {
        log_stream.str("");
        builtin_interfaces::msg::Time time;
        set_now(time);

        msg->level = level;

        msg->stamp = time;

        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
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
#if __GNUC__ >= 5

        log_stream<< std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X") << " : " << levelStr << " : ";
#endif
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
    simpleLogger(rclcpp::node::Node::SharedPtr _node):std::ostream(this)
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
    std::stringstream log_stream;
    std::stringstream msg_stream;

    int overflow(int c)
    {
        char ch = (char)c;
        if(ch == '\n')
        {

            std::cout << log_stream.str()  << std::endl;
            log_stream.str("");
            msg->message = msg_stream.str();
            publisher->publish(msg);
            msg_stream.str("");
        }
        else
        {
            log_stream << ch;
            msg_stream << ch;
        }
        return 0;
    }

};

