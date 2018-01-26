cmake_minimum_required(VERSION 3.5)

project(ros2_simple_logger_msgs)

if(NOT WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra -fPIC")
endif()

find_package(ament_cmake REQUIRED)

#Message generation
find_package(builtin_interfaces REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(ros2_components_msg REQUIRED)
file(GLOB MESSAGE_FILES 
    "msg/*.msg"
)

file(GLOB SERVICE_FILES 
    "srv/*.srv"
)

rosidl_generate_interfaces(${PROJECT_NAME}
  ${MESSAGE_FILES}
  ${SERVICE_FILES}
  DEPENDENCIES builtin_interfaces
)

ament_package()
