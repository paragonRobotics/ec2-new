cmake_minimum_required(VERSION 2.8.11)
project(ec2-new)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake
	config.h
)

add_custom_command(OUTPUT device_table.c
	COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/csv2c.py -i ${CMAKE_CURRENT_SOURCE_DIR}/device_table.csv -c device_table.c -e device_enum.h
	DEPENDS device_table.csv
)
add_custom_target(generate_device_table DEPENDS device_table.c)

add_library(ec2drv STATIC ec2drv.c devices.c device_table.c jtag_mode.c c2_mode.c boot.c)
add_dependencies(ec2drv generate_device_table)

