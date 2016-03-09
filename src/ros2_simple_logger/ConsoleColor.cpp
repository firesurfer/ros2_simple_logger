#include "ros2_simple_logger/ConsoleColor.h"


std::string printInColor(const std::string &str, ConsoleColor color)
{
    std::string col = "\033[" + std::to_string(color) + "m";
    return col + str + "\033[" + std::to_string(BG_DEFAULT) + "m";

}
