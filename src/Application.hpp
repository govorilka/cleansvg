#pragma once

#include <memory>
#include <string>

namespace CleanSVG
{

class Window;

class Application
{
    struct PrivateTag{};
public:
    Application(PrivateTag);
    ~Application();

    static std::unique_ptr<Application> create();

    std::unique_ptr<Window> createWindow(int w, int h, const std::string& title);
};

} // CleanSVG
