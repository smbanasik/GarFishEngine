set(DIR_WI ${PROJECT_SOURCE_DIR}/window_input)
set(DIR_GF ${PROJECT_SOURCE_DIR}/garfish)
set(DIR_GFU ${PROJECT_SOURCE_DIR}/garfish_user)
set(DIR_VKL ${PROJECT_SOURCE_DIR}/vulkan_low)
set(DIR_VKH ${PROJECT_SOURCE_DIR}/vulkan_high)
set(DIR_OTH ${PROJECT_SOURCE_DIR}/other)

file(GLOB_RECURSE HEADER_WI
    "${DIR_WI}/*.hpp
)
file(GLOB_RECURSE SOURCE_WI
    "${DIR_WI}/*.cpp
)
file(GLOB_RECURSE HEADER_GF
    "${DIR_GF}/*.hpp
)
file(GLOB_RECURSE SOURCE_GF
    "${DIR_GF}/*.cpp
)
file(GLOB_RECURSE HEADER_GFU
    "${DIR_GFU}/*.hpp
)
file(GLOB_RECURSE SOURCE_GFU
    "${DIR_GFU}/*.cpp
)
file(GLOB_RECURSE HEADER_VKL
    "${DIR_VKL}/*.hpp
)
file(GLOB_RECURSE SOURCE_VKL
    "${DIR_VKL}/*.cpp
)
file(GLOB_RECURSE HEADER_VKH
    "${DIR_VKH}/*.hpp
)
file(GLOB_RECURSE SOURCE_VKH
    "${DIR_VKH}/*.cpp
)
file(GLOB_RECURSE HEADER_OTH
    "${DIR_OTH}/*.hpp
)
file(GLOB_RECURSE SOURCE_OTH
    "${DIR_OTH}/*.cpp
)
source_group("WindowInput Headers" FILES ${HEADER_WI})
source_group("WindowInput Sources" FILES ${SOURCE_WI})
source_group("GarFish Headers" FILES ${HEADER_GF})
source_group("GarFish Sources" FILES ${SOURCE_GF})
source_group("GarFishUser Headers" FILES ${HEADER_GFU})
source_group("GarFishUser Sources" FILES ${SOURCE_GFU})
source_group("VulkanLow Headers" FILES ${HEADER_VKL})
source_group("VulkanLow Sources" FILES ${SOURCE_VKL})
source_group("VulkanHigh Headers" FILES ${HEADER_VKH})
source_group("VulkanHigh Sources" FILES ${SOURCE_VKH})
source_group("Other Headers" FILES ${HEADER_OTH})
source_group("WInput Sources" FILES ${SOURCE_OTH})

#target_include_directories(garfish PUBLIC 
    "${DIR_WI}"
    "${DIR_GF}"
    "${DIR_GFU}"
    "${DIR_VKL}"
    "${DIR_VKH}"
    "${DIR_OTH}"
)


set(HEADER_DIR ${PROJECT_SOURCE_DIR}/inc)
set(HEADERS 
${HEADER_DIR}/engine.hpp
${HEADER_DIR}/engine_types.hpp
${HEADER_DIR}/gf_vulkan.hpp
${HEADER_DIR}/gf_options.hpp
${HEADER_DIR}/gf_err.hpp
${HEADER_DIR}/gf_util.hpp
${HEADER_DIR}/gf_types.hpp
${HEADER_DIR}/gf_resource_manager.hpp
${HEADER_DIR}/gf_text.hpp
)

set(SOURCES
main.cpp
engine_main.cpp
gf_vulkan.cpp
gf_options.cpp
gf_err.cpp
gf_util.cpp
gf_types.cpp
gf_resource_manager.cpp
gf_text.cpp
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

target_link_libraries(garfish PUBLIC Vulkan::Vulkan glfw vma vkbootstrap imgui glm stb_image fastgltf::fastgltf ft_lib)