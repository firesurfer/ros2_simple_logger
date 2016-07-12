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
find_package(ament_lint_auto REQUIRED)
find_package(rosidl_default_generators REQUIRED)
set(message_files
	"msg/LoggingMessage.msg"
)



rosidl_generate_interfaces(${PROJECT_NAME}
	 ${message_files}
         DEPENDENCIES builtin_interfaces
    	  
)

get_available_rmw_implementations(middleware_implementations)
  foreach(middleware_impl ${middleware_implementations})
    find_package("${middleware_impl}" REQUIRED)
  endforeach()

macro(targets)
        if(NOT "${target_suffix} " STREQUAL " ")
          get_rclcpp_information("${rmw_implementation}" "rclcpp${target_suffix}")
        endif()

        if(NOT "${target_suffix}" STREQUAL "${rmw_default}")
            return()
        endif()
        MESSAGE("rclcpp${target_suffix}")
	
	include_directories(
		${CMAKE_BINARY_DIR}/rosidl_generator_cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/
			
	)
        file(GLOB SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/ros2_simple_logger/*.c*)
        add_library(ros2_simple_logger_lib${target_suffix} SHARED ${SRC_FILES} )
	ament_target_dependencies(ros2_simple_logger_lib${target_suffix}
                "rclcpp${target_suffix}"
		
	)


	add_dependencies(ros2_simple_logger_lib${target_suffix} ${PROJECT_NAME})
	target_link_libraries(ros2_simple_logger_lib${target_suffix}
		-pthread
                #${CMAKE_BINARY_DIR}/libros2_simple_logger__rosidl_typesupport_introspection_cpp.so
                #${CMAKE_BINARY_DIR}/libros2_simple_logger__rosidl_generator_c.so
                #${CMAKE_BINARY_DIR}/libros2_simple_logger__rosidl_typesupport_introspection_c.so

        )



	add_executable(ros2_simple_logger_test${target_suffix} ${CMAKE_CURRENT_SOURCE_DIR}/test/test_main.cpp)
        add_dependencies(ros2_simple_logger_test${target_suffix} ${PROJECT_NAME})
        get_rmw_typesupport(typesupport_impls "${rmw_implementation}" LANGUAGE "cpp")
        target_compile_definitions(ros2_simple_logger_test${target_suffix} PUBLIC "RMW_IMPLEMENTATION=${rmw_implementation}")

        foreach(typesupport_impl ${typesupport_impls})
            rosidl_target_interfaces(ros2_simple_logger_test${target_suffix} ${PROJECT_NAME} ${typesupport_impl})
        endforeach()

        ament_target_dependencies(ros2_simple_logger_test${target_suffix} ${CMAKE_CURRENT_SOURCE_DIR}/test/test_main.cpp
                "rclcpp${target_suffix}"
		 ros2_simple_logger_lib${target_suffix}
		 pthread
	)
        target_link_libraries(ros2_simple_logger_test${target_suffix}
                ros2_simple_logger_lib${target_suffix}
                #${CMAKE_BINARY_DIR}/libros2_simple_logger__rosidl_typesupport_introspection_cpp.so
                #${CMAKE_BINARY_DIR}/libros2_simple_logger__rosidl_generator_c.so
                #${CMAKE_BINARY_DIR}/libros2_simple_logger__rosidl_typesupport_introspection_c.so
                -pthread
        )



        add_executable(ros2_simple_logger_echo${target_suffix} ${CMAKE_CURRENT_SOURCE_DIR}/src/ros2_simple_logger/SimpleLoggerEcho.cpp)
        add_dependencies(ros2_simple_logger_test${target_suffix} ${PROJECT_NAME})
        get_rmw_typesupport(typesupport_impls "${rmw_implementation}" LANGUAGE "cpp")
        target_compile_definitions(ros2_simple_logger_echo${target_suffix} PUBLIC "RMW_IMPLEMENTATION=${rmw_implementation}")

        foreach(typesupport_impl ${typesupport_impls})
            rosidl_target_interfaces(ros2_simple_logger_echo${target_suffix} ${PROJECT_NAME} ${typesupport_impl})
        endforeach()

        ament_target_dependencies(ros2_simple_logger_echo${target_suffix} ${CMAKE_CURRENT_SOURCE_DIR}/src/ros2_simple_logger/SimpleLoggerEcho.cpp
                "rclcpp${target_suffix}"
                 ros2_simple_logger_lib${target_suffix}
                 pthread
        )
        target_link_libraries(ros2_simple_logger_echo${target_suffix}
                ros2_simple_logger_lib${target_suffix}
                #${CMAKE_BINARY_DIR}/libros2_simple_logger__rosidl_typesupport_introspection_cpp.so
                #${CMAKE_BINARY_DIR}/libros2_simple_logger__rosidl_generator_c.so
                #${CMAKE_BINARY_DIR}/libros2_simple_logger__rosidl_typesupport_introspection_c.so
                -pthread
        )

        install(TARGETS ros2_simple_logger_lib DESTINATION lib)

        install(TARGETS ros2_simple_logger_test DESTINATION tests)
        install(TARGETS ros2_simple_logger_echo DESTINATION bin)


endmacro()



call_for_each_rmw_implementation(targets GENERATE_DEFAULT)



file(GLOB INCLUDE_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/ros2_simple_logger/*.h*"
)

install(FILES ${INCLUDE_FILES} DESTINATION include/ros2_simple_logger) 


file(GLOB BINARY_FILES ${CMAKE_BINARY_DIR}/*.so)
install(FILES ${BINARY_FILES} DESTINATION lib)

ament_export_include_directories(include)

ament_export_libraries(ros2_simple_logger_lib)

ament_package()
