# TODOS

While I was able to refactor some of the tutorial code in the beginning and still follow along, the complexity of 
the engine has grown to a point where it would be best to refactor things after I have gotten to a suitable point.
This document serves as a reminder for things I would like to do once I get to that point.

## GFOptions structure
- Need two types of options: engine options & end user options
- Seperate or together?

## General
In no particular order:
- Put namespaces around vk files to categorize them
    - vk_types
	- vk_images
	- vk_descriptors
- gl_window needs to have its copying capabilities disabled.
- Actually check for support of things that we need
- Create an aggregate for pipelines, shaders, and descriptors. RAII
- Create a series of deletion queue aggregates which take arrays of objects and delete them.
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
- Textures!