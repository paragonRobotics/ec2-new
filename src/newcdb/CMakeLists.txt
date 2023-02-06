cmake_minimum_required(VERSION 2.8.11)
project(ec2-new)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../ec2drv)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../ec2drv)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../debug-core)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../debug-core)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(NEWCDB_SRC
	parsecmd.cpp
	cmdcommon.cpp
	cmdbreakpoints.cpp
	cmddisassemble.cpp
	cmdmaintenance.cpp
	cmdreadwrite.cpp
	cmdtarget.cpp
	utils.cpp
	main.cpp
)

add_executable(newcdb ${NEWCDB_SRC})
target_link_libraries(newcdb ec2drv dbgcore)

