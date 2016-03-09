if(_ros2_simple_logger_CONFIG_INCLUDED)
  return()
endif()
set(_ros2_simple_logger_CONFIG_INCLUDED TRUE)

set(ROS2_SIMPLE_LOGGER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../ros2_simple_logger)
MESSAGE("ros2_components are are in: " ${ROS2_SIMPLE_LOGGER_DIR})

file(GLOB LIB_FILES 
	${ROS2_SIMPLE_LOGGER_DIR}/build/*.a
	${ROS2_SIMPLE_LOGGER_DIR}/build/*.so 

)

set(ros2_simple_logger_LIBRARIES ${LIB_FILES})

set(ros2_simple_logger_INCLUDE_DIRS  
	${ROS2_SIMPLE_LOGGER_DIR}/src
	${ROS2_SIMPLE_LOGGER_DIR}/build/rosidl_generator_cpp/

)
