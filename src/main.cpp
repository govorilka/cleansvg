#include "Application.hpp"
#include "Window.hpp"

int main()
{
    if (auto app = CleanSVG::Application::create())
    {
        if (auto window = app->createWindow(640, 480, "ClearSVG"))
        {
            return window->loop();
        }
    }
    return -1;
}
