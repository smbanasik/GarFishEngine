// Spencer Banasik
// Created: 1/06/2025
// Last Modified: 1/06/2025
// Description:
// Primary structure for errors
#ifndef GF_ERR_HPP
#define GF_ERR_HPP

#include <functional>

// Do not define this in .hpp files!
#ifdef GF_VK_CHECK_TRUE
#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>
#define VK_CHECK(x)                                \
    do {                                           \
        VkResult err = x;                          \
        if (err) {                                 \
            gf::err::vk::VulkanError eng_err;      \
            eng_err.display(string_VkResult(err)); \
            abort();                               \
        }                                          \
    } while (0)
#else
#define VK_CHECK(x) x
#endif

namespace gf {
namespace err {
struct EngineError {
    virtual void display() = 0;
    virtual const char* what() = 0;
    //virtual void log() = 0;
};
namespace vk {
struct VulkanError : public EngineError {
    virtual void display() override;
    virtual const char* what() override;
    virtual void display(const char* string_vkresult);
};
}

namespace gl {
struct GLFWError : public EngineError {
    virtual void display() override;
    virtual const char* what() override;
};
struct GLFWInitErr : public GLFWError {
    virtual const char* what() override;
};
}
}
}
#endif
