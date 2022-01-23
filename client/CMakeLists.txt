cmake_minimum_required(VERSION 3.20)

# set the project name and version
project(ClientTest VERSION 1.0)

# release mode flags
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    # static link if gcc
    ## if((NOT MSVC) AND WIN32)
    ##     add_compile_definitions(AL_LIBTYPE_STATIC GLEW_STATIC)
    ##     set(Boost_USE_STATIC_LIBS ON)
    ##     set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
    ##     set(BUILD_SHARED_LIBS OFF)
    ##     set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
    ## endif()
endif()

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

find_package(PkgConfig)

find_package(Boost 1.77 REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(OpenAL 1.1 REQUIRED)
pkg_check_modules(Ogg REQUIRED IMPORTED_TARGET ogg)
pkg_check_modules(OggVorbis REQUIRED IMPORTED_TARGET vorbis)
pkg_check_modules(OggVorbisFile REQUIRED IMPORTED_TARGET vorbisfile)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

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
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    wsock32
    ws2_32
    # winmm
    glfw
    GLEW::GLEW
    ${OPENAL_LIBRARY}
    PkgConfig::Ogg
    PkgConfig::OggVorbis
    PkgConfig::OggVorbisFile
)
