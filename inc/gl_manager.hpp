// Spencer Banasik
// Created: 1/1/2025
// Last Modified: 1/5/2025
// Description:
// Owns glfw library
// Serves to generate and modify contexts
// As well as surfaces
#ifndef GL_MANAGER_HPP
#define GL_MANAGER_HPP

#include <gl_types.hpp>

#include <gl_window.hpp>

namespace gf {

class GLManager {
public:
    GLManager();
    ~GLManager();
    GLManager& get();

    struct WInputConxtext {
        gl::WindowContext window;
        //MouseContext mouse;
        //InputContext key;
        //JoyContext joystick;
    };
    
    // TODO: surface creation and deletion
    void create_surface();
    void delete_surface();

    bool check_init() { return is_init; };

private:
    static GLManager* loaded_glfw;
    GLManager(const GLManager& other) = delete;
    GLManager& operator=(const GLManager& other) = delete;

    bool is_init = false;
};
}
#endif
