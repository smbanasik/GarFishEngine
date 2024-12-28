# Documentation

## Vulkan Structure

Vulkan provides a lot of power at the expense of being incredibly verbose and low level. You are not creating a game engine
without creating your own graphics api first. Below is my attempt to roughly explain the important structures of vulkan and 
how the engine will be utilizing them. This is not the higher level abstractions, which will be covered in "Engine Abstractions," 
but instead the low level Vulkan structures.

### Initializations
- Instace
- Physical Device
- Device
- Surface
- Queue

### Synchronization
- Semaphore
- Fence

### Swapchains and Images
- Swapchain
- Framedata
- AllocatedImage

### Shaders and Pipelines
- VkShaderModel
- Pipeline
- Descriptor Sets


## Engine Abstractions