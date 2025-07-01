# The Graphics Pipeline, From Output to Input #{graphicspipeline}

There are many moving parts in the GarFish engine, and extending upon it can be difficult without  
understanding the process from the beginning to the end. The process can be split into two halves:  
the GPU ready side and the abstracted side. Since everything on the GPU ready side starts with the  
shaders, we'll focus there and work our way up to the load process.  

## Starting with Shaders

Shaders receive external data (that is, data that isn't from a previous shader) in two ways: descriptor  
sets and push constants. Descriptor sets are the default way of passing information to shaders and are  
used to pass buffers, images, and so on. Push constants are small pieces of data stored in the  
command buffer itself and occur once per draw call (such as vkCmdDrawIndexed).  

Our shaders make use of the device buffer address extension to pass the buffer address as a push  
constant instead of passing the buffer through a descriptor set. In terms of the data flow that we'll  
recieve texture data and other data from the descriptor set and the raw vertex data from the push constants.  

## Considering Commands

Now that we understand what data needs to be passed to the shaders, we need to discuss the commands used  
to send all of this, since this determines what we'll need for our RenderObject class. The RenderObject  
will be a flat object containing all of the information needed to render an object, stored in the  
DrawContext, and created when an IRenderable calls it's draw function.  

According to our shaders, we'll need the vertex buffer address (VBA), the object's world matrix, and  
the its descriptors. All drawing requires a pipeline, so we'll need to include one as well. Finally,  
to reduce the amount of vertices needed, we'll use the DrawIndexed command, meaning we'll need a  
buffer for indices.  

This results in a render object looking like this:
~~~~~{.c++}
struct RenderObject {
    uint32_t index_count;
    uint32_t first_index;
    VkBuffer index_buffer;

    MaterialInstance* material;
    Bounds bounds;
    glm::mat4 transform;
    VkDeviceAddress vertex_buffer_address;
};
~~~~~

By including the index_count and first_index variables, we can reduce the amount of times we bind  
index buffer by allowing for multiple objects to use the same index and vertex buffer. One potential  
improvement to explore would be batching surfaces with the same materials, index buffers, and vertex  
buffers into one draw call, though this may fall onto the model design instead.  

### MaterialInstance

The descriptors and the pipeline form a MaterialInstance. A material will define the texture of an  
object, how it reacts to light, and what graphical effects it may have.  

## The Half Way Mark

After the RenderObject, we move into the input side of the pipeline. Every input format should have some  
way of transforming into our RenderObject when we call it's draw function in update_scene. Right now we  
just have our Node and Scene system, which contain MeshAssets. A MeshAsset is a high level representation  
of an object, containing a name, surfaces, and vertex data.  

### Vertex Data
The simpler of the two is the vertex data, stored in the GPUMeshBuffers class. This contains the vertex  
buffer, index buffer, and the VBA.  

### Surfaces
The surfaces, could be considered submeshes of our MeshAsset. If a MeshAsset is a character, a surface is  
the characters skin, it's clothes, and so on. The combination of a surface and the MeshAsset's buffers  
make up a RenderObject. A surface contains the start index, count, and a material.  

## Scene Management
The next step above the MeshAsset is a scene. A scene is a manager for a series of MeshAssets. It  
contains the meshes, nodes, images, materials, samplers, and anything else needed to handle its  
resources. It's the top layer of our assets. Everything should be grouped together into a scene.  

## Loading Scenes
So how is a scene loaded? How do we create those resources?  

### Vertices and Buffers
Once a collection of vertices and indices are put together, upload_mesh is called to produce a  
GPUMeshBuffers for a MeshAsset.

### MaterialInstances
Materials are constructed with the write_material function, using the scenes descriptor pool, material  
resources that are constructed, and the pass type. Samplers and images are loaded in with their  
respective functions.