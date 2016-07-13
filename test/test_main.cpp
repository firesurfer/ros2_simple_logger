#include "ros2_simple_logger/Logger.h"
#include "ros2_simple_logger/SimpleLoggerSubscriber.h"

rclcpp::node::Node::SharedPtr node;
bool abortSpin = false;

void spin()
{
    rclcpp::executors::SingleThreadedExecutor exec;
    std::cout << "spinning" << std::endl;
    rclcpp::WallRate loop_rate(20);
    while(rclcpp::ok() && !abortSpin)
    {
        exec.spin_node_once(node,std::chrono::milliseconds(10));
        loop_rate.sleep();

    }
    std::cout << "stopped spinning node" << std::endl;

}
int main(int argc, char **argv)
{
    std::cout << "GCC Version:" << __GNUC__ << std::endl;
    rclcpp::init(argc,argv);
	node = std::make_shared<rclcpp::node::Node>("DemoLogger");
    std::thread * spinner = new std::thread(&spin);
    INIT_LOGGER(node);
    simpleLoggerSubscriber::SharedPtr test = std::make_shared<simpleLoggerSubscriber>(node);

    LOGLEVEL(LogLevel::Debug);
    LOG(LogLevel::Debug) << " debug stream test" << std::endl;
    LOG(LogLevel::Info) << " info stream test" << std::endl;
    LOG(LogLevel::Important) << " important stream test" << std::endl;
    LOG(LogLevel::Warning) << " warning stream test" << std::endl;
    LOG(LogLevel::Error) << "exception stream test" << std::endl;
    LOG(LogLevel::Fatal) << "fatal stream test" << std::endl;


    LOGLEVEL(LogLevel::Warning);
    LOG(LogLevel::Info) << "this shouldnt show up" << std::endl;
    LOG(LogLevel::Warning) << "this should show up" << std::endl;
    LOGLEVEL(LogLevel::Debug);

    std::string input = "";
    while(input != "exit")
    {

        std::getline (std::cin,input);
        LOG(LogLevel::Info) << input << std::endl;
    }
    std::cout << "Going to exit now" << std::endl;
    abortSpin = true;
    spinner->join();
}
