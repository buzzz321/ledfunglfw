#include "glad.h" // must be before glfw.h
#include <GLFW/glfw3.h>
#include <iostream>

void error_callback(int error, const char* description)
{
    std::cerr<<"Error: "<< description <<std::endl;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main(){
    if (!glfwInit())
    {
        // Initialization failed
        std::cerr<< "Error could not init glfw!"<< std::endl;
        exit(1);
    }

    glfwSetErrorCallback(error_callback);

    GLFWwindow* window = glfwCreateWindow(640, 480, "My Title", NULL, NULL);
    if (!window)
    {
        std::cerr<< "Error could not create window"<<std::endl;
        exit(1);
        // Window or OpenGL context creation failed
    }
    
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);

    
    if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
	{
        std::cerr<<" Error could not load glad "<< std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(1);
	}
    
    while (!glfwWindowShouldClose(window))
    {
        // Keep running
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
