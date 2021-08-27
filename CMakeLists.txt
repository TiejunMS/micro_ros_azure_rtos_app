cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
set(CMAKE_C_STANDARD 99)

set(CORE_LIB_DIR ${CMAKE_SOURCE_DIR}/azure_rtos)

# Set the toolchain if not defined
if(DEFINED PCAP_NAME)
    set(CMAKE_TOOLCHAIN_FILE "cmake/linux.cmake")
    add_definitions("-m32")
    add_link_options(-m32)
    message(STATUS "Using PCAP with interface: ${PCAP_NAME}")
else()
    set(CMAKE_TOOLCHAIN_FILE "cmake/arm-gcc-cortex-m4.cmake")
endif()

include(cmake/utilities.cmake)

# Define the Project
project(stm32l4_azure_rtos_microros C ASM)

add_subdirectory(microros)

add_subdirectory(lib)
add_subdirectory(app)
