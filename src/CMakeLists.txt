set(HEADER_DIR ${PROJECT_SOURCE_DIR}/inc)
set(HEADERS 
${HEADER_DIR}/engine.hpp
${HEADER_DIR}/engine_types.hpp
${HEADER_DIR}/gf_vulkan.hpp
${HEADER_DIR}/gf_options.hpp
${HEADER_DIR}/gf_err.hpp
)

set(SOURCES
main.cpp
engine_main.cpp
gf_vulkan.cpp
gf_options.cpp
gf_err.cpp
)

# Adding Groups
file (GLOB_RECURSE HEADER_GL
    "${HEADER_DIR}/gl_*.hpp"
)
source_group("WInput Header" FILES ${HEADER_GL})

file (GLOB_RECURSE SRC_GL
    "gl_*.cpp"
)
source_group("WInput Source" FILES ${SRC_GL})

file (GLOB_RECURSE HEADER_VK
    "${HEADER_DIR}/vk_*.hpp"
)
source_group("Vulkan Header" FILES ${HEADER_VK})

file (GLOB_RECURSE SRC_VK
    "vk_*.cpp"
)
source_group("Vulkan Source" FILES ${SRC_VK})

source_group("Other Header" FILES ${HEADERS})
source_group("Other Source" FILES ${SOURCES})

add_executable(garfish
${HEADER_GL}
${SRC_GL}
${HEADER_VK}
${SRC_VK}
${SOURCES}
${HEADERS}
)

set_target_properties(garfish PROPERTIES 
LINKER_LANGUAGE CXX
CXX_STANDARD 20)

target_include_directories(garfish PUBLIC "${PROJECT_SOURCE_DIR}/inc")

target_link_libraries(garfish PUBLIC Vulkan::Vulkan glfw vma vkbootstrap imgui glm stb_image fastgltf::fastgltf)