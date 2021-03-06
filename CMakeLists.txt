###          Copyright Emil Fresk 2015-2017.
### Distributed under the Boost Software License, Version 1.0.
###    (See accompanying file LICENSE.md or copy at
###          http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.1)

########################################
# Project name
########################################
project(libviconstream)
find_package(catkin QUIET)

########################################
# Enable C++11
########################################
set (CMAKE_CXX_STANDARD 11)

########################################
# Check the architecture
########################################
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(HOST_ARCH linux64)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(HOST_ARCH linux32)
else()
    message(FATAL_ERROR "Configuration error: Unable to detect if 64 or 32-bit.
    CMAKE_SIZEOF_VOID_P is not 4 or 8: CMAKE_SIZEOF_VOID_P="
    ${CMAKE_SIZEOF_VOID_P})
endif()

########################################
# Check arch and select correct libs
########################################
if (HOST_ARCH STREQUAL linux64)
    set(VICON_SDK_PATH lib/Linux_DataStreamSDK1_5/Linux64-boost-1.53.0)
else()
    set(VICON_SDK_PATH lib/Linux_DataStreamSDK1_5/Linux32-boost-1.53.0)
endif()

file(GLOB_RECURSE VICON_SDK     ${VICON_SDK_PATH}/libViconDataStreamSDK_CPP.*)
file(GLOB_RECURSE VICON_DEBUG   ${VICON_SDK_PATH}/libDebug*)
file(GLOB_RECURSE VICON_BTHREAD ${VICON_SDK_PATH}/libboost_thread*)
file(GLOB_RECURSE VICON_BUT     ${VICON_SDK_PATH}/libboost_unit_test*)
file(GLOB_RECURSE VICON_BSYS    ${VICON_SDK_PATH}/libboost_system*)
file(GLOB_RECURSE VICON_BLOC    ${VICON_SDK_PATH}/libboost_locale*)

########################################
# catkin requirements
########################################
if (catkin_FOUND)
    catkin_package(
      #DEPENDS pthread
        INCLUDE_DIRS ${catkin_INCLUDE_DIRS} include
        LIBRARIES ${PROJECT_NAME} ${VICON_SDK} ${VICON_DEBUG} ${VICON_BUT}
        ${VICON_BSYS} ${VICON_BLOC}
    )
endif()

########################################
# Library linking and source
########################################
include_directories(${catkin_INCLUDE_DIRS}
                    include)

add_library(${PROJECT_NAME}
            src/viconstream.cpp)

if (catkin_FOUND)
    add_dependencies(${PROJECT_NAME}
                     ${${PROJECT_NAME}_EXPORTED_TARGETS}
                     ${catkin_EXPORTED_TARGETS})
endif()

target_link_libraries(${PROJECT_NAME}
                      ${catkin_LIBRARIES}
                      ${VICON_SDK}
                      ${VICON_DEBUG}
                      ${VICON_BUT}
                      ${VICON_BSYS}
                      ${VICON_BLOC}
                      pthread)

########################################
# Include the example in the build
########################################
# add_subdirectory(example)

########################################
# Messages
########################################
message(STATUS " ")
message(STATUS "   Paths and variables")
message(STATUS "  ---------------------")
message(STATUS HOST_ARCH      = ${HOST_ARCH})
message(STATUS VICON_SDK_PATH = ${VICON_SDK_PATH})
message(STATUS VICON_SDK      = ${VICON_SDK})
message(STATUS VICON_DEBUG    = ${VICON_DEBUG})
message(STATUS VICON_BTHREAD  = ${VICON_BTHREAD})
message(STATUS VICON_BUT      = ${VICON_BUT})
message(STATUS VICON_BSYS     = ${VICON_BSYS})
message(STATUS VICON_BLOC     = ${VICON_BLOC})
message(STATUS " ")

