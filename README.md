# ros2_simple_logger

This is a simple logging package created for ROS 2 until an official logger is available.
It supports serveral log levels and logging message into a ros 2 topic.

The package also contains a tool for printing log messages that were published via the ros 2 topic to the commandline.

## Usage

The usage is fairly simple.
For information on how to add this package as a dependency to your package.xml and CMakeLists.txt please have a look at the [ros2_components](https://github.com/firesurfer/ros2_components) repository. 

The first thing you need to do is to initialize the logger with the used ros2 node:

```c++
    auto node = rclcpp::node::Node::make_shared("TestNode");
    INIT_LOGGER(node);
```
### Logging

In order to log something you can use a syntax like c++ streams
```c++
    LOG(LogLevel::Debug) << " debug stream test" << std::endl;
    LOG(LogLevel::Info) << " info stream test" << std::endl;
    LOG(LogLevel::Important) << " important stream test" << std::endl;
    LOG(LogLevel::Warning) << " warning stream test" << std::endl;
    LOG(LogLevel::Error) << "exception stream test" << std::endl;
    LOG(LogLevel::Fatal) << "fatal stream test" << std::endl;
```
The std::endl is important!
As you can see in the example there are 6 supported LogLevels, starting from Debug to Fatal.
The minimum logged level can be set via:

```c++
simpleLogger::getInstance()->setLogLevel(<LogLevel>);
```
The logger itself is implemented as a singleton and should provide a minimum of threadsafety (But I didn't test that).

### Recieving Logs via ROS 2

In order to recieve the logging messages via the ros 2 topic there is a class call `simpleLoggerSubscriber`.
It encapsulates the topic and message conversion.

Usage:
```c++
   auto node = rclcpp::node::Node::make_shared("TestNode2");
   simpleLoggerSubscriber* logSubscriber = new simpleLoggerSubscriber(node);
```

Adding a callback that is called when a new message arrives:

```c++
    auto func = [&] ( ros2_simple_logger::msg::LoggingMessage::SharedPtr msg)
    {
        
    };
    logSubscriber->setLoggingCallback(func);
```
