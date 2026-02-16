#include <vulkan_text/text_generate.hpp>

#include <vector>

vk_text::MeshBufferData vk_text::generate_text_squares(
    const std::string& text,
                                     const vkh_font_manager::Font& font_info) {
    MeshBufferData data;

    data.indices = generate_text_indices(text.length());
    data.vertices = generate_text_vertices(text, font_info);

    return data;
};
std::vector<uint32_t> vk_text::generate_text_indices(const std::size_t text_length) {
    std::vector<uint32_t> indices;

    indices.resize(text_length * 6);
    uint32_t* index = indices.data();

    // 0 1
    // 3 2
    for (std::size_t i = 0; i < text_length; i++) {
        
        std::size_t quad = i * 4;
        *(index++) = quad + 0;
        *(index++) = quad + 1;
        *(index++) = quad + 2;
        *(index++) = quad + 2;
        *(index++) = quad + 3;
        *(index++) = quad + 0;
    }
    return indices;
};
std::vector<vkl::Vertex> vk_text::generate_text_vertices(
    const std::string& text,
                                                const vkh_font_manager::Font& font_info) {
    std::vector<vkl::Vertex> vertices;

    vertices.resize(text.length() * 4);

    vkl::Vertex* vert_data = vertices.data();

    float start_x = 0;
    float start_y = 0;
    for (int i = 0; i < text.length(); i++) {

        std::span<vkl::Vertex, 4> quad(vert_data, 4);
        std::size_t char_num = static_cast<size_t>(static_cast<unsigned char>(text[i]));
        vkh_font_data::Character* character = &font_info.font_data->chars[char_num];

        populate_text_quad_normal(quad);
        populate_text_quad_color(quad);
        populate_text_quad_texture(quad, *character, *font_info.font_image);

        float x1 = start_x + character->padding.x;

        // Our padding.y isn't actually padding, and probably isn't well named.
        // It's actually the distance from the baseline (think bottom line of spiral
        // notebook) to the top of the character. In the case of normal letters, with no
        // dipping, their height and padding.y will be euqal, resulting in their bottom
        // being the baseline (0). When the padding.y is less than the size of the glyph,
        // the difference shows us how much to offset the character downwards.
        float y2 = start_y + (character->size.y - character->padding.y);
        float y1 = y2 - character->size.y;
        float x2 = x1 + character->size.x;

        quad[0].position = {x1, y1, 0};
        quad[1].position = {x2, y1, 0};
        quad[2].position = {x2, y2, 0};
        quad[3].position = {x1, y2, 0};

        start_x += character->padding.z >> 6;

        vert_data += 4;
    }

    return vertices;
};

// 0 1
// 3 2
void vk_text::populate_text_quad_texture(std::span<vkl::Vertex, 4> quad,
                                const vkh_font_data::Character& character,
                                const vkh_font_image::FontImage& texture) {
    // We assume the font is a 1 dimensional array of characters. Therefore,
    // we can calculate the texture coordiantes like so

    // Get width and height of character, and normalize it
    float texture_width = static_cast<float>(character.size.x) / texture.image_size.width;
    float texture_height = static_cast<float>(character.size.y) / texture.image_size.height;

    // Populate texcoords
    quad[0].uv_x = character.texture_topleft.x;
    quad[1].uv_x = character.texture_topleft.x + texture_width;
    quad[2].uv_x = character.texture_topleft.x + texture_width;
    quad[3].uv_x = character.texture_topleft.x;

    quad[0].uv_y = character.texture_topleft.y;
    quad[1].uv_y = character.texture_topleft.y;
    quad[2].uv_y = character.texture_topleft.y + texture_height;
    quad[3].uv_y = character.texture_topleft.y + texture_height;

};