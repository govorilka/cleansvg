#include <string>

#include "Window.hpp"

int main()
{
    auto window = CleanSVG::Window::create(640, 480, "ClearSVG");
    return window ? window->loop() : -1;
}
