#include <cstdint>
#include <algorithm>
#include <iostream>

#ifdef __APPLE__
/* Defined before OpenGL and GLUT includes to avoid deprecation messages */
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#else // __APPLE__
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#endif // __APPLE__

#include "Window.h"

namespace CleanSVG {

namespace {

const struct
{
    float x, y;
    float s, t;
} vertices[4] =
{
    { -1.f, -1.f, 0.f, 1.f },
    {  -1.f, 1.f, 0.f, 0.f },
    {   1.f,  -1.f, 1.f, 1.f },
    {   1.f,  1.f, 1.f, 0.f }
};

const char* vertex_shader_text =
"#version 410 core\n"
"uniform mat4 MVP;\n"
"layout (location = 0) in vec2 vPos;\n"
"layout (location = 1) in vec2 vPosTex;\n"
"layout (location = 0) out vec2 texcoord;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
"    texcoord = vPosTex;\n"
"}\n";

const char* fragment_shader_text =
"#version 410 core\n"
"uniform sampler2D tex;\n"
"layout (location = 0) in vec2 texcoord;\n"
"out vec4 finalColor;"
"void main()\n"
"{\n"
"    finalColor = texture(tex, texcoord);\n"
"}\n";

} // anonymous namespace

Window::Window(GLFWwindow* handle, PrivateTag)
    : m_handle(handle)
{
    glfwSetWindowUserPointer(m_handle, reinterpret_cast<void *>(this));

    glfwSetKeyCallback(m_handle, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        if (auto* handler = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window))) {
            handler->onKeyEvent(key, scancode, action, mods);
        }
    });
    glfwSetScrollCallback(m_handle, [](GLFWwindow* window, double xoffset, double yoffset) {
        if (auto* handler = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window))) {
            handler->onScrollEvent(xoffset, yoffset);
        }
    });
    glfwSetDropCallback(m_handle, [](GLFWwindow* window, int count, const char** paths) {
        if (auto* handler = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window))) {
            handler->onDropEvent(count, paths);
        }
    });
}

Window::~Window()
{
    glfwDestroyWindow(m_handle);
    glfwTerminate();
}

