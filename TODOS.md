# TODOS

While I was able to refactor some of the tutorial code in the beginning and still follow along, the complexity of 
the engine has grown to a point where it would be best to refactor things after I have gotten to a suitable point.
This document serves as a reminder for things I would like to do once I get to that point.

## General
- Put namespaces around vk files to catogirze them
    - vk_types
	- vk_images
	- vk_descriptors
- Create a vk core class which contains the instance, device, gpu, and so on. RAII
- Create an aggregate for pipelines, shaders, and descriptors. RAII
- Create a full class for the swapchain. RAII
