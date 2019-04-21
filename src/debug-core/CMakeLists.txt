cmake_minimum_required(VERSION 2.8.11)
project(ec2-new)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../ec2drv)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../ec2drv)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(DBGCORE_SRC
	cdbfile.cpp
	ihex.c
	memremap.cpp
	module.cpp
	symbol.cpp
	symtab.cpp
	symtypetree.cpp
	target.cpp
	targets51.cpp
	targetsilabs.cpp
	dbgsession.cpp
	outformat.cpp
	contextmgr.cpp
	breakpointmgr.cpp
	linespec.cpp
	target-dummy.cpp
)

add_library(dbgcore STATIC ${DBGCORE_SRC})
target_link_libraries(dbgcore ec2drv usb)
