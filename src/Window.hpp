#pragma once

#include <memory>

struct GLFWwindow;

namespace CleanSVG {

class VectorImage;

class Window
{
    struct PrivateTag{};
public:
    Window(GLFWwindow* handle, PrivateTag);
    ~Window();

    static std::unique_ptr<Window> create(int w, int h, const std::string& title);

    GLFWwindow* handle() const { return handle_; };

    void load(const char* filename);

    int loop();

private:
    void updateImage();

private:
    GLFWwindow* handle_ = nullptr;
    std::unique_ptr<VectorImage> image_;
    bool updateImage_ = true;
    unsigned program_ = 0;
    unsigned texture_ = 0;
    unsigned vao_ = 0;
};

} // namespace CleanSVG
