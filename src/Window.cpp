#include "Window.h"
#include <stdio.h>
#include <stdlib.h>

namespace vme
{
    Window::Window(int width, int height, const char* title)
        : width_(width), height_(height)
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        // TODO: full screen mode
        window_ = glfwCreateWindow(width, height, title, NULL, NULL);

        if(!window_)
        {
            // TODO: Handle Error
            abort();
        }
    }
    
    Window::~Window()
    {
        glfwDestroyWindow(window_);
    }
} // namespace vme
