#include "Application.hpp"
#include "Window.hpp"

#include <GLFW/glfw3.h>

#include <iostream>

namespace {

void error_callback(int error, const char* description)
{
    std::cerr << "Error: " << description << std::endl;
}

} // anonymous namespace

CleanSVG::Application::Application(PrivateTag)
{}

CleanSVG::Application::~Application()
{
    glfwTerminate();
}

std::unique_ptr<CleanSVG::Application> CleanSVG::Application::create()
{
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
    {
        return nullptr;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    return std::make_unique<Application>(PrivateTag());
}

std::unique_ptr<CleanSVG::Window> CleanSVG::Application::createWindow(int w, int h, const std::string& title)
{
    auto window = glfwCreateWindow(w, h, title.c_str(), NULL, NULL);
    if (!window)
    {
        return nullptr;
    }

    return std::make_unique<Window>(window);
}
