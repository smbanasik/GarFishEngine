# GarFish Engine

`git clone --recurse-submodules <garfish-url>`
`git submodule status` to see version dependencies

## Description

This engine serves as a general purpose base for future projects, built from C++ and Vulkan.

## Scope

The GarFish engine promises at least the following features:
- A performant interface of Vulkan's features
- A performant interface for text rendering
- An abstraction of graphical data such as models, materials, and so on
- A system for storing elements on the screen.
- General UI elements such as buttons, draggable screens, etc.

## Setup

Required:
- Vulkan
- GLFW
- VMA
- VKbootstrap
- ImGui
- GLM
- fastgltf
- STB Image
- FreeType
- Tracy

Optional for Doxygen:
- Graphviz

### Vulkan
- Head to the [Vulkan website](https://vulkan.lunarg.com/sdk/home) and download. We're using Vulkan 1.3.296.0, run the installer.
- Only the defaults are needed.
- Installer should handle everything else!

### GLFW
- Included as a submodule

### VMA
- Head to the [VMA github](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator/tree/master), then to the include folder, and download `vk_mem_alloc.h`.
- Create a folder called `vma` in the `third_party` folder.
- Place your `vk_mem_alloc.h` into the `vma` folder

### VKBootstrap
- Included as a submodule

### ImGui
- Head to the [ImGui github](https://github.com/ocornut/imgui), and download all of the .h and .cpp files on the main folder
- Go to the backends folder and download:
    - imgui_impl_glfw (.h/.cpp)
	- imgui_impl_vulkan (.h/.cpp)
- Create a folder called `imgui` in the `third_party` folder.
- Place all of the files into the `imgui` folder.

### GLM
- Included as a submodule

### STBImage
- Grab `stb_image.h` from the [stb github](https://github.com/nothings/stb/blob/master/stb_image.h) and download it.
- Create a folder called `stb_image` in the `third_party` folder.
- Place `stb_image.h` into the `stb_image` folder.

### FastGLTF
- Included as a submodule

### FreeType
- Included as a submodule

### Graphviz
- Head to the [graphviz download](https://www.graphviz.org/download/) link.
- Ensure the dot tool is in the path.
- Doxygen will now generate useful diagrams. Learn more [here](https://www.doxygen.nl/manual/diagrams.html).

### Tracy
- Tracy is added as a submodule.
- TODO: instructions

### Assets
The `assets` folder is not included in the repository. Engine code that relies on these is likely there 
for testing purposes and could be safely removed.

## Docs folder
Use the docs folder for more information

## Future Plans
- Base Engine
    - Low level interface (given items to display on screen, display them)
	- Text interface
	- Basic UI elements (buttons, boxes, text boxes)
	- Asset loading system (models, textures, images, etc.)
	- Shader system
	- Higher level interface (scene system, choose what goes on screen)
	- Multithreading
- 2D
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
- 3D
    - Material system
	- Physics system
	- Real time gameplay system
	- Advanced lighting system
    - Level of detail and culling
	- Skeletal animations?
	- Multiplayer/network functionality
