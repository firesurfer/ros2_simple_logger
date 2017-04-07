#include "ros2_simple_logger/Logger.h"

simpleLogger* simpleLogger::instance = NULL;

std::mutex simpleLogger::globalLogger_mutex;

void simpleLogger::initLogger(rclcpp::node::Node::SharedPtr _node)
{
    std::lock_guard<std::mutex> lock(globalLogger_mutex);
    this->publisher = _node->create_publisher<ros2_simple_logger::msg::LoggingMessage>("ros2_log", rmw_qos_profile_sensor_data);
}

simpleLogger *simpleLogger::getInstance()
{
    if(instance == NULL)
        instance = new simpleLogger();
    return instance;
}

simpleLogger::~simpleLogger()
{
    logFileWriter.close();
}

void simpleLogger::setLogLevel(LogLevel level)
{
    currentLogLevel = level;
}

void simpleLogger::setLogFilePath(std::__cxx11::string path)
{
    if(path != "" && logFileWriter.is_open())
        logFileWriter.close();

    logFileWriter.open(path);
    logFilePath = path;
}

simpleLogger &simpleLogger::getStream(LogLevel level)
{

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
   // builtin_interfaces::msg::Time time;
    set_now(time);
    this->msg_log_level = level;
    //msg->level = level;
    //msg->stamp = time;

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



simpleLogger::simpleLogger():std::ostream(this)
{

}

int simpleLogger::overflow(int c)
{
    char ch = (char)c;
    if(ch == '\n')
    {
        if(emptyLog)
            return 0;
        std::cout << log_stream.str()  << std::endl;
        if(logFileWriter.is_open())
            logFileWriter << log_stream.str() << std::endl;
        log_stream.str("");
        auto msg = std::make_shared<ros2_simple_logger::msg::LoggingMessage>();
        msg->stamp = time;
        msg->level = msg_log_level;
        msg->message = msg_stream.str();
        if(publisher != NULL)
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
