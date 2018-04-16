#pragma once

class GLFWwindow;

namespace CleanSVG
{

class Window
{
public:
    Window(GLFWwindow* window);
    ~Window();

    int loop();

private:
    GLFWwindow* window_ = nullptr;
};

} // CleanSVG
