#include <garfish/camera.hpp>

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#define GLFW_INCLUDE_VULKAN
#include<GLFW/glfw3.h>

#include <window_input/wi_manager.hpp>
#include <window_input/wi_types.hpp>

float gf::Camera::pitch = 0.f;
float gf::Camera::yaw = 0.f;
glm::vec3 gf::Camera::position{};
glm::vec3 gf::Camera::velocity{};
bool gf::Camera::mouse_enabled = false;

glm::mat4 gf::Camera::get_view_matrix() {
    glm::mat4 camera_translation = glm::translate(glm::mat4(1.f), position);
    glm::mat4 camera_rotation = get_rotation_matrix();
    return glm::inverse(camera_translation * camera_rotation);
}
glm::mat4 gf::Camera::get_rotation_matrix() {
    glm::quat pitch_rotation = glm::angleAxis(pitch, glm::vec3{ 1.f, 0.f, 0.f });
    glm::quat yaw_rotation = glm::angleAxis(yaw, glm::vec3{ 0.f, -1.f, 0.f });
    return glm::toMat4(yaw_rotation) * glm::toMat4(pitch_rotation);
}

void gf::Camera::glfw_camera_callback(wi::WInputContext* context, wi::Key* key) {
    if (key->action == GLFW_PRESS) {
        switch (key->key) {
        case GLFW_KEY_W:
            Camera::velocity.z = -1;
            break;
        case GLFW_KEY_S:
            Camera::velocity.z = 1;
            break;
        case GLFW_KEY_A:
            Camera::velocity.x = -1;
            break;
        case GLFW_KEY_D:
            Camera::velocity.x = 1;
            break;
        }
    }
    if (key->action == GLFW_RELEASE) {
        switch (key->key) {
        case GLFW_KEY_W:
            Camera::velocity.z = 0;
            break;
        case GLFW_KEY_S:
            Camera::velocity.z = 0;
            break;
        case GLFW_KEY_A:
            Camera::velocity.x = 0;
            break;
        case GLFW_KEY_D:
            Camera::velocity.x = 0;
            break;
        }
    }
}

void gf::Camera::glfw_camera_mouse(wi::WInputContext* context) {
    if (mouse_enabled)
        return;
    if (context->mouse.first_mouse) {
        wi::Double2D new_pos = context->mouse.get_mouse_coords();
        yaw = static_cast<float>(new_pos.x);
        pitch = static_cast<float>(new_pos.y);
        context->mouse.first_mouse = false;
    }
    wi::Double2D offsets = context->mouse.get_mouse_offset();

    // TODO: remove magic number
    yaw += static_cast<float>(offsets.x) * 0.00075;
    pitch -= static_cast<float>(offsets.y) * 0.00075;
}

void gf::Camera::update() {
    glm::mat4 camera_rotation = get_rotation_matrix();
    // TODO: remove magic number
    position += glm::vec3(camera_rotation * glm::vec4(velocity * 0.25f, 0.f));
}

void gf::Camera::mouse_swap(wi::WInputContext* context, wi::Key* key) {
    if (key->action == GLFW_PRESS)
        mouse_enabled = !mouse_enabled;
    
    (mouse_enabled) ? context->mouse.enable_cursor() : context->mouse.disable_cursor();
}