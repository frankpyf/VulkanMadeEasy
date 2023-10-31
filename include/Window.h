#pragma once
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace vme
{
    class Window
    {
    private:
        /* data */
        int width_;
        int height_;
        GLFWwindow* window_;
    public:
        Window(int width, int height, const char* title);
        virtual ~Window();
        inline const int GetHeight()   const { return height_; }
        inline const int GetWidth()    const { return width_; }
    };
    
} // namespace vme
