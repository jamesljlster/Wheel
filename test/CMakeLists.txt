cmake_minimum_required(VERSION 2.8)

# Set project
project(Wheel_Test C CXX)

# Set file list
file(GLOB PROJECT_SRCS 
	${CMAKE_CURRENT_SOURCE_DIR}/*.c
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
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

set(PROJECT_DEPS Wheel ${SERIALPORT_LIB})
if(MINGW)
	set(PROJECT_DEPS
		${PROJECT_DEPS}
		setupapi
		)
elseif(UNIX)
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

endif()

# Build test
foreach(TEST_FILE_PATH ${PROJECT_SRCS})
	# Get each file name
	get_filename_component(TEST_FILE_NAME ${TEST_FILE_PATH} NAME_WE)
	
	# Build executable
	add_executable(${TEST_FILE_NAME} ${TEST_FILE_PATH})
	set_target_properties(${TEST_FILE_NAME}
		PROPERTIES OUTPUT_NAME ${TEST_FILE_NAME}
		)
	target_link_libraries(${TEST_FILE_NAME} ${PROJECT_DEPS})

	# Install
	install(TARGETS ${TEST_FILE_NAME}
		RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/bin"
		)
endforeach()

