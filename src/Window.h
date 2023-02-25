#pragma once

#include <memory>

#include "Fwd.h"
#include "VectorImage.h"
#include "Camera.h"

namespace CleanSVG {

class Window final
{
    struct PrivateTag{};

public:
    Window(GLFWwindow* handle, PrivateTag);
    ~Window();

    static std::unique_ptr<Window> create(int w, int h, const std::string& title);

    void load(const char* filename);

    int loop();

private:
    void onKeyEvent(int key, int scancode, int action, int mods);
    void onScrollEvent(double xoffset, double yoffset);
    void onDropEvent(int count, const char** paths);

    void updateImage();

private:
    GLFWwindow* m_handle = nullptr;

    std::unique_ptr<VectorImage> m_image;
    bool m_updateImage = true;
    unsigned m_program = 0;
    unsigned m_texture = 0;
    unsigned m_vao = 0;

    Camera m_camera;
};

} // namespace CleanSVG
