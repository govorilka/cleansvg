#include "Window.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>

namespace {

const struct
{
    float x, y;
    float r, g, b;
} vertices[3] =
{
    { -1.f, -1.f, 1.f, 0.f, 0.f },
    {  1.f, -1.f, 0.f, 1.f, 0.f },
    {   0.f,  1.f, 0.f, 0.f, 1.f }
};

const char* vertex_shader_text =
"#version 110\n"
"uniform mat4 MVP;\n"
"attribute vec3 vCol;\n"
"attribute vec2 vPos;\n"
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
"    color = vCol;\n"
"}\n";

const char* fragment_shader_text =
"#version 110\n"
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_FragColor = vec4(color, 1.0);\n"
"}\n";

float global_x = 0;
float global_y = 0;
float global_scale = 1.0;

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

} // anonymous namespace


CleanSVG::Window::Window(GLFWwindow* window)
    : window_(window)
{
    glfwSetKeyCallback(window, key_callback);
    glfwSetScrollCallback(window, scroll_callback);
}

CleanSVG::Window::~Window()
{
    glfwDestroyWindow(window_);
}

int CleanSVG::Window::loop()
{
    glfwMakeContextCurrent(window_);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval(1);

    // NOTE: OpenGL error checks have been omitted for brevity

    auto vertex_buffer = GLuint(0);
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    auto vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertex_shader);

    auto fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);

    auto program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    auto mvp_location = glGetUniformLocation(program, "MVP");
    auto vpos_location = glGetAttribLocation(program, "vPos");
    auto vcol_location = glGetAttribLocation(program, "vCol");

    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(vertices[0]), (void*) 0);
    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(vertices[0]), (void*) (sizeof(float) * 2));

    while (!glfwWindowShouldClose(window_))
    {
        int width = 0;
        int height = 0;
        glfwGetFramebufferSize(window_, &width, &height);
        float ratio = width / float(height);

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        glm::mat4 m(1.f);
        m = glm::translate(m, glm::vec3(global_x, global_y, 1.0));
        m = glm::scale(m, glm::vec3(global_scale, global_scale, 1.0));

        glm::mat4 p = glm::ortho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
        glm::mat4 mvp = p * m;

        glUseProgram(program);
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE,  glm::value_ptr(mvp));
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window_);
        glfwPollEvents();
    }

    return 0;
}
