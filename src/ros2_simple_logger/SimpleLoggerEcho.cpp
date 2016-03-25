#include "ros2_simple_logger/Logger.h"

rclcpp::node::Node::SharedPtr node;

int main(int argc, char **argv)
{
    rclcpp::init(argc,argv);
    node = std::make_shared<rclcpp::node::Node>("simpleLoggerEcho");

    simpleLogger::initLogger(node);
    simpleLoggerSubscriber* test = new simpleLoggerSubscriber(node);
    rclcpp::WallRate loop_rate(20);
    while(true)
    {
        rclcpp::spin_some(node);
        loop_rate.sleep();
    }

}
