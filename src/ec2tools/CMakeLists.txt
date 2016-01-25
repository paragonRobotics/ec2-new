cmake_minimum_required(VERSION 2.8.11)
project(ec2-new)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../ec2drv)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../ec2drv)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(ec2writeflash writeflash.c ihex.c)
target_link_libraries(ec2writeflash ec2drv usb)

add_executable(ec2readflash readflash.c ihex.c)
target_link_libraries(ec2readflash ec2drv usb)

add_executable(ec2_update_fw ec2fw.c)
target_link_libraries(ec2_update_fw ec2drv usb)

add_executable(ec2readfw ec2rfw.c)
target_link_libraries(ec2readfw ec2drv usb)

add_executable(ec2device ec2device.cpp)
target_link_libraries(ec2device ec2drv usb)

add_executable(ec2adapters ec3adapters.c ihex.c)
target_link_libraries(ec2adapters ec2drv usb)

add_executable(ec2test-any ec2test-any.cpp ihex.c)
target_link_libraries(ec2test-any ec2drv usb)

