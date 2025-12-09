# December Report {#decemberreport}

## Current Progress

### vkh_font_manager

#### Character
Contains individual character information such as texture position, size, and spacing

#### Font
Contains a FontMaterial, font name, and the whole character information.
Additionally contains a MaterialImage which is the font texture.
Finally, contains the MaterialInstance which is used to create render objects from the Font.
Contains 3 empty functions that would be used to generate a font texture, the spacings, and the material instance.

**Different Pipelines**
MeshNode -> MeshAsset -> GeoSurface -> GLTFMaterial (owning) -> MaterialInstance
TextNode -> TextAsset -> Word -> TextMaterial (owning) -> MaterialInstance

How does a Font play into this?
Because a font contains the character data and font texture, it is a simple resource much like an image.

In this case, we should have a FontManager which will own all of the fonts (character and texture data).
You will be able to create TextMaterials from fonts.

At the end of the day, fonts should **only** be character and texture data.

I believe we need only 1 material for this which is the ImageMaterial. Normally materials are owned by a material manager. We may
want to consider using a non-owning pointer and have the ImageMaterial owned by the material manager.

Since Fonts will now be iamge & character data, we can remove the AllocatedImage map and keep it associated with the font.

### vkh_textlow_types

TextMat - useless placeholder for now. Will be TextMaterial.
Word - geo surface equivalent with start_idx, count, and material. Change start_idx and count to reflect letters & not verts.
TextAsset - MeshAsset equivalent. Contains words (surfaces) and Mesh buffers which will contain vertex data.

### vkh_textlow
Empty, remove.

### vkh_texthigh
Empty, remove.

### gf_text

Character - redeclared in here, old perhaps?
Font - redeclared in here, old perhaps?
Text box - High level text implementation
TextManager - High level text implementation

Old file that should be cleaned up and removed.

## Proposal

**Plan of attack**
___

First, we must separate the logic between font management and text management.
Fonts are entirely focused on spacing and images.
Text is entirely focused on creating vertices and using the spacing and images to assemble words.

Start with fonts:
- Redo the font type
- Create the font manager
- Create an interface with the font manager for interacting with fonts.

Definition of done:
- When we can arbitrarily load fonts and their data, we are done.

Continue with basic text rendering:
- Create a single Word of text without worrying about spacing and arbitrarily writing to that Word.
- Then, consider spacing.

Finally, work on multiWord text.

This covers the basic functionality of text, and then we can consider more advanced options.