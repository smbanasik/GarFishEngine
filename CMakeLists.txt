cmake_minimum_required (VERSION 3.25)

project("GarFish Engine" VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

set (VS_STARTUP_PROJECT garfish)

# Used for neovim
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Single-config generators need a build type; multi-config generators ignore it.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# Tracy Options
option(TRACY_ENABLE "Enable Tracy instrumentation" OFF)
option(TRACY_ON_DEMAND "Does not start profiling until specified if true" OFF)
option(TRACY_NO_EXIT "Waits for an incoming connection before terminating if true" OFF)

if(NOT TRACY_ENABLE AND (TRACY_ON_DEMAND OR TRACY_NO_EXIT))
    message(WARNING "Tracy variables do nothing unless TRACY_ENABLE is true!")
endif()

# Compile time values
set(ALIGAR_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(ALIGAR_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(ALIGAR_VERSION_PATCH ${PROJECT_VERSION_PATCH})
configure_file(
    "${PROJECT_SOURCE_DIR}/src/common/config.hpp.in"
    "${CMAKE_BINARY_DIR}/generated/common/config.hpp"
    @ONLY
)

# Used for GLM compatibiltiy with Vulkan (0,1 instead of -1,1)
add_compile_definitions(GLM_FORCE_DEPTH_ZERO_TO_ONE)
add_subdirectory(third_party)

# Shaders
find_program(GLSL_VALIDATOR glslangValidator REQUIRED HINTS /usr/bin /usr/local/bin $ENV{VULKAN_SDK}/Bin/ $ENV{VULKAN_SDK}/Bin32/)

file(GLOB_RECURSE GLSL_SOURCE_FILES CONFIGURE_DEPENDS
    "${PROJECT_SOURCE_DIR}/shaders/*.frag"
    "${PROJECT_SOURCE_DIR}/shaders/*.vert"
    "${PROJECT_SOURCE_DIR}/shaders/*.comp"
)

foreach(GLSL ${GLSL_SOURCE_FILES})
    message(STATUS "BUILDING SHADER")
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV "${CMAKE_BINARY_DIR}/shaders/${FILE_NAME}.spv")
    message(STATUS ${GLSL})
    add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/shaders
    COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
    DEPENDS ${GLSL})
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
    Shaders
    DEPENDS ${SPIRV_BINARY_FILES}
    )

# Src files and post build
add_subdirectory(src)

