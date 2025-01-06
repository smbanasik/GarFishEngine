set(HEADER_DIR ${PROJECT_SOURCE_DIR}/inc)
set(HEADERS 
${HEADER_DIR}/engine.hpp
${HEADER_DIR}/engine_types.hpp
${HEADER_DIR}/gf_window.hpp
${HEADER_DIR}/gf_vulkan.hpp
${HEADER_DIR}/vk_initializers.hpp
${HEADER_DIR}/vk_images.hpp
${HEADER_DIR}/vk_types.hpp
${HEADER_DIR}/vk_descriptors.hpp
${HEADER_DIR}/vk_pipelines.hpp
${HEADER_DIR}/vk_loader.hpp
${HEADER_DIR}/vk_materials.hpp
${HEADER_DIR}/vk_renderable.hpp
${HEADER_DIR}/gf_options.hpp
${HEADER_DIR}/gl_manager.hpp
${HEADER_DIR}/gl_window.hpp
${HEADER_DIR}/gl_keys.hpp
${HEADER_DIR}/gl_mouse.hpp
${HEADER_DIR}/gl_joystick.hpp
${HEADER_DIR}/gl_types.hpp
${HEADER_DIR}/vk_core.hpp
)

set(SOURCES
main.cpp
engine_main.cpp
gf_window.cpp
gf_vulkan.cpp
vk_initializers.cpp
vk_images.cpp
vk_descriptors.cpp
vk_pipelines.cpp
vk_loader.cpp
vk_materials.cpp
vk_renderable.cpp
gf_options.cpp
gl_manager.cpp
gl_window.cpp
gl_keys.cpp
gl_mouse.cpp
gl_joystick.cpp
vk_core.cpp
)

add_executable(garfish
${SOURCES}
${HEADERS}
)

set_target_properties(garfish PROPERTIES 
LINKER_LANGUAGE CXX
CXX_STANDARD 20)

target_include_directories(garfish PUBLIC "${PROJECT_SOURCE_DIR}/inc")

target_link_libraries(garfish PUBLIC Vulkan::Vulkan glfw vma vkbootstrap imgui glm stb_image fastgltf::fastgltf)