#include "ros2_simple_logger/Logger.h"

std::shared_ptr<simpleLogger> simpleLogger::instance;



void simpleLogger::initLogger(rclcpp::node::Node::SharedPtr node)
{

    if(!instance)
    {
        simpleLogger * dummyPtr = new simpleLogger(node);
        instance = std::shared_ptr<simpleLogger>(dummyPtr);
    }
}

std::shared_ptr<simpleLogger> simpleLogger::getInstance()
{

    if(instance != NULL)
        return instance;
    else
        throw std::runtime_error("Instance was destroyed or not created - did you call initLogger");

}
