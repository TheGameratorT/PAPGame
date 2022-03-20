cmake_minimum_required(VERSION 3.20)

# set the project name and version
project(papgame VERSION 1.0)

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

message("[PAPGameC] Static build: ${STATIC_BUILD}")

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
    set(PNG_SHARED FALSE)
    set(PNG_STATIC TRUE)
    set(Boost_USE_STATIC_LIBS ON)
    set(BUILD_SHARED_LIBS OFF)
    set(CMAKE_FIND_LIBRARY_SUFFIXES ${LIB_SUFFIX})
    if(NOT MSVC)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
    endif()
else()
    set(PNG_SHARED TRUE)
    set(PNG_STATIC FALSE)
endif()

find_package(Boost 1.77 REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

find_package(OpenGL REQUIRED)

set(PNG_BUILD_ZLIB TRUE)

add_subdirectory("${LIB_DIR}/glew/build/cmake")
add_subdirectory("${LIB_DIR}/glfw")
add_subdirectory("${LIB_DIR}/glm")
add_subdirectory("${LIB_DIR}/libogg")
add_subdirectory("${LIB_DIR}/libvorbis")
add_subdirectory("${LIB_DIR}/openal-soft")
add_subdirectory("${LIB_DIR}/zlib")

if (STATIC_BUILD)
    set(ZLIB_LIBRARY $<TARGET_LINKER_FILE:zlibstatic>)
else()
    set(ZLIB_LIBRARY $<TARGET_LINKER_FILE:zlib>)
endif()
set(ZLIB_INCLUDE_DIR "${LIB_DIR}/zlib" "${CMAKE_CURRENT_BINARY_DIR}/libs/zlib")

add_subdirectory("${LIB_DIR}/libpng")

# find the source files
file(GLOB_RECURSE SOURCES
    "source/*.cpp"
    "../common/*.cpp"
)

set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/appicon.rc")

# add the executable
add_executable(${PROJECT_NAME} ${SOURCES} ${APP_ICON_RESOURCE_WINDOWS})

target_include_directories(${PROJECT_NAME} PRIVATE
    "../common"
    ${OPENAL_INCLUDE_DIR}
    ${Ogg_INCLUDE_DIR}
    ${ZLIB_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    wsock32
    ws2_32
    opengl32
    glfw
    glm
    OpenAL
    ogg
    vorbis
    vorbisfile
)

if (STATIC_BUILD)
    target_link_libraries(${PROJECT_NAME} PRIVATE glew_s zlibstatic png_static)
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE glew zlib png)

    set(IMPORTED_LIBRARY_DLLS
        $<TARGET_FILE:glew>
        $<TARGET_FILE:OpenAL>
        $<TARGET_FILE:zlib>
        $<TARGET_FILE:png>
    )

    foreach(file_i ${IMPORTED_LIBRARY_DLLS})
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${file_i}
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
        )
    endforeach()
endif()
