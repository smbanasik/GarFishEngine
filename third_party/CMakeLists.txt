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