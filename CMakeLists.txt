############################################################################
#
# Copyright (C) 2015 Emil Fresk.
# All rights reserved.
#
# This file is part of the cppViconStream library.
#
# GNU Lesser General Public License Usage
# This file may be used under the terms of the GNU Lesser
# General Public License version 3.0 as published by the Free Software
# Foundation and appearing in the file LICENSE included in the
# packaging of this file.  Please review the following information to
# ensure the GNU Lesser General Public License version 3.0 requirements
# will be met: http://www.gnu.org/licenses/lgpl-3.0.html.
#
# If you have questions regarding the use of this file, please contact
# Emil Fresk at emil.fresk@gmail.com.
#
############################################################################

cmake_minimum_required(VERSION 2.8.3)

########################################
# Project name
########################################
project(vicon_stream)

########################################
# Enable C++14 and other C++ flags
########################################
set(CMAKE_CXX_FLAGS "-std=c++14 ${CMAKE_CXX_FLAGS}")

########################################
# Check the architecture
########################################
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(HOST_ARCH linux64)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(HOST_ARCH linux32)
else()
    message(FATAL_ERROR "Configuration error: Unable to detect if 64 or 32-bit.
    CMAKE_SIZEOF_VOID_P is not 4 or 8: CMAKE_SIZEOF_VOID_P=" ${CMAKE_SIZEOF_VOID_P})
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
# Library linking and source
########################################
add_library(vicon_stream SHARED viconstream.cpp)
target_link_libraries(vicon_stream ${VICON_SDK} ${VICON_DEBUG} ${VICON_BUT}
    ${VICON_BSYS} ${VICON_BLOC})

########################################
# Messages
########################################
message(STATUS " ")
message(STATUS "   Paths and variables")
message(STATUS "  ---------------------")
message(STATUS HOST_ARCH=${HOST_ARCH})
message(STATUS VICON_SDK_PATH=${VICON_SDK_PATH})
message(STATUS VICON_SDK=${VICON_SDK})
message(STATUS VICON_DEBUG=${VICON_DEBUG})
message(STATUS VICON_BTHREAD=${VICON_BTHREAD})
message(STATUS VICON_BUT=${VICON_BUT})
message(STATUS VICON_BSYS=${VICON_BSYS})
message(STATUS VICON_BLOC=${VICON_BLOC})
message(STATUS " ")

