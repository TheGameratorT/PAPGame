cmake_minimum_required(VERSION 3.20)

# set the project name and version
project(ServerTest VERSION 1.0)

# release mode flags
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    # static link if gcc
    if((NOT MSVC) AND WIN32)
        set(Boost_USE_STATIC_LIBS ON)
        set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
        set(BUILD_SHARED_LIBS OFF)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
    endif()
endif()

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

find_package(Boost 1.77 REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

# find the common files
file(GLOB_RECURSE SOURCES
    "source/*.cpp"
    "../common/*.cpp"
)

# add the executable
add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE "../common")

target_link_libraries(${PROJECT_NAME}
    wsock32
    ws2_32
)
