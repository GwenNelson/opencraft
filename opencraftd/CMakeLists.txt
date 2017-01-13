cmake_minimum_required(VERSION 3.0)

project("opencraftd")

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

ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)
find_package(Boost 1.55 REQUIRED COMPONENTS system program_options thread log log_setup)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${Boost_INCLUDE_DIRS})

add_executable(opencraftd utils.cpp bbuff.cpp opencraft_client_connection.cpp opencraft_server.cpp main.cpp)

target_link_libraries(opencraftd pthread ${Boost_LIBRARIES})
