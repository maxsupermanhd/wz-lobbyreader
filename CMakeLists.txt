cmake_minimum_required(VERSION 2.8.11)
project(wz-lobbyreader)

add_library(lobbyreader lobbyreader.cpp)
target_include_directories(lobbyreader PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(lobbydemo lobbydemo.cpp)
target_link_libraries(lobbydemo LINK_PUBLIC lobbyreader)