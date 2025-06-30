/**
* @file
* @author Spencer Banasik
*/
#ifndef VKL_SWAPCHAIN_HPP
#define VKL_SWAPCHAIN_HPP
#include <stdint.h>
#include <vector>
#include <utility>

#include<vulkan/vulkan.h>

namespace vkl_core {
class VKCore;
}

namespace vkl_frames {
/**
* @class SwapChain
* @brief A RAII abstraction of Vulkan's swapchain, along with the data needed to use it.
* @author Spencer Banasik
* @details The SwapChain is what is Vulkan uses to present images. Each image in the
* swapchain_images contains the actual data that the GPU presents to the screen, and the
* swapchain_image_views provide associated data with it.
*/
class SwapChain {
public:

    SwapChain(vkl_core::VKCore* core, uint32_t width, uint32_t height);
    ~SwapChain();
    SwapChain(SwapChain&& other) noexcept;
    SwapChain& operator=(SwapChain&& other) noexcept {
        swapchain = std::move(other.swapchain);
        swapchain_format = std::move(other.swapchain_format);
        swapchain_images = std::move(other.swapchain_images);
        swapchain_image_views = std::move(other.swapchain_image_views);
        core_handle = std::move(other.core_handle);
        other.swapchain = nullptr;
        for (auto it = other.swapchain_image_views.begin(); it != swapchain_image_views.end(); it++) {
            *it = nullptr;
        }
    }
    SwapChain(SwapChain& other) = delete;
    SwapChain& operator=(SwapChain& other) = delete;

    /**
    * @brief Remakes the swapchain according to the specified dimensions.
    * @author Spencer Banasik
    * @details The swapchain and all of its supplemental member variables are remade.
    * This must be called whenever the window is resized.
    * @param [in] width The width of the swapchain.
    * @param [in] height The height of the swapchain.
    */
    void remake_swapchain(uint32_t width, uint32_t height);
    void destroy_swapchain();

    /**
    * @brief Swapchain handle.
    * @details A structure with some images we can draw to and then display on screen.
    * Unique to different platforms, hence the KHR suffix. Must be recreated if
    * a window is resized. It holds a list of images and image-views, accessabile with
    * swapchain_images and swapchain_image_vies
    */
    VkSwapchainKHR swapchain{};

    /**
    * @brief Enumerator describing the data format of the swapchain.
    */
    VkFormat swapchain_format{};

    /**
    * @brief Dimensions of the swapchain.
    */
    VkExtent2D swapchain_extent{};

    /**
    * @brief Handles to dimensional arrays of image data.
    */
    std::vector<VkImage> swapchain_images;

    /**
     * @brief Handles to image metadata
     * @details Since images are not directly accessible, the image view provides
     * meta data such as how to access an image, which part to access, and so on.
     */
    std::vector<VkImageView> swapchain_image_views;
private:
    vkl_core::VKCore* core_handle;
};
}
#endif
