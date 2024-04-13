#include "openglWindow.h"

openglWindow::~openglWindow()
{
    onUninit();
    glfwTerminate();
}

bool openglWindow::createWindow(int w, int h)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(800, 600, "ML_CG_TEST", NULL, NULL);
    if (m_window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(m_window);
    glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);


	return onInit();
}

void openglWindow::run()
{
    while (!glfwWindowShouldClose(m_window))
    {
        processInput(m_window);

        doRender();

        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
}

void openglWindow::processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

}
