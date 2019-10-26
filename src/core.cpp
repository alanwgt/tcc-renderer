#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>

#include "core.h"

namespace wega
{
    void Core::DebugOpenGL()
    {
        std::cout << "\nOPENGL DEBUG INFO:\n";
        int intVar;
        glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &intVar);
        std::cout << "Max n of vertex attribs supported: " << intVar << "\n";
        std::cout << "\n===============================\n\n";
    }

    void Core::ClearColor(float r, float g, float b, float a)
    {
        glClearColor(r, g, b, a);
        // clear all the color information from the pixels and use the color above
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void Core::DrawRainbowTriangle(GLFWwindow* window)
    {
        /* float timeValue = glfwGetTime(); */
        /* float colorValue = (std::sin(timeValue) / 2.0f) + 0.5f; */
        /* int vertexColorLocation = glGetUniformLocation(2, ) */
    }
}
