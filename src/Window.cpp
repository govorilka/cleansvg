#include "Window.hpp"
#include "VectorImage.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>

#include "linmath.h"

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
"#version 110\n"
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
"#version 110\n"
"uniform sampler2D texture;\n"
"varying vec2 texcoord;\n"
"void main()\n"
"{\n"
"    gl_FragColor = texture2D(texture, texcoord);\n"
"}\n";

float global_x = 0;
float global_y = 0;
float global_scale = 1.0;

CleanSVG::Window* window = nullptr;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    else if (key == GLFW_KEY_LEFT && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        global_x -= 0.1f;
    }
    else if (key == GLFW_KEY_RIGHT && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        global_x += 0.1f;
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (yoffset > 0)
    {
       global_scale = std::min(global_scale * 1.1, 10.0);
    }
    else if (yoffset < 0)
    {
        global_scale = std::max(global_scale / 1.1, 0.1);
    }
}

void drop_callback(GLFWwindow* handle, int count, const char** paths)
{
    if (window && window->handle() == handle)
    {
        if (count > 0)
        {
            window->load(paths[0]);
        }
    }
}

} // anonymous namespace

CleanSVG::Window::Window(GLFWwindow* handle)
    : handle_(handle)
{
    window = this;

    glfwSetKeyCallback(handle_, key_callback);
    glfwSetScrollCallback(handle_, scroll_callback);
    glfwSetDropCallback(handle_, drop_callback);
}

CleanSVG::Window::~Window()
{
    window = nullptr;
    glfwDestroyWindow(handle_);
}

 void CleanSVG::Window::load(const char* filename)
 {
    image_ = VectorImage::load(filename);
    if (image_)
    {
        updateImage_ = true;
        image_->savePng("1.png");
    }
 }

int CleanSVG::Window::loop()
{
    glfwMakeContextCurrent(handle_);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval(1);

    while (!glfwWindowShouldClose(handle_))
    {
        int width = 0;
        int height = 0;
        glfwGetFramebufferSize(handle_, &width, &height);
        float ratio = width / float(height);

        glViewport(0, 0, width, height);
        glClearColor(1.0, 1.0, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

        updateImage();

        if (program_ != -1)
        {
            auto mvp_location = glGetUniformLocation(program_, "MVP");

            glm::mat4 m(1.f);
            m = glm::translate(m, glm::vec3(global_x, global_y, 1.0));
            m = glm::scale(m, glm::vec3(global_scale, global_scale, 1.0));

            glm::mat4 p = glm::ortho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
            glm::mat4 mvp = p * m;

            glUniformMatrix4fv(mvp_location, 1, GL_FALSE,  glm::value_ptr(mvp));        
        }

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glfwSwapBuffers(handle_);
        glfwPollEvents();
    }

    return 0;
}

void CleanSVG::Window::updateImage()
{
    if (!updateImage_)
    {
        return;
    }

    updateImage_ = false;

    // Create the OpenGL objects inside the first context, created above
    // All objects will be shared with the second context, created below
    int x, y;
    char pixels[16 * 16];

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    if (image_)
    {
        RasterImage raster = image_->toRaster();
        if (raster.width != 0 && raster.height != 0)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, raster.width, raster.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, raster.data.data());    
        }
    }
    else
    {
        srand((unsigned int) glfwGetTimerValue());

        for (y = 0;  y < 16;  y++)
        {
            for (x = 0;  x < 16;  x++)
            {
                pixels[y * 16 + x] = rand() % 256;
            }
        }

        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, 16, 16, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, pixels);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    auto vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertex_shader);

    auto fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);

    program_ = glCreateProgram();
    glAttachShader(program_, vertex_shader);
    glAttachShader(program_, fragment_shader);
    glLinkProgram(program_);

    auto texture_location = glGetUniformLocation(program_, "texture");
    auto vpos_location = glGetAttribLocation(program_, "vPos");
    auto texcoord_location = glGetAttribLocation(program_, "vPosTex");

    GLuint vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glUseProgram(program_);
    glUniform1i(texture_location, 0);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(vertices[0]), (void*) 0);

    glEnableVertexAttribArray(texcoord_location);
    glVertexAttribPointer(texcoord_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(vertices[0]), (void*) (sizeof(float) * 2));
}