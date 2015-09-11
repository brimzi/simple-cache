cmake_minimum_required(VERSION 2.8)

set (SOLUTION_NAME "SimpleCache" CXX)
project(${SOLUTION_NAME})
add_subdirectory(cache-server)
add_subdirectory(test-client)