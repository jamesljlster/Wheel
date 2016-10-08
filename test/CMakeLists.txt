cmake_minimum_required(VERSION 2.8)

# Variables
set(PROJECT_NAME Wheel_test)

# Set project
project(${PROJECT_NAME} C CXX)

# Set file list
set(PROJECT_SRCS
	${CMAKE_CURRENT_SOURCE_DIR}/test.c
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
include_directories(${SERIALPORT_INCLUDE_DIR})

set(PROJECT_DEPS ${SERIALPORT_LIB} Wheel)
if(UNIX)
	find_library(CONIO_LIB conio
		"/usr/lib"
		"/usr/local/lib"
		)
	find_path(CONIO_INCLUDE_DIR conio.h
		"/usr/include"
		"/usr/local/include"
		)
	include_directories(${CONIO_INCLUDE_DIR})
	set(PROJECT_DEPS
		${PROJECT_DEPS}
		${CONIO_LIB}
		)
elseif(MINGW)
	set(PROJECT_DEPS
		${PROJECT_DEPS}
		setupapi
		)
endif()
	

# Build test
add_executable(${PROJECT_NAME} ${PROJECT_SRCS})
set_target_properties(${PROJECT_NAME}
	PROPERTIES
	OUTPUT_NAME	${PROJECT_NAME}
)
target_link_libraries(${PROJECT_NAME} ${PROJECT_DEPS})

# Install
install(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/bin"
)
