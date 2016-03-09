#include "ros2_simple_logger/ConsoleColor.h"


std::__cxx11::string printInColor(const std::__cxx11::string &str, ConsoleColor color)
{
    std::string col = "\033[" + std::to_string(color) + "m";
    return col + str + "\033[" + std::to_string(BG_DEFAULT) + "m";

}
