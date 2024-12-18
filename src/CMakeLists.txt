set(HEADER_DIR ${PROJECT_SOURCE_DIR}/inc)
set(HEADERS 
${HEADER_DIR}/engine.hpp
${HEADER_DIR}/engine_types.hpp
${HEADER_DIR}/gf_window.hpp
${HEADER_DIR}/gf_vulkan.hpp
${HEADER_DIR}/vk_initializers.hpp
${HEADER_DIR}/vk_images.hpp
${HEADER_DIR}/vk_types.hpp
)

set(SOURCES
main.cpp
engine_main.cpp
gf_window.cpp
gf_vulkan.cpp
vk_initializers.cpp
vk_images.cpp
)

add_executable(garfish
${SOURCES}
${HEADERS}
)

set_target_properties(garfish PROPERTIES 
LINKER_LANGUAGE CXX
CXX_STANDARD 20)

target_include_directories(garfish PUBLIC "${PROJECT_SOURCE_DIR}/inc")

target_link_libraries(garfish PUBLIC Vulkan::Vulkan glfw vma vkbootstrap glm)