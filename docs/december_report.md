# December Report {#decemberreport}

## URGENT TODOS

Adjust our cmake script to handle files again, remove source_groups, and remove file prefixes Adjust our third_party script to properly handle free type. Consuder using git submodules for libraries that we fully include.

## vkh_font_manager

### Requirements

___

Our goal with a font manager is to arbitrarily create fonts of different types and sizes for other components to use. There are two parts of a font needed to render text:

- The font image (for the texture)
- The font data (for the spacings)

These are connected, but ultimately separate resources. We'll be applying the structure of arrays idiom to this system.

#### Current Flow

First, let's review the flow. After the library is initialized, we create a `FT_Face`, which takes a font path. Afterwards, we can set different attributes of the font, such as the pixel size.

With the face created, we can access each glyphs bitmap information and paddings. Once that data has been extracted, the `FT_Face` can be safely discarded, so it is advantageous to assemble the font image and font data together. However, these are ultimately separate systems that benefit from low coupling.

With the data in hand, we can produce an array of characters containing the font data and an `AllocatedImage` that represents the font texture.

#### Proposed Architecture

Recall that font data and font images are best with low coupling, because they are separate systems. Given this, we will want two files: font_data and font_image, each with their own types and helper functions. These functions will take in an `FT_Face` and produce an array of character data or an `AllocatedImage`, respectively.

These files will be used to power the `FontManager`, whose goal is to store the outputs of those helper functions and provide an interface to access them. The `FontManager` should encapsulate the details of these files, so it should only take a path and a font name. The `FontManager` will have its own type that it returns, a `Font` which contains pointers to the font data and font image.

### Character

Contains individual character information such as texture position, size, and spacing

### FontData

Contains the character spacings for the font.

## **Different Pipelines** 

MeshNode -> MeshAsset -> GeoSurface -> GLTFMaterial (owning) -> MaterialInstance

TextNode -> TextAsset -> Word -> TextMaterial (owning) -> MaterialInstance

## Text (CURRENTLY DOING)

### Requirements

___

The fonts were the easy part. Now we need to move onto the next part: actually rendering text. For now, let's focus on the vertices. This means we'll need our position data and texture data. Let's start with the position data.

For the purposes of this implementation, we avoid discussing about resource management, and we will not be considering Multi-Word optimizations. Suppose we wanted to write the sentence "Hello, world!" What would this look like? Text in Vulkan is a set of textured quads to represent the letters. These quads would all be spaced relative to each other, so they would be one "model," and all have their coordiantes in model space. Then, we can use a transformation matrix to translate them across the screen.

To achieve this we need a function that will *generate squares* using the spacings. The vertices themselves are easy to construct since it's a square with dimensions according to the size of the character. Then, we can offset the vertex coordinates according to the spacings. Using indices is an important requirement, since it reduces the number of vertices we need by a third. The indices can also be calculated manually according to whatever square we're on, and the texture coordinates should be easily added using the font data. `generate_text_squares` will perform these operations, taking in a `std::string` and an allocator, and outputting whatever the `upload_mesh` function requires.

Helper functions called `generate_text_indicies`, `generate_text_offsets`, and `generate_text_texcoords` will be used to break the work down into more manageable sections.

### Proposed Architecture

___

## vkh_textlow_types

TextMat - useless placeholder for now. Will be TextMaterial. Word - geo surface equivalent with start_idx, count, and material. Change start_idx and count to reflect letters & not verts. TextAsset - MeshAsset equivalent. Contains words (surfaces) and Mesh buffers which will contain vertex data.

## gf_text

Character - redeclared in here, old perhaps?

Font - redeclared in here, old perhaps?

Text box - High level text implementation

TextManager - High level text implementation
