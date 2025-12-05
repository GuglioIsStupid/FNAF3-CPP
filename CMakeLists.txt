cmake_minimum_required(VERSION 3.16)
include(FetchContent)

project(FNAF3)

# force x64
if (WIN32 AND NOT CMAKE_GENERATOR_PLATFORM)
    set(CMAKE_GENERATOR_PLATFORM x64 CACHE STRING "" FORCE)
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)

# i disabled these cuz the libraries spewing warnings are annoying asf!!!!!!
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    #add_compile_options(-Wall -Wextra -Wpedantic)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    #add_compile_options(/W4)
    add_compile_options(/wd4251 /wd4275 /wd4996 /wd4003 /wd5033 /wd4005 /wd4244 /wd4819 /wd5287)
endif()

if (MSVC)
    add_compile_options(/Z7)
else()
    if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-O3)
    endif()
endif()

if (WIN32 AND NOT CMAKE_GENERATOR_PLATFORM)
    set(CMAKE_GENERATOR_PLATFORM x64 CACHE STRING "" FORCE)
endif()

if(APPLE) #guh
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_definitions(-DSDL_DISABLE_SIMD_ALIGNED)

# OpenGL
find_package(OpenGL REQUIRED)

include_directories(${OPENGL_INCLUDE_DIRS}
    "${PROJECT_SOURCE_DIR}/src"
    "${PROJECT_SOURCE_DIR}/external/glad/include"
    "${PROJECT_SOURCE_DIR}/external/imgui"
    "${PROJECT_SOURCE_DIR}/external/imgui/backends"
)

# FreeType
add_subdirectory("external/freetype")

# SDL3
set(SDL_DISABLE_GIT ON CACHE BOOL "" FORCE)
set(SDL_SKIP_GIT ON CACHE BOOL "" FORCE)
set(SDL_DONT_USE_GIT_REVISION ON CACHE BOOL "" FORCE)
set(SDL_GIT_REVISION "release" CACHE STRING "" FORCE)
set(GIT_FOUND OFF CACHE BOOL "" FORCE)
set(SDL_DISABLE_GIT_REVISION ON CACHE BOOL "" FORCE)
set(SDL_SHARED ON)
set(SDL_STATIC OFF)
set(SDL_OPENGLES OFF)
add_subdirectory(external/SDL EXCLUDE_FROM_ALL)

if (TARGET SDL3-static)
    add_library(SDL3::SDL3 ALIAS SDL3-static)
elseif (TARGET SDL3-shared)
    add_library(SDL3::SDL3 ALIAS SDL3-shared)
endif()

# SDL_image
set(SDLIMAGE_VENDORED ON)
set(SDLIMAGE_AVIF OFF)
set(SDLIMAGE_BMP OFF)
set(SDLIMAGE_JPEG ON)
set(SDLIMAGE_WEBP OFF)
add_subdirectory(external/SDL_image EXCLUDE_FROM_ALL)

# SDL_mixer
set(SDLMIXER_VENDORED ON)
add_subdirectory(external/SDL_mixer EXCLUDE_FROM_ALL)

# GLM
set(GLM_TEST_ENABLE OFF CACHE BOOL "GLM no test")
add_subdirectory(external/glm)

# Glad
set(GLAD_SOURCES "${PROJECT_SOURCE_DIR}/external/glad/src/glad.c")

# ImGui sources
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui)
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdl3.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# Project sources
file(GLOB_RECURSE PROJECT_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
)

if(DEFINED ENV{LUAJIT_LIB})
    set(LUAJIT_LIB $ENV{LUAJIT_LIB})
else()
    find_library(LUAJIT_LIB NAMES luajit lua51 PATHS /usr/lib /usr/local/lib)
endif()
if(DEFINED ENV{LUAJIT_INCLUDE_DIR})
    set(LUAJIT_INCLUDE_DIR $ENV{LUAJIT_INCLUDE_DIR})
else()
    find_path(LUAJIT_INCLUDE_DIR NAMES luajit.h PATHS /usr/include /usr/local/include)
endif()
if(NOT LUAJIT_LIB OR NOT LUAJIT_INCLUDE_DIR)
    message(FATAL_ERROR "LuaJIT not found. Please set LUAJIT_LIB and LUAJIT_INCLUDE_DIR")
endif()

include_directories(${LUAJIT_INCLUDE_DIR})

list(APPEND PROJECT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/external/miniz/miniz.c)

list(APPEND PROJECT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/external/easings/src/easing.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external)

if(WIN32)
    list(APPEND PROJECT_SOURCES ${APP_RESOURCE_FILE})
else()
    set(PROJECT_SOURCES)
endif()

add_executable(FNAF3 ${PROJECT_SOURCES} ${GLAD_SOURCES} ${IMGUI_SOURCES})

set_property(GLOBAL PROPERTY JOB_POOLS single_job=1 multi_job=8)
set_property(TARGET FNAF3 PROPERTY JOB_POOL_COMPILE multi_job)

# Includes
if (TARGET SDL3_image)
    message(STATUS "SDL3_image target found")
    target_include_directories(FNAF3 PRIVATE
        ${PROJECT_SOURCE_DIR}/external/SDL_image/include
    )
endif()
if (TARGET SDL3_mixer)
    message(STATUS "SDL3_mixer target found")
    target_include_directories(FNAF3 PRIVATE
        ${PROJECT_SOURCE_DIR}/external/SDL_mixer/include
    )
endif()
if (TARGET freetype)
    message(STATUS "Freetype target found")
    target_include_directories(FNAF3 PRIVATE
        ${PROJECT_SOURCE_DIR}/external/freetype/include
    )
endif()
if (TARGET glm)
    message(STATUS "GLM target found")
    target_include_directories(FNAF3 PRIVATE
        ${PROJECT_SOURCE_DIR}/external/glm
    )
endif()

# Dr_libs
target_include_directories(FNAF3 PRIVATE
    ${PROJECT_SOURCE_DIR}/external/dr_libs
)

# STB
target_include_directories(FNAF3 PRIVATE
    ${PROJECT_SOURCE_DIR}/external/stb
)

# Miniz
target_include_directories(FNAF3 PRIVATE
    ${PROJECT_SOURCE_DIR}/external/miniz
)

# Easings
target_include_directories(FNAF3 PRIVATE
    ${PROJECT_SOURCE_DIR}/external/easings/src
)

target_compile_definitions(FNAF3 PRIVATE
    $<$<CONFIG:Debug>:DEBUG=1>
    $<$<CONFIG:Release>:RELEASE=1>
)

target_link_libraries(FNAF3 PRIVATE SDL3::SDL3 ${OPENGL_LIBRARIES})
target_link_libraries(FNAF3 PRIVATE SDL3_image::SDL3_image)
target_link_libraries(FNAF3 PRIVATE SDL3_mixer::SDL3_mixer)
target_link_libraries(FNAF3 PRIVATE freetype)
target_link_libraries(FNAF3 PRIVATE ${LUAJIT_LIB})

if (UNIX)
    target_link_libraries(FNAF3 PRIVATE resolv)
endif()

set(ASSETS_SOURCE_DIR "${CMAKE_SOURCE_DIR}/assets")
set(ASSETS_DEST_DIR "${CMAKE_BINARY_DIR}/bin/Debug/assets")

add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E echo "Copying assets..."
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${ASSETS_SOURCE_DIR}" "${ASSETS_DEST_DIR}"
)

add_dependencies(FNAF3 copy_assets)