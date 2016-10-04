cmake_minimum_required(VERSION 2.8)

# Set project
project(Wheel C CXX)

# Include directory
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Set file list
set(WHEEL_SRCS
	${CMAKE_CURRENT_SOURCE_DIR}/Wheel.c
	)

set(WHEEL_HEADER
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
set(WHEEL_DEPS ${SERIALPORT_LIB})
include_directories(${SERIALPORT_INCLUDE_DIR})

# Build Wheel library
add_library(Wheel STATIC ${WHEEL_SRCS})
set_target_properties(Wheel
	PROPERTIES
		OUTPUT_NAME		"Wheel"
		PREFIX			"lib"
		PUBLIC_HEADER	${WHEEL_HEADER}
	)
target_link_libraries(Wheel ${WHEEL_DEPS})

# Install
install(TARGETS Wheel
	RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/bin"
	ARCHIVE DESTINATION "${CMAKE_INSTALL_PREFIX}/lib"
	LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib"
	PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_PREFIX}/include"
	)

