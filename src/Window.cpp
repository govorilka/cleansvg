#include <cstdint>

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>

#include <GLFW/glfw3.h>

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <iostream>

#include "linmath.h"

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
"attribute vec2 vPos;\n"
"attribute vec2 vPosTex;\n"
"varying vec2 texcoord;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
"    texcoord = vPosTex;\n"
"}\n";

const char* fragment_shader_text =
"#version 410 core\n"
"uniform sampler2D texture;\n"
"varying vec2 texcoord;\n"
"void main()\n"
"{\n"
"    gl_FragColor = texture2D(texture, texcoord);\n"
"}\n";

float global_x = 0;
float global_y = 0;
float global_scale = 1.0;

Window* window = nullptr;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    } else if (key == GLFW_KEY_LEFT && (action == GLFW_REPEAT || action == GLFW_PRESS)) {
        global_x -= 0.1f;
    } else if (key == GLFW_KEY_RIGHT && (action == GLFW_REPEAT || action == GLFW_PRESS)) {
        global_x += 0.1f;
    } else if (key == GLFW_KEY_UP && (action == GLFW_REPEAT || action == GLFW_PRESS)) {
        global_y += 0.1f;
    } else if (key == GLFW_KEY_DOWN && (action == GLFW_REPEAT || action == GLFW_PRESS)) {
        global_y -= 0.1f;
    }
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (yoffset > 0) {
       global_scale = std::min(global_scale * 1.1, 10.0);
    } else if (yoffset < 0) {
       global_scale = std::max(global_scale / 1.1, 0.1);
    }
}

void dropCallback(GLFWwindow* handle, int count, const char** paths)
{
    if (window && window->handle() == handle && count > 0) {
        window->load(paths[0]);
    }
}

void error_callback(int error, const char* description)
{
    std::cerr << "Error: " << description << std::endl;
}

} // anonymous namespace

Window::Window(GLFWwindow* handle, PrivateTag)
    : m_handle(handle)
{
    window = this;
    glfwSetKeyCallback(m_handle, keyCallback);
    glfwSetScrollCallback(m_handle, scrollCallback);
    glfwSetDropCallback(m_handle, dropCallback);
}

Window::~Window()
{
    window = nullptr;
    glfwDestroyWindow(m_handle);
    glfwTerminate();
}

std::unique_ptr<Window> Window::create(int w, int h, const std::string& title)
{
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        return nullptr;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    auto window = glfwCreateWindow(w, h, title.c_str(), nullptr, nullptr);
    if (!window) {
        return nullptr;
    }

    return std::make_unique<Window>(window, PrivateTag());
}

void Window::load(const char* filename)
{
    m_image = VectorImage::load(filename);
    if (m_image) {
        m_updateImage = true;
        m_image->savePng("1.png");
    }
}

int Window::loop()
{
    glfwMakeContextCurrent(m_handle);

    int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0) {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        return -1;
    }

    glfwSwapInterval(1);

    while (!glfwWindowShouldClose(m_handle))
    {
        int width = 0;
        int height = 0;
        glfwGetFramebufferSize(m_handle, &width, &height);
        float ratio = width / float(height);

        glViewport(0, 0, width, height);
        glClearColor(1.0, 1.0, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

        updateImage();
        if (m_program != 0) {
            auto mvp_location = glGetUniformLocation(m_program, "MVP");

            glm::mat4 m(1.f);
            m = glm::translate(m, glm::vec3(global_x, global_y, 1.0));
            m = glm::scale(m, glm::vec3(global_scale, global_scale, 1.0));

            glm::mat4 p = glm::ortho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
            glm::mat4 mvp = p * m;

            glUniformMatrix4fv(mvp_location, 1, GL_FALSE,  glm::value_ptr(mvp));

            glUseProgram(m_program);  
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);   
        }
        glfwSwapBuffers(m_handle);
        glfwPollEvents();
    }

    return 0;
}

void Window::updateImage()
{
    if (!m_updateImage)
    {
        return;
    }

    m_updateImage = false;

    if (m_texture == 0) {
        glGenTextures(1, &m_texture);
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);

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

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    auto vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertex_shader);

    auto fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);

    m_program = glCreateProgram();
    glAttachShader(m_program, vertex_shader);
    glAttachShader(m_program, fragment_shader);
    glLinkProgram(m_program);

    if (m_vao == 0) {
        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);
    }

    auto texture_location = glGetUniformLocation(m_program, "texture");
    auto vpos_location = glGetAttribLocation(m_program, "vPos");
    auto texcoord_location = glGetAttribLocation(m_program, "vPosTex");

    GLuint vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glUniform1i(texture_location, GL_TEXTURE0);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(vertices[0]), (void*) 0);

    glEnableVertexAttribArray(texcoord_location);
    glVertexAttribPointer(texcoord_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(vertices[0]), (void*) (sizeof(float) * 2));
}

} // namespace CleanSVG
