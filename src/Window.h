#pragma once

#include <memory>

#include "Fwd.h"
#include "VectorImage.h"

namespace CleanSVG {

class Window
{
    struct PrivateTag{};

public:
    Window(GLFWwindow* handle, PrivateTag);
    ~Window();

    static std::unique_ptr<Window> create(int w, int h, const std::string& title);

    GLFWwindow* handle() const { 
        return m_handle;
    };

    void load(const char* filename);

    int loop();

private:
    void updateImage();

private:
    GLFWwindow* m_handle = nullptr;
    std::unique_ptr<VectorImage> m_image;
    bool m_updateImage = true;
    unsigned m_program = 0;
    unsigned m_texture = 0;
    unsigned m_vao = 0;
};

} // namespace CleanSVG
