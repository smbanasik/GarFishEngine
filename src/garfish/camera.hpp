#ifndef GF_CAMERA_HPP
#define GF_CAMERA_HPP

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace gf {
namespace wi {
class WInputContext;
struct Key;
} // namespace wi

/**
 * @struct Camera
 * @brief Temporary structure to handle camera.
 */
class Camera {
  public:
    // TODO: these should be member variables, but I need to update wi_keys to accomodate for calling member functions.
    static glm::vec3 velocity;
    static glm::vec3 position;
    static float pitch;
    static float yaw;
    static bool mouse_enabled;

    glm::mat4 get_view_matrix();
    glm::mat4 get_rotation_matrix();

    static void glfw_camera_callback(wi::WInputContext* context, wi::Key* key);
    static void glfw_camera_mouse(wi::WInputContext* context);
    static void mouse_swap(wi::WInputContext* context, wi::Key* key);

    void update();
};
}
#endif