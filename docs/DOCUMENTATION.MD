# Documentation

## Vulkan Structure

Vulkan provides a lot of power at the expense of being incredibly verbose and low level. You are not creating a game engine
without creating your own graphics api first. Below is my attempt to roughly explain the important structures of vulkan and 
how the engine will be utilizing them. This is not the higher level abstractions, which will be covered in "Engine Abstractions," 
but instead the low level Vulkan structures.

### Initializations
The structures below are needed to do virtually anything in vulkan. These are used to setup other structures.

- Instance
    - This is the entry point to Vulkan which contains the context for our engine. Validation layers, surface creation. Must be destroyed.
- Surface
    - What vulkan uses to put objects on the window. Not used directly, given to the device. Must be destroyed.
- Physical Device
    - A representation of the physical GPU on the device. Queried for features.
- Device
    - Abstraction of the device, used to interface with the GPU. Must be destroyed.

### General Concepts
Oftentimes a structure will have an info structure associated with it. Info structures act as a way of capturing all of the information needed 
for a given object. As an example, a pipeline layout will be created with pipeline layout info, which describes how the layout should be. This 
layout in turn describes how the actual object should be.

### Swapchains and Images
- Swapchain
    - The swapchain is a structure that stores "images" that can be drawn to and presented. Holds a list of images and image-views.
	- Must be recreated on resize.
- Image
    - A handle to an image object
- Image-view
    - A view to the image handle, describing how to access the image and which part to access.
- Swapchain (Structure)
    - Holds the swapchain, its format and size, and stores the images and image views for the swapchain.
- AllocatedImage
    - An image and its information, along with its allocation
- Drawn Image
    - An allocated image that we draw to, then copy over to the swap chain image we wish to submit.

### Commands
The structures here are needed to actually submit commands to the GPU. Multithreading is possible with 1 pool and 1 buffer per thread, but
only one submission is possible at a time.

- Queue
    - Commands are submitted to this queue, multiple types of queues.
- Command Pool
    - A command pool that allocates commnad buffers
- Command Buffer
    - A buffer that you can place GPU commands into, for queue submission. Can be recorded once and submitted multiple times.
	- Queue submission is often the expensive part, recording commands is cheap.
- FrameData
    - A personal abstraction made to bundle a command pool, command buffer, and synchronization structures together.
- ActiveFrames
    - The member variable that contains all of the command and synchronization structures for every swapchain image

### Synchronization
- Semaphore
    - Use to order operations on the GPU. Used to define an order of operations on the GPU.
	- Signal and Wait semaphores, an operation signals a semaphore, another waits on it.
	    - Think of it like a linked list, where the semaphores are the connections.
- Fence
    - Use when the CPU must wait for the GPU

### Shaders and Pipelines
- ShaderModule
    - A processed shader file read in from a file. Consumed by pipeline
- Descriptor Set Layout
    - The layout for a descriptor set
- DescriptorPool
    - Similar to a command pool, allocates descriptors
	- Resetting a descriptor pool destroys all descriptor sets that it allocated
- DescriptorSet
    - Provides data to shaders
- DescriptorAllocator
    - A personal abstraction made to easily allocate and clear layouts
- PipelineLayout
    - A layout for a given pipeline, takes in a descriptor layout
- Pipeline
    - A structure describing a full graphics pipeline
- Push constants
    - Quick and small data that can be sent to a shader

## Engine Abstractions

### Materials
- GLTF Materials
    - Material Pipelines (pipeline + layout)
        - Shaders
    - Material constants for shaders
    - Descriptor set layout
    - Material resources
        - Images and buffers for material

### Renderables

- RenderObject
    - A flattened structure containing everything vulkan needs to render something