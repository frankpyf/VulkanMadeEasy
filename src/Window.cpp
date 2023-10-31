#include "Window.h"
#include <stdio.h>

namespace vme
{
    Window::Window(int width, int height, const char* title)
        : width_(width), height_(height)
    {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 0);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        // TODO: full screen mode
        window_ = glfwCreateWindow(width, height, title, NULL, NULL);

        if(!window_)
        {
            // TODO: Handle Error
        }
    }
    
    Window::~Window()
    {
        glfwDestroyWindow(window_);
    }
} // namespace vme
