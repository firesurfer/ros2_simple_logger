#include "SimpleLoggerSubscriber.h"





simpleLoggerSubscriber::simpleLoggerSubscriber(rclcpp::Node::SharedPtr _node, bool request_history, std::string node_name_for_history)
{
    node = _node;
    using namespace std::placeholders;
    if(request_history)
    {
        this->requestHistoryPub = node->create_publisher<std_msgs::msg::String>("ros2_request_log_history", rmw_qos_profile_default);
        auto msg = std::make_shared<std_msgs::msg::String>();
        msg->data = node_name_for_history;
        this->requestHistoryPub->publish(msg);

    }
    subscription = node->create_subscription<ros2_simple_logger_msgs::msg::LoggingMessage>("ros2_log", std::bind(&simpleLoggerSubscriber::internalListenerCallback, this,_1), rmw_qos_profile_default);

}
