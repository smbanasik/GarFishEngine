// Spencer Banasik
// Created: 12/14/2024
// Last Modified: 12/14/2024
// Description:
// Structure for engine
#ifndef GF_ENGINE_HPP
#define GF_ENGINE_HPP

class GLFWWindow;

namespace GF {
class Engine {
public:
    Engine();
    ~Engine();
    void run();

private:

    GLFWWindow* window;

    void init_window(GLFWWindow* window);

};
}
#endif