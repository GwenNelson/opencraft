cmake_minimum_required(VERSION 2.8)

project("opencraftd")

ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)
find_package(Boost 1.55 REQUIRED COMPONENTS system program_options log)
include_directories( ${Boost_INCLUDE_DIRS})

add_executable(opencraftd main.cpp)

target_link_libraries(opencraftd pthread ${Boost_LIBRARIES})
