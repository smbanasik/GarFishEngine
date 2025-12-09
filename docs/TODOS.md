# TODOS

While I was able to refactor some of the tutorial code in the beginning and still follow along, the complexity of 
the engine has grown to a point where it would be best to refactor things after I have gotten to a suitable point.
This document serves as a reminder for things I would like to do once I get to that point.

## GFOptions structure
- Need two types of options: engine options & end user options
- Seperate or together?

## Next Steps
- Text boxes just produce the data necessary to render, rendering is done by a text node or other.
    - Next step is to assemble text data using Quad
- Need to add precompiled header file!
- Redo header files
    - Need types in their own files, separate common include types vs implementation
    - Merge inc and src together into a single code folder (refer to doom 3 code)
    - Structure files together based on purpose
        - GL files go together
        - Vulkan files go somewhat together
        - Higher level files go together
        - Type files for frequent included items
- Create a TileMap class
    - Owns a grid of sprites with subdivisions
    - How do we handle multiple texture atlasses?
- Change shaders to calculate viewproj outside of shader (and exclude static_elems from this)
    - When do shaders run and with what? Draw command?
    - Many shaders? Or state in shaders?

## General
- Get a textured 2d image on screen
- Implement text rendering on the screen
    - Render atlas is needed
    - Should be able to reuse code from rogue eng
- Implement some sort of sprite system
    - Textures, shaders, etc.
- UI elements
    - Button manager & buttons
In no particular order:
- Actually check for support of things that we need
- Create an aggregate for pipelines, shaders, and descriptors. RAII
- Create a series of deletion queue aggregates which take arrays of objects and delete them.
- Draw image should use window size first
- Expand pipeline builder functionality
- Remove compute effects and compute shader items
- Move upload mesh function out of gf_vulkan
- Move immediate submit and immediate member variables to its own data structure.
- Review VkGuide for improvements with some patterns like mesh buffers
- Review the load mesh function at a deeper level
- Move image creation to its own area
- Create files to handle the engine architecture (render obj, node, meshnode, etc)
- Sort the render context! Sort by pipeline to reduce bindings
- Create or #include a logging system
- Look into sorting draws before rendering, need an efficient way
- Give RAII to buffers and images
    - This is hard to do right now! Things fall out of scope before they should sometimes, other times things aren't being copied correctly. For now we'll manage memory manually and give this another shot. Refer to this commit to find my results: 236c572
    - Come back to this later when we have a better understanding of things.
    - Fork the repo, delete everything unnecessary, remove library dependencies, slim it all down. Then experiment.
