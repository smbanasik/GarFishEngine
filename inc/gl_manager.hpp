// Spencer Banasik
// Created: 1/1/2025
// Last Modified: 1/1/2025
// Description:
// Owns glfw library
// Serves to generate and modify contexts
// As well as surfaces
#ifndef GL_MANAGER_HPP
#define GL_MANAGER_HPP

#include <gl_types.hpp>

namespace gf {
namespace gl {
class GLManager {
public:
    GLManager();
    ~GLManager();
    GLManager& get();

    struct ContextBuilder {
        WindowType type;
        Extent2D window_dims;
        std::string title;
        GLFWmonitor* monitor = nullptr;
    };

    WindowContext create_window(ContextBuilder* builder);
    void set_callbacks(WindowContext context, CallbackBits callbacks);

    // TODO: surface creation and deletion
    void create_surface();
    void delete_surface();

    bool is_init = false;

private:
    static GLManager* loaded_glfw;
    GLManager(const GLManager& other) = delete;
    GLManager& operator=(const GLManager& other) = delete;
};
}
}
#endif
