# The Garfish Engine {#mainpage}

If you're reading this, thank you for your interest! The GarFish Engine is an engine made from scratch  
using C++, Vulkan, GLFW, and some other libraries. It's goal is to be a multipurpose engine capable of  
making 2D and 3D games of any genre with the toolset to empower artists and programmers.  

## Architecture Writeups

- [The Graphics Pipeline](#graphicspipeline)
- [Text Rendering](#textrendering)

## Learning Resources

Here are some references that I think are useful. Vulkan Guide and Vulkan Tutorail are great beginner  
resources, with the Vulkan Spec providing a deeper understanding of the concepts discussed.  

Eventually I'd like to include some topics about multithreading and synchronization, once the engine  
is ready.

### Vulkan Specific Resources

- [Vulkan Guide](https://vkguide.dev/) - Tutorial focused on utilizing Vulkan in a game engine
- [Vulkan Tutorial](https://vulkan-tutorial.com/) - More comprehensive but not focused on game engine architecture
- [Vulkan Spec](https://docs.vulkan.org/spec/latest/index.html) - Primary Vulkan reference
- [How to Learn Vulkan](https://www.jeremyong.com/c++/vulkan/graphics/rendering/2018/03/26/how-to-learn-vulkan/) - Older but useful resource
- [Intel's Tutorial](https://www.intel.com/content/www/us/en/developer/articles/training/api-without-secrets-introduction-to-vulkan-part-1.html)
- [Vulkan Buffer Access Benchmark](https://github.com/sebbbi/perftest)
- [VulkanHub](https://vkdoc.net/chapters/introduction) - An easier to read specification

### Showcases and Studies

- [Doom Eternal Study](https://simoncoenen.com/blog/programming/graphics/DoomEternalStudy) - A look at the ways Doom Eternal utilizes the GPU
- [Doom 2016 Study](https://www.adriancourreges.com/blog/2016/09/09/doom-2016-graphics-study/) - A look at how Doom 2016 handles rendering

## Terminology

Below is a list of terms that I may use in the documentation.  

Bundle - A struct with the primary purpose of keeping related data together in one class. Does not have behavior associated with it. Always uses struct keyword.
RAII Wrapper - A class with the primary purpose of providing Resource Acquisition Is Initiallization and the rule of 3/5/0 to a set of handles.


## File Structure and Scope

- Related functions/classes go into the same header file
  - Really, bundles should be the only things appearing in a types file, *if* the bundles are all in the same scope.
- Larger data structures go into their own header file (like std)
- Prefer smaller header files, keep things related.
- Every .cpp should have an associated .hpp