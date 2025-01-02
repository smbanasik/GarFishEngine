# TODOS

While I was able to refactor some of the tutorial code in the beginning and still follow along, the complexity of 
the engine has grown to a point where it would be best to refactor things after I have gotten to a suitable point.
This document serves as a reminder for things I would like to do once I get to that point.

## GL system
- Structure of GL system:
    - WindowState - contains contexts for: window, mouse, keys, etc.
    - gl_manager - Main interface, initalizes and sets up the modules for a window
        - Owns the library
    - gl_window - RAII. Creates and owns a window. Holds information like monitor, dimensions, etc.
        - Functions to edit window properties as needed
    - gl_mouse - Creates and owns a mouse context, holds position info, velocity, direction vector.
        - Functions to ease this process
        - Has callbacks for a window
    - gl_joystick - ditto ^
    - gl_keys - Creates and owns a key context. Key context has callback, a map of key->pressed and key -> action.
        - Functions to ease this process
        - Also will handle char actions for text input
    - All callbacks should have a hookin system, store it context
- Flaws of current system:
    - WindowContext - would contain callbacks, manager doesn't have much purpose
    - Window function set is nice, but system would flop for mouse and keyboard.
         - Mouse and keyboard really need dedicated callbacks to make it easy on user,
         - and then hookins that the user could add.

## GFError Structure
- Have a set of error classes for different aspects of engine
    - GLFW, Vulkan, etc.
    - Child classes to those parent errors can be used for filtering.
    - Custom logic can be used, such as glfw's error descriptions.
    - Errors will have a .display() which outputs the error to the terminal.
    - a .what() will also be there which outputs human readable text for what happened (like std)
    - a .log() will log this to an error file. (if a logging system is implemented)
- Whenever something breaks, we throw one of the errors.

## GFOptions structure
- Need two types of options: engine options & end user options
- Seperate or together?

## General
In no particular order:
- Error checking in gl files
- Put namespaces around vk files to categorize them
    - vk_types
	- vk_images
	- vk_descriptors
- Actually check for support of things that we need
- Create a vk core class which contains the instance, device, gpu, and so on. RAII
- Create an aggregate for pipelines, shaders, and descriptors. RAII
- Create a series of deletion queue aggregates which take arrays of objects and delete them.
- Create a full class for the swapchain. RAII
- Separate the swapchain and draw/depth image creation logic
- Draw image should use window size first
- Expand pipeline builder functionality
- Remove compute effects and compute shader items
- Give RAII to buffers
- Move upload mesh function out of gf_vulkan
- Move immediate submit and immediate member variables to its own data structure.
- Review VkGuide for improvements with some patterns like mesh buffers
- Review the load mesh function at a deeper level
- Move image creation to its own area
- Create files to handle the engine architecture (render obj, node, meshnode, etc)
- Move structs GeoSurface and MeshAsset to a more sensible place
- Sort the render context! Sort by pipeline to reduce bindings
- Create or #include a logging system