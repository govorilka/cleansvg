#pragma once

#include <memory>

struct GLFWwindow;

namespace CleanSVG
{

class VectorImage;

class Window
{
public:
    Window(GLFWwindow* handle);
    ~Window();

    GLFWwindow* handle() const { return handle_; };

    void load(const char* filename);

    int loop();

private:
    void updateImage();

private:
    GLFWwindow* handle_ = nullptr;
    std::unique_ptr<VectorImage> image_;
    bool updateImage_ = true;
    unsigned program_ = -1;
    unsigned texture_ = 0;
};

} // CleanSVG
