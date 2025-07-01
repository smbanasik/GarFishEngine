# Text Rendering {#textrendering}

## Text Rendering Requirements
We should have the capabilities of a modern word processor. That means per letter customization, fonts, scaling, and so on.

## Text Rendering Solution
Like the graphics pipeline, text rendering can be split into three categories: low level, high level,  
and user level.  
At a user level, we provide a font and text, then the text is outputted to the screen. We could do this  
with a text node, or have some sort of text attachment to an object.  
At a high level, we take the font and create a texture and we handle how characters should be spaced  
out together. Items like word wrapping, text coloring, or other more detailed options should be handled here.  
At a low level, we take a series of boxes from the words and turn them into a single render object.  

### Implementation Details
Let's start with what a render object requires:
- A vertex buffer
- A vertex address
- An index buffer (with a start idx and count)
- A MaterialInstance
- A transform matrix
- Bounds for view culling

**Helper functions**
The lowest level can ultimately be represented by set of functions, the first taking in an array of  
*some object* and populating the vertex and index buffers of a GPUMeshBuffer bundle.  
The second function takes a font of some kind and produces a MaterialInstance.  

**The Low Level**
Here we create a class that's capable of holding this information. We'll call it TextAsset, since,  
like the MeshAsset, it's primary purpose is to be easily converted to a RenderObject.
A TextAsset will hold our GPUMeshBuffers, a name, and an array of Words. Words are the  
GeoSurface equivalent of this system.  

Like a GeoSurface, a Word contains a start_idx, a count, and a material equivalent called a TextMat  
(bounds checking will be considered later). Importantly, the start_idx and count are for letters, not  
vertices, since we can compute those.
A TextMat contains a material instance, a Font ptr, and text configuration for our Word. Conceptually,  
a word is not a word, but instead a set of characters in a TextAsset with the same formatting.  
This should be implemented in such a way that *if I have a sentence like this one*, there should only be  
two Words: one for the italicized text, and one for the normal text. The goal is to minimize draw calls,  
which have to occur once per MaterialInstance, while still providing a way to customize the text per  
character.

Finally, a Font is texture data, a MaterialInstance, and character spacing all wrapped into one. These are  
allocated and handled with a FontManager(?).

>*A Quick Note*
>We need to consider editing text. Can we just change the characters that we need to? Need to look into  
how buffers are populated.

You'll notice that the TextAsset doesn't actually contain any text. The low level classes are only  
concerned with converting already configured Font and Text data into a render object.  
There are two helper functions provided, the first taking an array of *some object* and populating the  
buffers of a TextAsset. The second function takes a Font and produces a MaterialInstance for it to own.

**The High Level**
With our low level interface decided, we can move onto how we're actually going to manage our text.  
Our high level class will be called the TextBox. The TextBoxes concern is to establish the spacing and  
boundaries of the text while also storing the resources to construct a TextAsset.

This brings us to the next obstacle. What format do we want to use to produce our Words. This is the same  
issue as editing text! Can fill a buffer bit by bit or do we need to create a character array first?  
More research for tomorrow.