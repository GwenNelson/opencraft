cmake_minimum_required(VERSION 3.0)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set (CMAKE_CXX_STANDARD 14)

FIND_PACKAGE(Boost COMPONENTS system program_options thread log log_setup)
FIND_PACKAGE(ZLIB REQUIRED)
find_package(jsoncpp REQUIRED)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-narrowing -fpermissive")

ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS} ${JSONCPP_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/test_data)

FILE(GLOB libopencraft-protocol_src ${CMAKE_SOURCE_DIR}/src/*.cpp)

ADD_LIBRARY(opencraft-protocol SHARED ${libopencraft-protocol_src} ${CMAKE_SOURCE_DIR}/src/packets.autogen.cpp)

TARGET_LINK_LIBRARIES(opencraft-protocol pthread ${ZLIB_LIBRARIES} ${Boost_LIBRARIES})

