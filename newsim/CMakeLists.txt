cmake_minimum_required(VERSION 3.0)

cmake_policy(SET CMP0004 OLD)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMakeFiles")

FIND_PACKAGE(Boost COMPONENTS system program_options thread log log_setup)
FIND_PACKAGE(ZLIB REQUIRED)
find_package(jsoncpp REQUIRED)

set (CMAKE_CXX_STANDARD 11)

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

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS} ${JSONCPP_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/../libopencraft/include)

FILE(GLOB newsim_src                    ${CMAKE_SOURCE_DIR}/src/*.cpp)

ADD_EXECUTABLE(newsim ${newsim_src} )


TARGET_LINK_LIBRARIES(newsim ${CMAKE_SOURCE_DIR}/../libopencraft/build/libopencraft.so pthread zmq zmqpp ${JSONCPP_LIBRARIES}  ${ZLIB_LIBRARIES} ${Boost_LIBRARIES})

