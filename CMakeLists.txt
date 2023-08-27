#
# Copyright (c) 2023 juskim. All rights reserved.
# GitHub: jus-kim, YouTube: @juskim
#

cmake_minimum_required(VERSION 3.20.0)
add_compile_options(-Werror)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(nimBLE)

# Include syscall directory for custom Zephyr-based driver headers
list(APPEND SYSCALL_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SYSCALL_INCLUDE_DIRS ${SYSCALL_INCLUDE_DIRS})

# Include all header files
add_subdirectory(src)
zephyr_include_directories(include)
