cmake_minimum_required(VERSION 2.8)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_C_FLAGS_RELEASE "-O3")

# Variables
set(PROJECT_NAME Wheel)

# Set project
project(${PROJECT_NAME} C)

# Include directory
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Set file list
set(PROJECT_SRCS
	${CMAKE_CURRENT_SOURCE_DIR}/Wheel.c
	)

set(PROJECT_HEADER
	${CMAKE_CURRENT_SOURCE_DIR}/Wheel.h
	)

# Set dependences
find_path(SERIALPORT_INCLUDE_DIR libserialport.h
	"/usr/include"
	"/usr/local/include"
	)
include_directories(${SERIALPORT_INCLUDE_DIR})

# Build Wheel library
add_library(${PROJECT_NAME} STATIC ${PROJECT_SRCS})
set_target_properties(${PROJECT_NAME}
	PROPERTIES
		OUTPUT_NAME		${PROJECT_NAME}
		PREFIX			"lib"
		PUBLIC_HEADER	${PROJECT_HEADER}
	)

# Install
install(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/bin"
	ARCHIVE DESTINATION "${CMAKE_INSTALL_PREFIX}/lib"
	LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib"
	PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_PREFIX}/include"
	)

