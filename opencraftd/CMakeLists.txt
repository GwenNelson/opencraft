cmake_minimum_required(VERSION 2.8)

project("opencraftd")

ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)
find_package(Boost 1.55 REQUIRED COMPONENTS system program_options thread log log_setup)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${Boost_INCLUDE_DIRS})

add_executable(opencraftd opencraft_server.cpp main.cpp)

target_link_libraries(opencraftd pthread ${Boost_LIBRARIES})
