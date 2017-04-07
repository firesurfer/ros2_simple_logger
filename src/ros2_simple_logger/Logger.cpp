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
    messageLogLevel = level;
    printLogLevel = level;
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
    if(level != currentLogLevel)
        log_stream<<'\n';
    currentLogLevel = level;
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
    if(c == EOF)
        return c;
    if(c == '\n')
    {
        //Some thread safety - not perfect
        std::lock_guard<std::mutex> lock(globalLogger_mutex);

        if(log_stream.str() == "")
            return c;

        //Prepare ros message
        //builtin_interfaces::msg::Time time;
        set_now(time);
        //msg->level = level;
        //msg->stamp = time;

        //Get time
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);

        //Generate loglevel output
        std::string levelStr = "";
        switch(currentLogLevel)
        {
        case Debug:
            levelStr = printInColor("Debug  : ", ConsoleColor::FG_DEFAULT, ConsoleColor::BG_GREEN);
            break;
        case Info:
            levelStr = printInColor("Info   : ", ConsoleColor::FG_GREEN);
            break;
        case Important:
            levelStr = printInColor("Important: ", ConsoleColor::FG_BLUE);
            break;
        case Warning:
            levelStr = printInColor("Warning: ", ConsoleColor::FG_RED);
            break;
        case Error:
            levelStr = printInColor("Error  : ", ConsoleColor::FG_RED, ConsoleColor::BG_YELLOW);
            break;
        case Fatal:
            levelStr = printInColor("Fatal  : ", ConsoleColor::FG_RED, ConsoleColor::BG_WHITE);
            break;
        }
        std::ostringstream timeStrStream;
        //Write preset into log_stream
    #if __GNUC__ >= 5
        timeStrStream << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X") << " : ";
    #else
        timeStrStream << ctime (&in_time_t) << " : " ;
    #endif

        if(printLogLevel <= currentLogLevel)
            std::cout << timeStrStream.str() << levelStr << log_stream.str()  << std::endl;
        if(fileLogLevel <= currentLogLevel && logFileWriter.is_open())
            logFileWriter << timeStrStream.str() << levelStr << log_stream.str() << '\n';
        if(messageLogLevel <= currentLogLevel && publisher != NULL){
            auto msg = std::make_shared<ros2_simple_logger::msg::LoggingMessage>();
            msg->stamp = time;
            msg->level = messageLogLevel;
            msg->message = log_stream.str();
            publisher->publish(msg);
        }
        log_stream.str("");
    }
    else
    {
        log_stream << (char)c;
    }
    return c;
}
