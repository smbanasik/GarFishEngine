find_package(Vulkan REQUIRED)
message(STATUS "Vulkan version: ${Vulkan_VERSION}")

add_subdirectory(glfw)
add_subdirectory(fastgltf)

add_library(vma INTERFACE)
target_include_directories(vma INTERFACE  ${CMAKE_CURRENT_LIST_DIR}/vma)

add_library(glm INTERFACE)
target_include_directories(glm INTERFACE  ${CMAKE_CURRENT_LIST_DIR}/glm)

add_library(stb_image INTERFACE)
target_include_directories(stb_image INTERFACE  ${CMAKE_CURRENT_LIST_DIR}/stb_image)

add_subdirectory(vk-bootstrap)

add_subdirectory(freetype)
add_library(ft_lib INTERFACE)
target_link_libraries(ft_lib INTERFACE freetype)

add_library(imgui STATIC)
target_include_directories(imgui PUBLIC  ${CMAKE_CURRENT_LIST_DIR}/imgui)
target_sources(imgui PRIVATE
imgui/imgui.cpp

imgui/imgui_demo.cpp
imgui/imgui_draw.cpp
imgui/imgui_widgets.cpp
imgui/imgui_tables.cpp
imgui/imgui_impl_vulkan.cpp
imgui/imgui_impl_glfw.cpp
)

target_link_libraries(imgui PUBLIC Vulkan::Vulkan glfw)

add_library(aligar_deps INTERFACE)
target_link_libraries(aligar_deps INTERFACE Vulkan::Vulkan glfw vma vk-bootstrap::vk-bootstrap imgui glm stb_image fastgltf::fastgltf ft_lib)