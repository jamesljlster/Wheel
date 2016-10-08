cmake_minimum_required(VERSION 2.8)

# Variables
set(PROJECT_NAME Wheel)

# Set project
project(${PROJECT_NAME} C CXX)

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
find_library(SERIALPORT_LIB serialport
	"/usr/lib"
	"/usr/local/lib"
	)
find_path(SERIALPORT_INCLUDE_DIR libserialport.h
	"/usr/include"
	"/usr/local/include"
	)
set(PROJECT_DEPS ${SERIALPORT_LIB})
if(MINGW)
	set(PROJECT_DEPS
		${PROJECT_DEPS}
		setupapi
		)
endif()
include_directories(${SERIALPORT_INCLUDE_DIR})

# Build Wheel library
add_library(${PROJECT_NAME} STATIC ${PROJECT_SRCS})
set_target_properties(${PROJECT_NAME}
	PROPERTIES
		OUTPUT_NAME		${PROJECT_NAME}
		PREFIX			"lib"
		PUBLIC_HEADER	${PROJECT_HEADER}
	)
target_link_libraries(${PROJECT_NAME} ${PROJECT_DEPS})

# Install
install(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/bin"
	ARCHIVE DESTINATION "${CMAKE_INSTALL_PREFIX}/lib"
	LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib"
	PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_PREFIX}/include"
	)

