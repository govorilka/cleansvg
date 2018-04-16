#include "Application.hpp"
#include "Window.hpp"

int main()
{
    auto app = CleanSVG::Application::create();
    if (!app)
    {
        return -1;
    }

    auto window = app->createWindow(640, 480, "ClearSVG");
    if (!window)
    {
        return -1;
    }

    return window->loop();
}
