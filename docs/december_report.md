# December Report {#decemberreport}

## vkh_font_manager

### Requirements
___

Our goal with a font manager is to arbitrarily create fonts of different types and sizes
for other components to use.
There are two parts of a font needed to render text:
    - The font image (for the texture)
    - The font data (for the spacings)
These are connected, but ultimately separate resources. We'll be applying the structure of
arrays idiom to this system.

#### Current Flow
First, let's review the flow. After the library is initialized, we create a `FT_Face`,
which takes a font path. Afterwards, we can set different attributes of the font, such as the
pixel size.

With the face created, we can access each glyphs bitmap information and paddings. Once that
data has been extracted, the `FT_Face` can be safely discarded, so it is advantageous to
assemble the font image and font data together. However, these are ultimately separate systems
that benefit from low coupling.

With the data in hand, we can produce an array of characters containing the font data and 
an `AllocatedImage` that represents the font texture.

#### Proposed Architecture
Recall that font data and font images are best with low coupling, because they are separate systems. Given this, we will want two files: font_data and font_image, each with their own 
types and helper functions. These functions will take in an `FT_Face` and produce an array of
character data  or an `AllocatedImage`, respectively.

These files will be used to power the `FontManager`, whose goal is to store the outputs of 
those helper functions and provide an interface to access them. The `FontManager` should 
encapsulate the details of these files, so it should only take a path and a font name. The 
`FontManager` will have its own type that it returns, a `Font` which contains pointers to 
the font data and font image.

### Character
Contains individual character information such as texture position, size, and spacing

### FontData
Contains the character spacings for the font.

**Different Pipelines**
MeshNode -> MeshAsset -> GeoSurface -> GLTFMaterial (owning) -> MaterialInstance
TextNode -> TextAsset -> Word -> TextMaterial (owning) -> MaterialInstance

## vkh_textlow_types

TextMat - useless placeholder for now. Will be TextMaterial.
Word - geo surface equivalent with start_idx, count, and material. Change start_idx and count to reflect letters & not verts.
TextAsset - MeshAsset equivalent. Contains words (surfaces) and Mesh buffers which will contain vertex data.

## gf_text

Character - redeclared in here, old perhaps?
Font - redeclared in here, old perhaps?
Text box - High level text implementation
TextManager - High level text implementation

Old file that should be cleaned up and removed.

This covers the basic functionality of text, and then we can consider more advanced options.
