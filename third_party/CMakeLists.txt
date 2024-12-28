find_package(Vulkan REQUIRED)

add_subdirectory(glfw-3.4)


add_library(vma INTERFACE)
target_include_directories(vma INTERFACE vma)

add_library(glm INTERFACE)
target_include_directories(glm INTERFACE glm)

add_library(vkbootstrap STATIC)

target_sources(vkbootstrap PRIVATE
vkbootstrap/VkBootstrap.h
vkbootstrap/VkBootstrap.cpp)

target_include_directories(vkbootstrap PUBLIC vkbootstrap)
target_link_libraries(vkbootstrap PUBLIC Vulkan::Vulkan)

add_library(imgui STATIC)
target_include_directories(imgui PUBLIC imgui)
target_sources(imgui PRIVATE
imgui/imgui.h
imgui/imgui.cpp

imgui/imgui_demo.cpp
imgui/imgui_draw.cpp
imgui/imgui_widgets.cpp
imgui/imgui_tables.cpp
imgui/imgui_impl_vulkan.cpp
imgui/imgui_impl_glfw.cpp
)

target_link_libraries(imgui PUBLIC Vulkan::Vulkan glfw)