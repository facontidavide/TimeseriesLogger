cmake_minimum_required(VERSION 2.8)

add_definitions( -std=c++11)

project(TimeseriesLogger)
add_executable(${PROJECT_NAME} main.cpp timeseries_logger.cpp)
