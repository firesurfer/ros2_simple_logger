cmake_minimum_required(VERSION 3.5)


project(ros2_simple_logger)



if(NOT WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra -fPIC")
endif()

find_package(ament_cmake REQUIRED)
find_package(builtin_interfaces REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rmw REQUIRED)
find_package(rmw_implementation_cmake REQUIRED)
find_package(ament_lint_auto REQUIRED)
find_package(rosidl_default_generators REQUIRED)
set(message_files
    "msg/LoggingMessage.msg"
    )



rosidl_generate_interfaces(${PROJECT_NAME}
    ${message_files}
    DEPENDENCIES builtin_interfaces
    )


get_default_rmw_implementation(rmw_implementation)
#Remove this line in case you want to force another rmw implementation.
set(rmw_implementation "rmw_fastrtps_cpp")
get_rmw_typesupport(typesupport_impls "${rmw_implementation}" LANGUAGE "cpp")

MESSAGE("Default rmw_implementation is:" ${rmw_implementation})



include_directories(
   # ${CMAKE_BINARY_DIR}/rosidl_generator_cpp
    ${rclcpp_INCLUDE_DIRS}
    ${rmw_INCLUDE_DIRS}
    ${rmw_implementation_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/

    )
file(GLOB SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/ros2_simple_logger/*.c*)
add_library(ros2_simple_logger_lib ${SRC_FILES} )


add_dependencies(ros2_simple_logger_lib ${PROJECT_NAME})
target_compile_definitions(ros2_simple_logger_lib PUBLIC "RMW_IMPLEMENTATION=${rmw_implementation}")

foreach(typesupport_impl ${typesupport_impls})
    rosidl_target_interfaces(ros2_simple_logger_lib ${PROJECT_NAME} ${typesupport_impl})
endforeach()

target_link_libraries(ros2_simple_logger_lib
    ${rclcpp_LIBRARIES}
    ${rmw_LIBRARIES}
    ${rmw_implementation_LIBRARIES}
    ${std_msgs_LIBRARIES}
    ${builtin_interfaces_LIBRARIES}
    -pthread
 )



add_executable(ros2_simple_logger_test ${CMAKE_CURRENT_SOURCE_DIR}/test/test_main.cpp)
add_dependencies(ros2_simple_logger_test ${PROJECT_NAME})

target_compile_definitions(ros2_simple_logger_test PUBLIC "RMW_IMPLEMENTATION=${rmw_implementation}")

foreach(typesupport_impl ${typesupport_impls})
    rosidl_target_interfaces(ros2_simple_logger_test ${PROJECT_NAME} ${typesupport_impl})
endforeach()

target_link_libraries(ros2_simple_logger_test
    ros2_simple_logger_lib
    -pthread
    )



add_executable(ros2_simple_logger_echo ${CMAKE_CURRENT_SOURCE_DIR}/src/ros2_simple_logger/SimpleLoggerEcho.cpp)
add_dependencies(ros2_simple_logger_test ${PROJECT_NAME})

target_compile_definitions(ros2_simple_logger_echo PUBLIC "RMW_IMPLEMENTATION=${rmw_implementation}")

foreach(typesupport_impl ${typesupport_impls})
    rosidl_target_interfaces(ros2_simple_logger_echo ${PROJECT_NAME} ${typesupport_impl})
endforeach()


target_link_libraries(ros2_simple_logger_echo
    ros2_simple_logger_lib

    -pthread
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

ament_export_dependencies(ament_cmake builtin_interfaces rclcpp rmw rmw_implementation_cmake)
ament_package()
