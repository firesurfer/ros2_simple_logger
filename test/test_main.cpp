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

    simpleLogger::getInstance()->setLogLevel(LogLevel::Debug);
    LOG(LogLevel::Debug) << " debug stream test" << std::endl;
    LOG(LogLevel::Info) << " info stream test" << std::endl;
    LOG(LogLevel::Important) << " important stream test" << std::endl;
    LOG(LogLevel::Warning) << " warning stream test" << std::endl;
    LOG(LogLevel::Error) << "exception stream test" << std::endl;
    LOG(LogLevel::Fatal) << "fatal stream test" << std::endl;

   // test->setLogToConsole(false);
    simpleLogger::getInstance()->setLogLevel(LogLevel::Warning);
    LOG(LogLevel::Info) << "this shouldnt show up" << std::endl;
    LOG(LogLevel::Warning) << "this should show up" << std::endl;

    while(input != "exit")
    {

        std::getline (std::cin,input);
        LOG(LogLevel::Info) << input << std::endl;
    }
}
