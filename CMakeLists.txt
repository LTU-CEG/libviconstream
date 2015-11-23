cmake_minimum_required(VERSION 2.8.3)
project(vicon_stream)

set(CMAKE_CXX_FLAGS "-std=c++14 ${CMAKE_CXX_FLAGS}")

# Check the architecture
set(HOST_ARCH linux32)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(HOST_ARCH linux64)
endif()

message(STATUS HOST_ARCH=${HOST_ARCH})

# Check arch and select correct libs and includes for Vicon
if (HOST_BITNESS EQUAL linux64)
    file(GLOB_RECURSE MYLIB2 lib/Linux_DataStreamSDK1_5/Linux64-boost-1.53.0/libViconDataStreamSDK_CPP.*)
else()
    file(GLOB_RECURSE MYLIB2 lib/Linux_DataStreamSDK1_5/Linux32-boost-1.53.0/libViconDataStreamSDK_CPP.*)
endif()

get_filename_component(MY_INC ${MYLIB2} DIRECTORY)
message(STATUS MYLIB2=${MYLIB2})
message(STATUS MY_INC=${MY_INC})


#message(STATUS " ")
#get_cmake_property(_variableNames VARIABLES)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()
#message(STATUS " ")

