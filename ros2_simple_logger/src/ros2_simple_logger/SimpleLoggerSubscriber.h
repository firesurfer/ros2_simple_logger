#pragma once


#include "Logger.h"

class simpleLoggerSubscriber
{
public:
    typedef std::shared_ptr<simpleLoggerSubscriber> SharedPtr;

    simpleLoggerSubscriber(rclcpp::Node::SharedPtr _node, bool request_history= false, std::string node_name_for_history = "");
    void setLoggingCallback(std::function<void(ros2_simple_logger_msgs::msg::LoggingMessage::SharedPtr)> func)
    {
        loggingCallback = func;
    }
    void setLogToConsole(bool state = true)
    {
        disableLogToConsole = !state;
    }
    void setFilter(std::string nodeName)
    {
        this->filterNodeName = nodeName;
    }

    static time_t get_time_from_msg(ros2_simple_logger_msgs::msg::LoggingMessage::SharedPtr msg)
    {
        int64_t unixtime = msg->stamp.sec + msg->stamp.nanosec/1000000000;

        //auto chrono =std::chrono::duration_cast<std::chrono::time_point>(unixtime);

        time_t time = unixtime;

        // std::chrono::milliseconds(unixtime);

        return time;
    }
    static std::string get_time_as_string(time_t time)
    {
        std::stringstream ss;

#if __GNUC__ >= 5

        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %X");
#else
        ss << ctime (&time);
#endif
        return ss.str();
    }
private:
    void internalListenerCallback(ros2_simple_logger_msgs::msg::LoggingMessage::SharedPtr msg)
    {
        if(filterNodeName != "")
        {
            if(msg->nodename != filterNodeName)
            {
                return;
            }
        }

        if(loggingCallback)
            loggingCallback(msg);

        if(disableLogToConsole)
            return;

        std::string levelStr = "";
        int level = msg->level;
        switch(level)
        {
        case Debug:
            levelStr = printInColor("Debug", ConsoleColor::FG_BLUE);
            break;
        case Info:
            levelStr = printInColor("Info", ConsoleColor::FG_GREEN);
            break;
        case Important:
            levelStr = printInColor("Important", ConsoleColor::FG_WHITE, ConsoleColor::BG_GREEN);
            break;
        case Warning:
            levelStr = printInColor("Warning", ConsoleColor::FG_RED);
            break;
        case Error:
            levelStr = printInColor("Error", ConsoleColor::FG_RED, ConsoleColor::BG_YELLOW);
            break;
        case Fatal:
            levelStr = printInColor("Fatal", ConsoleColor::FG_RED, ConsoleColor::BG_WHITE);
            break;
        }
        std::cout<< get_time_as_string(get_time_from_msg(msg)) << " : " << msg->nodename << " : " << levelStr << " : " << msg->message << std::endl;

    }

    bool disableLogToConsole = false;
    rclcpp::Node::SharedPtr node;
    std::function<void(ros2_simple_logger_msgs::msg::LoggingMessage::SharedPtr)> loggingCallback;
    rclcpp::Subscription<ros2_simple_logger_msgs::msg::LoggingMessage>::SharedPtr subscription;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr requestHistoryPub;
    std::string filterNodeName = "";

};



