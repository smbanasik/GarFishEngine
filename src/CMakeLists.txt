set(DIR_WI ${CMAKE_CURRENT_SOURCE_DIR}/window_input)
set(DIR_GF ${CMAKE_CURRENT_SOURCE_DIR}/garfish)
set(DIR_GFU ${CMAKE_CURRENT_SOURCE_DIR}/garfish_user)
set(DIR_VKL ${CMAKE_CURRENT_SOURCE_DIR}/vulkan_low)
set(DIR_VKH ${CMAKE_CURRENT_SOURCE_DIR}/vulkan_high)
set(DIR_OTH ${CMAKE_CURRENT_SOURCE_DIR}/common)

file(GLOB_RECURSE HEADER_WI
    "${DIR_WI}/*.hpp"
)
file(GLOB_RECURSE SOURCE_WI
    "${DIR_WI}/*.cpp"
)
file(GLOB_RECURSE HEADER_GF
    "${DIR_GF}/*.hpp"
)
file(GLOB_RECURSE SOURCE_GF
    "${DIR_GF}/*.cpp"
)
file(GLOB_RECURSE HEADER_GFU
    "${DIR_GFU}/*.hpp"
)
file(GLOB_RECURSE SOURCE_GFU
    "${DIR_GFU}/*.cpp"
)
file(GLOB_RECURSE HEADER_VKL
    "${DIR_VKL}/*.hpp"
)
file(GLOB_RECURSE SOURCE_VKL
    "${DIR_VKL}/*.cpp"
)
file(GLOB_RECURSE HEADER_VKH
    "${DIR_VKH}/*.hpp"
)
file(GLOB_RECURSE SOURCE_VKH
    "${DIR_VKH}/*.cpp"
)
file(GLOB_RECURSE HEADER_OTH
    "${DIR_OTH}/*.hpp"
)
file(GLOB_RECURSE SOURCE_OTH
    "${DIR_OTH}/*.cpp"
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
source_group("Common Headers" FILES ${HEADER_OTH})
source_group("Common Sources" FILES ${SOURCE_OTH})

set(SOURCES 
    ${SOURCE_WI}
    ${SOURCE_GF}
    ${SOURCE_GFU}
    ${SOURCE_VKL}
    ${SOURCE_VKH}
    ${SOURCE_OTH}
)

set(HEADERS 
    ${HEADER_WI}
    ${HEADER_GF}
    ${HEADER_GFU}
    ${HEADER_VKL}
    ${HEADER_VKH}
    ${HEADER_OTH}
)

add_executable(garfish
    ${SOURCES}
    ${HEADERS}
)

set_target_properties(garfish PROPERTIES 
LINKER_LANGUAGE CXX
CXX_STANDARD 20
)

set(INC_DIRS
    ${DIR_WI}
    ${DIR_GF}
    ${DIR_GFU}
    ${DIR_VKL}
    ${DIR_VKH}
    ${DIR_OTH}
)

target_include_directories(garfish PUBLIC 
    ${INC_DIRS}
)

target_link_libraries(garfish PUBLIC Vulkan::Vulkan glfw vma vkbootstrap imgui glm stb_image fastgltf::fastgltf ft_lib)