cmake_minimum_required(VERSION 2.8)

# Set project
project(Wheel_test C CXX)

# Set file list
set(WHEEL_TEST_SRCS
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

set(WHEEL_TEST_DEPS ${SERIALPORT_LIB} Wheel)
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
	set(WHEEL_TEST_DEPS
		${WHEEL_TEST_DEPS}
		${CONIO_LIB}
		)
endif()

# Build test
add_executable(WHEEL_TEST_BIN ${WHEEL_TEST_SRCS})
set_target_properties(WHEEL_TEST_BIN
	PROPERTIES
	OUTPUT_NAME	"Wheel_test"
)
target_link_libraries(WHEEL_TEST_BIN ${WHEEL_TEST_DEPS})

# Install
install(TARGETS WHEEL_TEST_BIN
	RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/bin"
)
