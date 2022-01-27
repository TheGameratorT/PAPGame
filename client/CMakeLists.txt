cmake_minimum_required(VERSION 3.20)

# set the project name and version
project(ClientTest VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

set(LIB_DIR "${PROJECT_SOURCE_DIR}/libs")
if(CMAKE_SIZEOF_VOID_P STREQUAL "4")
    set(LIB_ARCH "x86")
else()
    set(LIB_ARCH "x64")
endif()
if (MSVC)
    set(LIB_SUFFIX ".lib")
else()
    set(LIB_SUFFIX ".a")
endif()

set(STATIC_BUILD FALSE)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(STATIC_BUILD TRUE)
endif()

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_FRAMEWORK FALSE) # OpenAL

if(WIN32)
    add_compile_definitions(_WIN32_WINNT=0x0601) # Boost Asio
endif()

# static link
if(STATIC_BUILD)
    add_compile_definitions(AL_LIBTYPE_STATIC GLEW_STATIC)
    set(LIBTYPE STATIC) # OpenAL
    set(Boost_USE_STATIC_LIBS ON)
    set(BUILD_SHARED_LIBS OFF)
    set(CMAKE_FIND_LIBRARY_SUFFIXES ${LIB_SUFFIX})
    if(NOT MSVC)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
    endif()
endif()

find_package(Boost 1.77 REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

if (STATIC_BUILD)
    add_library(glew_s STATIC IMPORTED)
    set_target_properties(glew_s PROPERTIES
        IMPORTED_LOCATION "${LIB_DIR}/glew/${LIB_ARCH}/glew32s${LIB_SUFFIX}"
        INTERFACE_INCLUDE_DIRECTORIES "${LIB_DIR}/glew/include")
else()
    add_library(glew SHARED IMPORTED)
    set_target_properties(glew PROPERTIES
        IMPORTED_IMPLIB "${LIB_DIR}/glew/${LIB_ARCH}/glew32${LIB_SUFFIX}"
        INTERFACE_INCLUDE_DIRECTORIES "${LIB_DIR}/glew/include")
endif()

add_subdirectory("${LIB_DIR}/glfw")
add_subdirectory("${LIB_DIR}/glm")
add_subdirectory("${LIB_DIR}/libogg")
add_subdirectory("${LIB_DIR}/libvorbis")
add_subdirectory("${LIB_DIR}/openal-soft")

# find the source files
file(GLOB_RECURSE SOURCES
    "source/*.cpp"
    "../common/*.cpp"
)

# add the executable
add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    "../common"
    ${OPENAL_INCLUDE_DIR}
    ${Ogg_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    wsock32
    ws2_32
    glfw
    glm
    OpenAL
    ogg
    vorbis
    vorbisfile
)

if (STATIC_BUILD)
    target_link_libraries(${PROJECT_NAME} PRIVATE glew_s)
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE glew)
endif()
