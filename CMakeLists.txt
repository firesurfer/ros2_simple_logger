cmake_minimum_required(VERSION 2.8.3)


project(ros2_simple_logger)

#set(WORKSPACE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
#include(findPackageInWorkspace.cmake)

if(NOT WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra")
endif()

find_package(ament_cmake REQUIRED)
find_package(builtin_interfaces REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rmw REQUIRED)
find_package(rmw_implementation_cmake REQUIRED)

set(message_files
	"msg/LoggingMessage.msg"
)

 rosidl_generate_interfaces(${PROJECT_NAME}

	 ${message_files}
	  DEPENDENCIES builtin_interfaces
    	SKIP_INSTALL
)

macro(targets)
	if(NOT "${target_suffix} " STREQUAL " ")
	  get_rclcpp_information("${rmw_implementation}" "rclcpp${target_suffix}")
	endif()
	
	include_directories(
		${CMAKE_CURRENT_SOURCE_DIR}/src
		${CMAKE_CURRENT_SOURCE_DIR}/build/rosidl_generator_cpp	
	)
        file(GLOB SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c*)
        add_library(ros2_simple_logger_lib${target_suffix} ${SRC_FILES})
	ament_target_dependencies(ros2_simple_logger_lib${target_suffix}
        	"rclcpp"
		
	)
	target_link_libraries(ros2_simple_logger_lib${target_suffix}
		-pthread
	)
	add_executable(ros2_simple_logger_test${target_suffix} ${CMAKE_CURRENT_SOURCE_DIR}/test/test_main.cpp)
   	ament_target_dependencies(ros2_simple_logger_test${target_suffix}
        	"rclcpp"	
		ros2_simple_logger_lib${target_suffix}
	)
        target_link_libraries(ros2_simple_logger_test${target_suffix}
		ros2_simple_logger_lib${target_suffix}
		${CMAKE_CURRENT_SOURCE_DIR}/build/libros2_simple_logger__rosidl_generator_c.so
		${CMAKE_CURRENT_SOURCE_DIR}/build/libros2_simple_logger__rosidl_typesupport_opensplice_cpp.so

            -pthread)
	#install(TARGETS ros2_parameter_server${target_suffix} DESTINATION bin)

endmacro()



call_for_each_rmw_implementation(targets GENERATE_DEFAULT)



file(GLOB INCLUDE_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.h*"
)
message(STATUS "FILES TO INSTALL (ROS2_SIMPLE_LOGGER)")
message(STATUS ${INCLUDE_FILES})
install(FILES ${INCLUDE_FILES} DESTINATION include/ros2_simple_logger) 
install(TARGETS ros2_simple_logger_lib DESTINATION lib)


ament_export_include_directories(include)
ament_export_libraries(ros2_simple_logger_lib)

ament_package()
