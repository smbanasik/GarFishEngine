# GarFish Engine

## Description

This engine serves as a general purpose base for future projects. Primarily powered by Vulkan and GLFW. Check the attributions for more info.

## Scope

The GarFish engine promises at least the following features:
- A performant interface of Vulkan's features
- A performant interface for text rendering
- An abstraction of graphical data such as models, materials, and so on
- A system for storing elements on the screen.
- General UI elements such as buttons, draggable screens, etc.
- See more below.

## Setup

Required:
- GLFW
- Vulkan
- GLM
- STB Image
- VMA
- VKbootstrap

### Vulkan
- Head to the [Vulkan website](https://vulkan.lunarg.com/sdk/home) and download. We're using Vulkan 1.3.296.0, run the installer.
- Only the defaults are needed.
- Installer should handle everything else!

### GLFW
- Head to the [GLFW website](https://www.glfw.org/) and download. We're using GLFW 3.4
- Extract into the third_party folder
- That's it!

### VMA
- Head to the [VMA github](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator/tree/master), then to the include folder, and download `vk_mem_alloc.h`.
- Create a folder called `vma` in the `third_party` folder.
- Place your `vk_mem_alloc.h` into the `vma` folder

### VKBootstrap
- Head to the [VKBootstrap github](https://github.com/charles-lunarg/vk-bootstrap), then src folder, and download all of the files.
- Create a folder called `vkbootstrap` in the `third_party` folder.
- Place the files into the `vkbootstrap` folder.

### GLM
- Head to the [glm github](https://github.com/g-truc/glm/tree/master/glm) and download the glm directory.
- Place the folder into `third_party`

### STB Image
- Head to the stb github, download
- Place into `stb_image` folder

### Assets
The `assets` folder is not included in the repository. Engine code that relies on these is likely there 
for testing purposes and could be safely removed.

## Code Guidelines
- RAII
- Only include as needed (forward declaring)
- Minimize side effects
- Abstraction!
- Reduce code duplication

## Code Structure

### Names
- `vk_*` files are low level files that directly act with vulkan. They are not abstracted and serve as the low level abstractions with vulkan.
- `gf_*` files are meant to serve more as interface files for a user, to make utilizing vulkan or other libraries easier.
- `engine_*` files build upon the interface files, providing engine functionality.

## Future Plans
- Base Engine
    - Low level interface (given items to display on screen, display them)
	- Text interface
	- Basic UI elements (buttons, boxes, text boxes)
	- Asset loading system (models, textures, images, etc.)
	- Shader system
	- Higher level interface (scene system, choose what goes on screen)
	- Multithreading
- 2D Roguelike backport
    - Sprite system
	- Basic lighting
	- Tile system
	- Turn based gameplay system
	- Sound
	- Particles
	- Basic 2D Animations
	- LUA scripting
	- ECS pattern
	- Additional abstract patterns
	- Advanced UI elements (draggable screens)
- 3D Voxel game
    - Material system
	- Physics system
	- Real time gameplay system
	- Advanced lighting system
    - Level of detail and culling
	- Skeletal animations?
	- Multiplayer/network functionality
- 2D Terraria Clone(?)
- RTS/RPG Hybrid