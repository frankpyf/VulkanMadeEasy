#pragma once
#include "GLFW/glfw3.h"

namespace vme
{
    class Window
    {
    private:
        /* data */
        GLFWwindow* window_;
    public:
        Window(int width, int height, const char* title);
        ~Window();
    };
    
} // namespace vme
