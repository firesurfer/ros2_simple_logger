cmake_minimum_required(VERSION 2.8.3)


project(ros2_simple_logger)



if(NOT WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -g")
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
    	  
)

macro(targets)
	if(NOT "${target_suffix} " STREQUAL " ")
	  get_rclcpp_information("${rmw_implementation}" "rclcpp${target_suffix}")
	endif()
		MESSAGE("BINARY DIR IS IN: " ${CMAKE_BINARY_DIR})
	
	include_directories(
		${CMAKE_BINARY_DIR}/rosidl_generator_cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/
		${CMAKE_CURRENT_SOURCE_DIR}/build/rosidl_generator_cpp	
			
	)
        file(GLOB SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/ros2_simple_logger/*.c*)
        add_library(ros2_simple_logger_lib${target_suffix} ${SRC_FILES})
	ament_target_dependencies(ros2_simple_logger_lib${target_suffix}
        	"rclcpp"
		
	)
	add_dependencies(ros2_simple_logger_lib${target_suffix} ${PROJECT_NAME})
	target_link_libraries(ros2_simple_logger_lib${target_suffix}
		-pthread
	)
	add_executable(ros2_simple_logger_test${target_suffix} ${CMAKE_CURRENT_SOURCE_DIR}/test/test_main.cpp)
        ament_target_dependencies(ros2_simple_logger_test${target_suffix} ${CMAKE_CURRENT_SOURCE_DIR}/test/test_main.cpp
        	"rclcpp"	
		 ros2_simple_logger_lib${target_suffix}
		 pthread
	)
        add_executable(ros2_simple_logger_echo${target_suffix} ${CMAKE_CURRENT_SOURCE_DIR}/src/ros2_simple_logger/SimpleLoggerEcho.cpp)
        ament_target_dependencies(ros2_simple_logger_echo${target_suffix} ${CMAKE_CURRENT_SOURCE_DIR}/src/ros2_simple_logger/SimpleLoggerEcho.cpp
                "rclcpp"
                 ros2_simple_logger_lib${target_suffix}
                 pthread
        )
      
        target_link_libraries(ros2_simple_logger_test${target_suffix}
		ros2_simple_logger_lib${target_suffix}
		${CMAKE_BINARY_DIR}/libros2_simple_logger__rosidl_generator_c.so
		${CMAKE_BINARY_DIR}/libros2_simple_logger__rosidl_typesupport_opensplice_cpp.so

            -pthread)
        target_link_libraries(ros2_simple_logger_echo${target_suffix}
                ros2_simple_logger_lib${target_suffix}
                ${CMAKE_BINARY_DIR}/libros2_simple_logger__rosidl_generator_c.so
                ${CMAKE_BINARY_DIR}/libros2_simple_logger__rosidl_typesupport_opensplice_cpp.so

            -pthread)
	

endmacro()



call_for_each_rmw_implementation(targets GENERATE_DEFAULT)



file(GLOB INCLUDE_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/ros2_simple_logger/*.h*"
)

install(FILES ${INCLUDE_FILES} DESTINATION include/ros2_simple_logger) 
install(TARGETS ros2_simple_logger_lib DESTINATION lib)

file(GLOB BINARY_FILES ${CMAKE_BINARY_DIR}/*.so)
install(FILES ${BINARY_FILES} DESTINATION lib)

file(GLOB LOG_MSG_INCLUDE_FILES  "${CMAKE_BINARY_DIR}/rosidl_generator_cpp/ros2_simple_logger/msg/*.h*")
#MESSAGE(${LOG_MSG_INCLUDE_FILES} )
#install(FILES ${LOG_MSG_INCLUDE_FILES} DESTINATION include/ros2_simple_logger/msg)


ament_export_include_directories(include)

ament_export_libraries(ros2_simple_logger_lib ${BINARY_FILES})

ament_package()
