set(SRC_DIRS
    "${CMAKE_CURRENT_SOURCE_DIR}/window_input"
    "${CMAKE_CURRENT_SOURCE_DIR}/garfish"
    "${CMAKE_CURRENT_SOURCE_DIR}/garfish_user"
    "${CMAKE_CURRENT_SOURCE_DIR}/vulkan_low"
    "${CMAKE_CURRENT_SOURCE_DIR}/vulkan_high"
    "${CMAKE_CURRENT_SOURCE_DIR}/common"
    "${CMAKE_CURRENT_SOURCE_DIR}/vulkan_text"
)

set(SOURCES "")
set(HEADERS "")

foreach(dir IN LISTS SRC_DIRS)
    file(GLOB_RECURSE dir_sources CONFIGURE_DEPENDS "${dir}/*.cpp")
    file(GLOB_RECURSE dir_headers CONFIGURE_DEPENDS "${dir}/*.hpp")

    list(APPEND SOURCES ${dir_sources})
    list(APPEND HEADERS ${dir_headers})
endforeach()

source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}"
    FILES ${SOURCES} ${HEADERS}
)

add_executable(garfish
    ${SOURCES}
    ${HEADERS}
)

add_dependencies(garfish Shaders)

set_target_properties(garfish PROPERTIES
    LINKER_LANGUAGE CXX
    CXX_STANDARD 20
)

target_include_directories(garfish PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(garfish PUBLIC Vulkan::Vulkan glfw vma vkbootstrap imgui glm stb_image fastgltf::fastgltf ft_lib)

add_custom_command(TARGET garfish POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_BINARY_DIR}/shaders
            $<TARGET_FILE_DIR:garfish>/shaders
)

# Assets
file(GLOB_RECURSE ASSETS CONFIGURE_DEPENDS
    "${PROJECT_SOURCE_DIR}/assets/*"
)

add_custom_command(TARGET garfish POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:garfish>/assets
)
