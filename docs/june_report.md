# June Report {#junereport}

I have been adding doxygen support to the engine, and I'm noticing some organizational issues that  I'd want to address before moving forward, and some that can be addressed later.

## Next steps

Text rendering - See text writeup.
Multithreading.
Shortly after text rendering, I should redo the camera implementation. After that, I'd like to take a look at making realistic skies with gradients.

### Future Considerations

- The relationship between the ImageBufferAllocator and its allocations should be reworked, see the Later section
  - We should let the allocator handle deallocation and allow for the allocator to clear them early.
- documentation params should have \[in], \[out], and \[in,out]
- documentation should have the details section last.
- Convert IBaseMaterial into RAII with a protected parameterized base class.
- Go through the gf namespace and make any changes as needed
- There is an anti-pattern appearing where a class will have a VkDevice pointer as a private member variable since it's often needed for the destructor. This will result in many references all to the same address. Is there a way we can minimize this?
  - One option is static members, since we'll *rarely* (if ever) need more than one VkDevice or VkCore.
  - Resource managers could be used to do this, but we run into a similar issue.
- An interface that combines Imm_frame, the swapchain, and the frame_data to simplify submission.
  - Renderer class?
- See WInput section
- Create options for uniform buffers and compute pipelines.

### WInput:

The current use of friends and context pointers is a little annoying. I need to find a way to have a relationship similar to parent-child classes, but with composition instead. I want the "manager" to have access to private member variables that outsiders shouldn't have access to.
