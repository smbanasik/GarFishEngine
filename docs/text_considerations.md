# Text Rendering {#textrendering}

## Text Rendering Requirements

We should have the capabilities of a modern word processor. That means per letter customization, fonts, scaling, and so on. Since most text in a game is not dynamic, we can hold off on efficient text editing and just create the text asset for now. We'll come up with another system that uses a dynamic buffer for situations that would require per-letter increases to text rendering.

## Text Rendering Solution

Like the graphics pipeline, text rendering can be split into three categories: graphics level, engine level, and user level. At a user level, we provide a font and text, then the text is outputted to the screen. We could do this with a text node, or have some sort of text attachment to an object. At a engine level, we take the font and create a texture and we handle how characters should be spaced out together. Items like word wrapping, text coloring, or other more detailed options should be handled here. At a graphics level, we take a series of boxes from the words and turn them into a single render object. 

### Implementation Details

Let's start with what a render object requires:
- A vertex buffer
- A vertex address
- An index buffer (with a start idx and count)
- A MaterialInstance
- A transform matrix
- Bounds for view culling

#### **The Graphics Level**

Here we create a class that's capable of holding this information. We'll call it TextAsset, since, like the MeshAsset, it's primary purpose is to be easily converted to a RenderObject. A TextAsset will hold our GPUMeshBuffers, a name, and an array of Words. Words are the GeoSurface equivalent of this system. 

Like a GeoSurface, a Word contains a start_idx, a count, and a material equivalent called a TextMat (bounds checking will be considered later). Importantly, the start_idx and count are for letters, not vertices, since we can compute those. **EDIT**: This doesn't work given the below specifications? Need to rethink.

A TextMat contains a material instance, a Font ptr, and text configuration for our Word. Conceptually, a word is not a word, but instead a set of characters in a TextAsset with the same formatting. This should be implemented in such a way that *if I have a sentence like this one*, there should only be two Words: one for the italicized text, and one for the normal text. The goal is to minimize draw calls, which have to occur once per MaterialInstance, while still providing a way to customize the text per character.

**TODO**: How are we managing TextAssets and Words? How are they initialized?

**TODO**: TextMat currently only contains a Font pointer.

Finally, a Font is texture data and character spacing all wrapped into one. These are allocated and handled with a FontManager(?).

**TODO**: Let's separate fonts from actual word rendering. vkh_font or something. Afterall, fonts are just texture data and spacing data.

>*A Quick Note*
>We need to consider editing text. Can we just change the characters that we need to? Need to look into how buffers are populated.

**The Font Manager**

There should be relatively few fonts in a game compared to other assets, so like materials, we'll create a resource manager for them called the FontManager. This will facilitate the creation of Fonts, their material instances, and so on.

**Static Functions for Bundles**

You'll notice that the TextAsset doesn't actually contain any text. The low level classes are only concerned with converting already configured Font and Text data into a render object. There are a few helper functions provided, the first taking an array of *some object* and populating the buffers of a TextAsset.

Finally, we'll need a second function to create words from our input text, which will also require the buffers to be formatted accordingly (since words should be consecutive but don't have to be visually). 

#### **The Engine Level**

With our low level interface decided, we can move onto how we're actually going to manage our text. Our high level class will be called the TextBox. The TextBoxes concern is to establish the spacing and boundaries of the text while also storing the resources to construct a TextAsset.

Our TextAsset will require an array of Words and populated GPUMeshBuffers. Let's consider their creation requirements before we figure out how to implement them.

**Word Creation**

Words are a set of characters with the same material. Vulkan will only take a start index and a count, so the Word in data must be contiguous. When populating our buffers, we should consider the Words first and then assemble them with that in mind. Since words also contain a TextMat, we'll need to take care of those as well, and adjust vertices as needed in line with the TextMat.

**Buffer Population**

For now, we'll use a staging buffer to send our buffers to the GPU, so we can just use the UploadMesh function. This requires a set of vertices and indices already made.

With these considerations, when we want to assemble our text into vert data, we'll organize the text by Word, create our vectors of indices and vertices, and then run through the text, constructing a Quad and placing it into our respective buffers. Since we aren't concerned with editing text in this system, we can discard the text data and arguably the text buffer, to the point of never needing to store it in the first place.

Any other functionality will come on top of this.
