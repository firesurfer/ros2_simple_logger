#include "SimpleLoggerSubscriber.h"





simpleLoggerSubscriber::simpleLoggerSubscriber(rclcpp::node::Node::SharedPtr _node)
{
    node = _node;
    using namespace std::placeholders;
    subscription = node->create_subscription<ros2_simple_logger::msg::LoggingMessage>("ros2_log", std::bind(&simpleLoggerSubscriber::internalListenerCallback, this,_1), rmw_qos_profile_default);
}
