#include "glad.h" // must be before glfw.h
#include <GLFW/glfw3.h>
#include <iostream>

constexpr int32_t SCREEN_WIDTH = 1024;
constexpr int32_t SCREEN_HEIGHT = 768;

void error_callback(int error, const char* description)
{
    std::cerr<<"Error: "<< description <<std::endl;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main(){
    if (!glfwInit())
    {
        // Initialization failed
        std::cerr<< "Error could not init glfw!"<< std::endl;
        exit(1);
    }

    glfwSetErrorCallback(error_callback);

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "My Title", NULL, NULL);
    if (!window)
    {
        std::cerr<< "Error could not create window"<<std::endl;
        exit(1);
        // Window or OpenGL context creation failed
    }
    
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);  
    
    if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
	{
        std::cerr<<" Error could not load glad "<< std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(1);
	}
    
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        glfwSwapBuffers(window);
        // Keep running
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
