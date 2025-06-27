# June Report

I have been adding doxygen support to the engine, and I'm noticing some organizational issues that 
I'd want to address before moving forward, and some that can be addressed later.

## Before Moving Forward

Below is a list of things that I'm unahppy with and should be addressed before I move on to adding text to the engine.

- vk_images should be renamed
- The renderer class in vk_frames should be completed and the reliance on VKCore for the swapchain, frame data, and 
immediate frames should be moved to the renderer class.
  - We should consider the scope of the renderer class, what is it doing besides holding our frame data and swapchain?
- The relationship between the ImageBufferAllocator and its allocations should be reworked, see the Later section
- namespacing (see doxygen)
- Remove last modified and date created
- Convert IBaseMaterial into RAII with a protected parameterized base class.
- Move @details below @param and @returns
- More file categories (lower vulkan & higher vulkan)
  - lower vulkan implies very low level vulkan, defining our graphics API
  - higher vulkan would imply higher level library functions, using our graphics API with vulkan.
- A better split between vk_ and gf_ files, specifically:
  - gf_types
  - gf_vulkan
  - gf_resource_manager
  - vk_materials
  - vk_renderables
- Rework int "types" files and what's importing what.
  - vk_types should be reworked.
  - The types in vk_renderable may need to be adjusted?
  - More clarity on engine_types, gf_types, vk_types, and gf_util
- gf_err and gf_options should be put on hold and potentially removed for the time being.

## Later

Text rendering should come after documentation.
Shortly after text rendering, I should redo the camera implementation.
After that, I'd like to take a look at making realistic skies with gradients.

### WInput:
The current use of friends and context pointers is a little annoying. I need to find a way to have a
relationship similar to parent-child classes, but with composition instead. I want the "manager" to have
access to private member variables that outsiders shouldn't have access to.

I would also like to rename WInput and the gl_* prefix