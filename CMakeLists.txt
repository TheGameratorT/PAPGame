cmake_minimum_required(VERSION 3.20)

# set the project name and version
project(PAPGame VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

add_subdirectory(client)
add_subdirectory(server)
