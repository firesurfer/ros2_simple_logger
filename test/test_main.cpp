#include "ros2_simple_logger/Logger.h"

rclcpp::node::Node::SharedPtr node;
void spin()
{
    std::cout << "spinning" << std::endl;
    rclcpp::WallRate loop_rate(20);
    while(true)
    {
        rclcpp::spin_some(node);
        loop_rate.sleep();
    }


}
int main(void)
{
	node = std::make_shared<rclcpp::node::Node>("DemoLogger");
	 std::thread * t = new std::thread(&spin);
	simpleLogger::initLogger(node);
	simpleLoggerSubscriber* test = new simpleLoggerSubscriber(node);
	std::string input = "";
	while(input != "exit")
	{
		
        std::getline (std::cin,input);
		LOG_DEBUG(input)
	}
}
