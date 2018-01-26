#include "SimpleLoggerSubscriber.h"





simpleLoggerSubscriber::simpleLoggerSubscriber(rclcpp::Node::SharedPtr _node)
{
    node = _node;
    using namespace std::placeholders;
    subscription = node->create_subscription<ros2_simple_logger_msgs::msg::LoggingMessage>("ros2_log", std::bind(&simpleLoggerSubscriber::internalListenerCallback, this,_1), rmw_qos_profile_default);
}
