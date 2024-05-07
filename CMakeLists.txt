###################################################################
#
# reference：
# (1) https://cmake.org/cmake/help/v3.0/command/get_filename_component.html
# (2) configuration file of OpenCV/VINS/GPSTk
#
# Copyright(c)
#
# 2023
###################################################################

cmake_minimum_required(VERSION 3.10.2)

project(FiPPP VERSION 1.0)

# debug means a detailed output
# release will give a very brief hint during compilation
set(CMAKE_BUILD_TYPE "Debug")
# set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /DEBUG /INCREMENTAL")
set(CMAKE_CXX_FLAGS "-std=c++11")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall -g")
add_definitions("-DWIN32")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    MATH(EXPR stack_size "1600000000")
    set(CMAKE_EXE_LINKER_FLAGS "-Wl,--stack,${stack_size}")
endif ()

include_directories("./third_party/")

# This sets up variables contining GNU standard installation locations.
include( GNUInstallDirs )
include( BuildSetup.cmake )

set(SOURCE_DIR "src")

file(GLOB_RECURSE HEADERS   "${SOURCE_DIR}/*.h" EXCLUDE "${SOURCE_DIR}/FiPPPSolver/*"  )
file(GLOB_RECURSE HEADERS2  "${SOURCE_DIR}/*.hpp" EXCLUDE "${SOURCE_DIR}/FiPPPSolver/*")

message(${HEADERS})

set (INCLUDE_DIRS "")
foreach (_headerFile ${HEADERS})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND INCLUDE_DIRS ${_dir})
endforeach()

foreach (_headerFile ${HEADERS2})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND INCLUDE_DIRS ${_dir})
endforeach()

list(REMOVE_DUPLICATES INCLUDE_DIRS)

include_directories(${INCLUDE_DIRS})

file(GLOB_RECURSE SOURCES   "${SOURCE_DIR}/*.c" EXCLUDE "${SOURCE_DIR}/FiPPPSolver/*"  )
file(GLOB_RECURSE SOURCES2  "${SOURCE_DIR}/*.cpp" EXCLUDE "${SOURCE_DIR}/FiPPPSolver/*")

source_group("Header Files" FILES ${INCLUDE_DIRS})

add_library(FiPPP ${STADYN} ${SOURCES} ${SOURCES2} ${HEADERS} ${HEADERS2})

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_definitions("-DGNU")
    target_link_directories(FiPPP PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/third_party/CLAPACKlib/lapacklib)
    target_link_libraries(FiPPP blas lapack)
endif()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    target_link_directories(FiPPP PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/third_party/CLAPACKlib/debug)
    target_link_directories(FiPPP PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/third_party/CLAPACKlib/release)
    target_link_libraries(FiPPP blasd lapackd libf2cd)
endif()

# Install the FiPPP library and headers
install(TARGETS FiPPP DESTINATION lib)
install(FILES ${HEADERS}   ${HEADERS2}  DESTINATION include/FiPPP )

add_subdirectory(${SOURCE_DIR}/FiPPPSolver)