std::unique_ptr<Window> Window::create(int w, int h, const std::string& title)
{
    glfwSetErrorCallback([](int error, const char* description){
        std::cerr << "Error: " << description << std::endl;
    });

    if (!glfwInit()) {
        return nullptr;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

    auto* handle = glfwCreateWindow(w, h, title.c_str(), nullptr, nullptr);
    if (!handle) {
        return nullptr;
    }

    return std::make_unique<Window>(handle, PrivateTag());
}

void Window::load(const char* filename)
{
    m_image = VectorImage::load(filename);
    if (m_image) {
        m_image->savePng("1.png");
    }
    m_updateImage = true;
    m_camera.reset();
}

int Window::loop()
{
    glfwMakeContextCurrent(m_handle);

    {
        int version = gladLoadGL(glfwGetProcAddress);
        if (version == 0) {
            std::cout << "Failed to initialize OpenGL context" << std::endl;
            return -1;
        }
    }

    {
        const GLubyte* renderer = glGetString(GL_RENDERER);
        const GLubyte* version = glGetString(GL_VERSION);
        std::cout << "Renderer: " << renderer << std::endl;
        std::cout << "OpenGL version supported: " << version << std::endl;
    }



    glfwSwapInterval(1);

    while (!glfwWindowShouldClose(m_handle)) {
        int width = 0;
        int height = 0;
        glfwGetFramebufferSize(m_handle, &width, &height);
        m_camera.setRatio(width / float(height));

        glViewport(0, 0, width, height);
        glClearColor(1.0, 1.0, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

        updateImage();
        if (m_program != 0) {
            auto mvp_location = glGetUniformLocation(m_program, "MVP"); 
            glUniformMatrix4fv(mvp_location, 1, GL_FALSE,  m_camera.getData());
            glUseProgram(m_program); 
            glBindVertexArray(m_vao);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);   
        }
        glfwPollEvents();
        glfwSwapBuffers(m_handle);
    }

    return 0;
}

void Window::updateImage()
{
    if (!m_updateImage) {
        return;
    }

    m_updateImage = false;

    GLenum error = GL_NO_ERROR;

    if (m_texture == 0) {
        glGenTextures(1, &m_texture);
    }
    glBindTexture(GL_TEXTURE_2D, m_texture);
    if ((error = glGetError()) != GL_NO_ERROR) {
        return;
    }

    glActiveTexture(GL_TEXTURE0);
    if ((error = glGetError()) != GL_NO_ERROR) {
        return;
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    if (m_image) {
        RasterImage raster = m_image->toRaster();
        if (raster.width != 0 && raster.height != 0) { 
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, raster.width, raster.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, raster.data.data());  
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_RED);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_GREEN);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_BLUE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ALPHA); 
        }
    } else {
        std::uint8_t pixels[16 * 16 * 2];
        {
            std::uint8_t* it = pixels;
            bool toggleRow = false;
            for (int y = 0;  y < 16;  ++y) {
                bool toggleColumn = toggleRow;
                for (int x = 0;  x < 16;  ++x) {
                    *it++ = toggleColumn ? 128 : 192;
                    *it++ = 255;
                    toggleColumn = !toggleColumn;
                }
                toggleRow = !toggleRow;
            }
        }

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, 16, 16, 0, GL_RG, GL_UNSIGNED_BYTE, pixels);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_RED);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_GREEN);
    }

    if (m_program == 0) {
        auto vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
        glCompileShader(vertex_shader);
        {
            GLint success = 0;
            glGetShaderiv(vertex_shader,  GL_COMPILE_STATUS, &success);
            if (success == 0) {
                GLchar log[512] = {};
                GLsizei length = 0;
                glGetShaderInfoLog(vertex_shader, 512, &length, log);
                std::cout << log << std::endl;
                return;
            }
        }

        auto fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
        glCompileShader(fragment_shader);
        {
            GLint success = 0;
            glGetShaderiv(fragment_shader,  GL_COMPILE_STATUS, &success);
            if (success == 0) {
                GLchar log[512] = {};
                GLsizei length = 0;
                glGetShaderInfoLog(fragment_shader, 512, &length, log);
                std::cout << log << std::endl;
                return;
            }
        }

        m_program = glCreateProgram();
        glAttachShader(m_program, vertex_shader);
        glAttachShader(m_program, fragment_shader);
        glLinkProgram(m_program);
        {
            GLint success = 0;
            glGetProgramiv(m_program,  GL_LINK_STATUS, &success);
            if (success == 0) {
                GLchar log[512] = {};
                GLsizei length = 0;
                glGetProgramInfoLog(m_program, 512, &length, log);
                std::cout << log << std::endl;
                return;
            }
        }
    } 

    glUseProgram(m_program); 
    if ((error = glGetError()) != GL_NO_ERROR) {
        return;
    }

    auto texture_location = glGetUniformLocation(m_program, "tex");
    auto vpos_location = glGetAttribLocation(m_program, "vPos");
    auto texcoord_location = glGetAttribLocation(m_program, "vPosTex");

    if (m_vbo == 0) {
        glGenBuffers(1, &m_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        if ((error = glGetError()) != GL_NO_ERROR) {
            return;
        }
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        if ((error = glGetError()) != GL_NO_ERROR) {
            return;
        }
    } else {
       glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
       if ((error = glGetError()) != GL_NO_ERROR) {
           return;
       }
   }

    if (m_vao == 0) {
        glGenVertexArrays(1, &m_vao);
    }
    glBindVertexArray(m_vao);
    if ((error = glGetError()) != GL_NO_ERROR) {
        return;
    }

    glUniform1i(texture_location, 0);
    if ((error = glGetError()) != GL_NO_ERROR) {
        return;
    }
   
    glEnableVertexAttribArray(vpos_location);
    if ((error = glGetError()) != GL_NO_ERROR) {
        return;
    }

    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(vertices[0]), (void*) 0);
    if ((error = glGetError()) != GL_NO_ERROR) {
        return;
    }

    glEnableVertexAttribArray(texcoord_location);
    glVertexAttribPointer(texcoord_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(vertices[0]), (void*) (sizeof(float) * 2));
    if ((error = glGetError()) != GL_NO_ERROR) {
        return;
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Window::onKeyEvent(int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(m_handle, GLFW_TRUE);
    } else if (key == GLFW_KEY_LEFT && (action == GLFW_REPEAT || action == GLFW_PRESS)) {
        m_camera.left();
    } else if (key == GLFW_KEY_RIGHT && (action == GLFW_REPEAT || action == GLFW_PRESS)) {
        m_camera.right();
    } else if (key == GLFW_KEY_UP && (action == GLFW_REPEAT || action == GLFW_PRESS)) {
       m_camera.up();
    } else if (key == GLFW_KEY_DOWN && (action == GLFW_REPEAT || action == GLFW_PRESS)) {
       m_camera.down();
    }
}

void Window::onScrollEvent(double xoffset, double yoffset)
{
    m_camera.scroll(xoffset, yoffset);
}

void Window::onDropEvent(int count, const char** paths)
{
    if (count > 0) {
        load(paths[0]);
    }
}

} // namespace CleanSVG
