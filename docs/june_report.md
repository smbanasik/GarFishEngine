# June Report {#junereport}

I have been adding doxygen support to the engine, and I'm noticing some organizational issues that 
I'd want to address before moving forward, and some that can be addressed later.

## Before Moving Forward

Below is a list of things that I'm unahppy with and should be addressed before I move on to adding text to the engine.

**Addendum:** The core issues here come from a general file structure and scope of each file. Before we  address the issues below, let's solve this.  

We need to:
- [x] Reconsider our file prefixes and groupings, potentially swapping to a folder structure instead of a flat one.
- [ ] Reconsider the scope of some files and classes within the files.
- [ ] Adjust the type files to match more closely what they cover.

**Addendum Solution:**
- [ ] Remove gf namespace from most things
- [x] `com_engine_types` into it's own classes
- [x] `com_util` into it's own classes
- [x] Remove all `#includes` of `gf_err` for now
- [ ] Split `gf_resource_manager` into it's own classes, remove reliance on GFU types
- [ ] Avoid touching `gf_text` and `gf_vulkan` for now (these will be taken care of later)
- [ ] Take textureatlas and create ImageAtlas, move to own vkh class
- [ ] Split rename `vkh_loader` into `vkh_gltf`
- [ ] Move `load_image_from_path` from `vkh_gltf` to another area
- [ ] Split `vkh_materials` into interface and child classes
- [ ] Move the DrawContext to wherever RenderObject is defined.
- [ ] Move the bundles into RenderableTypes
- [ ] Create some sort of render primatives file and put Quad into it
- [ ] Split VkCore and Alloc into their own files
- [ ] Split `vkl_descriptors`
- [ ] Split `vkl_frames`
- [ ] Split `vkl_images`
- [ ] `vkl_initializers` is fine since declarations are free to `#include`
- [ ] `vkl_pipelines` is fine
- [ ] Split `vkl_types` into render types, compute types, and the rest can remain in `vkl_types`

### The list itself

- vk_images should be renamed
- The renderer class in vk_frames should be completed and the reliance on VKCore for the swapchain, frame data, and immediate frames should be moved to the renderer class.
  - We should consider the scope of the renderer class, what is it doing besides holding our frame data and swapchain?
- The relationship between the ImageBufferAllocator and its allocations should be reworked, see the Later section
  - We should let the allocator handle deallocation and allow for the allocator to clear them early.
- namespacing (see doxygen)
- Remove last modified and date created
- params should have \[in], \[out], and \[in,out]
- Convert IBaseMaterial into RAII with a protected parameterized base class.
- Move \@details below \@param and \@returns
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
- There is an anti-pattern appearing where a class will have a VkDevice pointer as a private member variable since it's often needed for the destructor. This will result in many references all to the same address. Is there a way we can minimize this?
  - One option is static members, since we'll *rarely* need more than one VkDevice or VkCore.
  - Resource managers could be used to do this, but we run into a similar issue.

## Later

Text rendering should come after documentation.
Shortly after text rendering, I should redo the camera implementation.
After that, I'd like to take a look at making realistic skies with gradients.

Additionally, creating different options for uniform buffers and storage buffers could be useful.
Same goes for graphics and compute pipelines.

### WInput:
The current use of friends and context pointers is a little annoying. I need to find a way to have a
relationship similar to parent-child classes, but with composition instead. I want the "manager" to have
access to private member variables that outsiders shouldn't have access to.

I would also like to rename WInput and the gl_* prefix