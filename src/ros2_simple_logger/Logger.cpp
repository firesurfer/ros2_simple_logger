#include "ros2_simple_logger/Logger.h"

simpleLogger* simpleLogger::instance = NULL;
bool simpleLogger::logger_destroyed = false;
std::mutex simpleLogger::globalLogger_mutex;

void simpleLogger::initLogger(rclcpp::node::Node::SharedPtr node)
{
    std::lock_guard<std::mutex> lock(globalLogger_mutex);
    if(instance == NULL)
    {

        instance = new simpleLogger(node);
    }
}

simpleLogger *simpleLogger::getInstance()
{
    if(logger_destroyed)
        throw std::runtime_error("Instance was destroyed or not created - did you call initLogger");

    if(instance != NULL)
        return instance;
    else
        throw std::runtime_error("Instance was destroyed or not created - did you call initLogger");

}
simpleLogger::~simpleLogger()
{
    if(logFileWriter != NULL)
    {
        logFileWriter->flush();
        delete logFileWriter;
    }
    if(instance != NULL)
    delete instance;
    instance = NULL;
    logger_destroyed = true;

}
void simpleLogger::setLogLevel(LogLevel level)
{
    currentLogLevel = level;
}

void simpleLogger::setLogFilePath(std::__cxx11::string path)
{
    std::ofstream writer(path);
    if(writer.is_open())
    {
        if(logFilePath == "" && path != "")
            logFileWriter = new std::ofstream(path);
        else
        {
            delete logFileWriter;
            if(path != "")
                logFileWriter = new std::ofstream(path);
        }

        this->logFilePath = path;

    }
}

simpleLogger &simpleLogger::getStream(LogLevel level)
{
    if(logger_destroyed)
        throw std::runtime_error("Logger already destroyed");
    //Some thread safety - not perfect
    std::lock_guard<std::mutex> lock(globalLogger_mutex);

    emptyLog = false;
    if(level < currentLogLevel)
    {
        emptyLog = true;
        return *this;
    }
    //clear log_stream
    log_stream.str("");

    //Prepare ros message
    builtin_interfaces::msg::Time time;
    set_now(time);
    msg->level = level;
    msg->stamp = time;

    //Get time
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    //Generate loglevel output
    std::string levelStr = "";
    switch(level)
    {
    case Debug:
        levelStr = printInColor("Debug", ConsoleColor::FG_DEFAULT, ConsoleColor::BG_GREEN);
        break;
    case Info:
        levelStr = printInColor("Info ", ConsoleColor::FG_GREEN);
        break;
    case Important:
        levelStr = printInColor("Important ", ConsoleColor::FG_BLUE);
        break;
    case Warning:
        levelStr = printInColor("Warning ", ConsoleColor::FG_RED);
        break;
    case Error:
        levelStr = printInColor("Error ", ConsoleColor::FG_RED, ConsoleColor::BG_YELLOW);
        break;

    case Fatal:
        levelStr = printInColor("Fatal ", ConsoleColor::FG_RED, ConsoleColor::BG_WHITE);
        break;
    }
    //Write preset into log_stream
#if __GNUC__ >= 5
    log_stream<< std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X") << " : " << levelStr << " : ";
#else
    log_stream << ctime (&in_time_t) << " : " << levelStr << " : ";
#endif

    //Return this as stream
    return *this;
}

void simpleLogger::set_now(builtin_interfaces::msg::Time &time)
{

    std::chrono::nanoseconds now = std::chrono::high_resolution_clock::now().time_since_epoch();
    if (now <= std::chrono::nanoseconds(0)) {
        time.sec = time.nanosec = 0;
    } else {
        time.sec = static_cast<builtin_interfaces::msg::Time::_sec_type>(now.count() / 1000000000);
        time.nanosec = now.count() % 1000000000;
    }
}



simpleLogger::simpleLogger(rclcpp::node::Node::SharedPtr _node):std::ostream(this)
{
    node = _node;
    publisher = node->create_publisher<ros2_simple_logger::msg::LoggingMessage>("ros2_log", rmw_qos_profile_sensor_data);
    msg = std::make_shared<ros2_simple_logger::msg::LoggingMessage>();
}

int simpleLogger::overflow(int c)
{

    char ch = (char)c;
    if(ch == '\n')
    {
        if(emptyLog)
            return 0;
        std::cout << log_stream.str()  << std::endl;
        if(logFilePath != "")
            *logFileWriter << log_stream.str() << std::endl;
        log_stream.str("");
        msg->message = msg_stream.str();
        publisher->publish(msg);
        msg_stream.str("");
    }
    else
    {
        if(emptyLog)
            return 0;
        log_stream << ch;
        msg_stream << ch;
    }
    return 0;
}
