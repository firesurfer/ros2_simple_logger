#include "ros2_simple_logger/Logger.h"
#include "ros2_simple_logger/SimpleLoggerSubscriber.h"
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
int main(int argc, char **argv)
{
    std::cout << "GCC Version:" << __GNUC__ << std::endl;
    rclcpp::init(argc,argv);
	node = std::make_shared<rclcpp::node::Node>("DemoLogger");
	 std::thread * t = new std::thread(&spin);
	simpleLogger::initLogger(node);
	simpleLoggerSubscriber* test = new simpleLoggerSubscriber(node);
	std::string input = "";
    LOG_DEBUG("Test");
    LOG_INFO("Test");
    LOG_IMPORTANT("Test");
    LOG_WARNING("Test");
    LOG_EXCEPTION("Test");

    LOG(LogLevel::Debug) << "stream test" << std::endl;
    LOG(LogLevel::Info) << "stream test" << std::endl;

    LOG(LogLevel::Important) << "stream test" << std::endl;
    LOG(LogLevel::Warning) << "stream test" << std::endl;

    LOG(LogLevel::Fatal) << "stream test" << std::endl;
	while(input != "exit")
	{
		
        std::getline (std::cin,input);
        LOG_INFO(input)
	}
}
