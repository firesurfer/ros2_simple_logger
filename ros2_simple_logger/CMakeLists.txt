cmake_minimum_required(VERSION 3.5)


project(ros2_simple_logger)


set (CMAKE_CXX_STANDARD 14)
if(NOT WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra -fPIC")
endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package (Threads REQUIRED)
find_package(ament_cmake REQUIRED)
find_package(builtin_interfaces REQUIRED)
find_package(rclcpp REQUIRED)
find_package(ros2_simple_logger_msgs REQUIRED)


include_directories(
    ${rclcpp_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/
)
file(GLOB SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/ros2_simple_logger/*.c*)
add_library(ros2_simple_logger_lib ${SRC_FILES} )

ament_target_dependencies(ros2_simple_logger_lib
    rclcpp
    ros2_simple_logger_msgs
)

target_link_libraries(ros2_simple_logger_lib
    Threads::Threads
)



add_executable(ros2_simple_logger_test ${CMAKE_CURRENT_SOURCE_DIR}/test/test_main.cpp)

ament_target_dependencies(ros2_simple_logger_test
    rclcpp
    ros2_simple_logger_msgs
)
target_link_libraries(ros2_simple_logger_test
    ros2_simple_logger_lib
    Threads::Threads
)



add_executable(ros2_simple_logger_echo ${CMAKE_CURRENT_SOURCE_DIR}/src/ros2_simple_logger/SimpleLoggerEcho.cpp)
ament_target_dependencies(ros2_simple_logger_echo
    rclcpp
    ros2_simple_logger_msgs
)

target_link_libraries(ros2_simple_logger_echo
    ros2_simple_logger_lib
    Threads::Threads
)

install(TARGETS ros2_simple_logger_lib DESTINATION lib)

install(TARGETS ros2_simple_logger_test DESTINATION tests/ros2_simple_logger)
install(TARGETS ros2_simple_logger_echo DESTINATION bin)








file(GLOB INCLUDE_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ros2_simple_logger/*.h*"
    )

install(FILES ${INCLUDE_FILES} DESTINATION include/ros2_simple_logger) 


file(GLOB BINARY_FILES ${CMAKE_BINARY_DIR}/*.so)
install(FILES ${BINARY_FILES} DESTINATION lib)

ament_export_include_directories(include)

ament_export_libraries(ros2_simple_logger_lib)

ament_export_dependencies(ament_cmake builtin_interfaces rclcpp rmw rmw_implementation_cmake ros2_simple_logger_msgs)
ament_package()
