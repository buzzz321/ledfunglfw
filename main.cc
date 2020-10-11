#include "glad.h" // must be before glfw.h
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

constexpr int32_t SCREEN_WIDTH = 1024;
constexpr int32_t SCREEN_HEIGHT = 768;

constexpr auto vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";

constexpr auto fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
} )";

unsigned int loadShaders(const char* shaderSource, GLenum shaderType) {

    unsigned int shader{0};
    int success{0};
    char infoLog[1024];

    shader = glCreateShader(shaderType); // GL_VERTEX_SHADER

    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
    return shader;
}

unsigned int makeShaderProgram(uint32_t vertexShader, uint32_t fragmentShader) {
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

void error_callback(int error, const char* description) {
    std::cerr << "Error: " << description << std::endl;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action,
                  int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void camera(uint32_t shaderId) {
    glm::mat4 view = glm::mat4(1.0f);

    glm::vec3 cameraPos = glm::vec3(32.0f, 32.0f, 90.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

    int modelView = glGetUniformLocation(shaderId, "view");
    glUniformMatrix4fv(modelView, 1, GL_FALSE, glm::value_ptr(view));
}

std::vector<glm::vec3> generateLeds(uint32_t amount) {
    std::vector<glm::vec3> retVal;
    for (float y = 0; y < (float)amount; ++y) {
        for (float x = 0; x < (float)amount; ++x) {
            glm::vec3 vertex((float)x*2,(float)y*2,0.0f);
            std::cout<<vertex.x<<" "<< vertex.y<< " " <<vertex.z<<std::endl;

            retVal.push_back(vertex);
        }
    }

    return retVal;
}

std::vector<float> generatedLedVertices(std::vector<float> &led, uint32_t amount){
    std::vector<float> retVal;
    for (uint32_t i = 0; i< amount; i++){
        for(auto coord :led){
            retVal.push_back(coord);
        }
    }
    return retVal;
}

int main() {
    std::vector<float> led = {
        0.5f,  0.5f, 0.0f,
        0.5f,  -0.5f, 0.0f,
        -0.5f, 0.5f,  0.0f,
        
        -0.5f, 0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        0.5f,  -0.5f, 0.0f
    };

    auto leds = generateLeds(32);
    auto ledVertices = generatedLedVertices(led, 32);
    
    if (!glfwInit()) {
        // Initialization failed
        std::cerr << "Error could not init glfw!" << std::endl;
        exit(1);
    }

    glfwSetErrorCallback(error_callback);

    GLFWwindow* window =
        glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "My Title", NULL, NULL);
    if (!window) {
        std::cerr << "Error could not create window" << std::endl;
        exit(1);
        // Window or OpenGL context creation failed
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << " Error could not load glad " << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(1);
    }

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, ledVertices.size(), ledVertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void*)0);
    glEnableVertexAttribArray(0);

    auto vertexShader = loadShaders(vertexShaderSource, GL_VERTEX_SHADER);
    auto fragmentShader = loadShaders(fragmentShaderSource, GL_FRAGMENT_SHADER);
    auto shaderProgram = makeShaderProgram(vertexShader, fragmentShader);

    float fov = glm::radians(45.0f);
    glm::mat4 projection = glm::perspective(
        fov, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
  
    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

        // 2. use our shader program when we want to render an object
        glUseProgram(shaderProgram);

        int modelprj = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(modelprj, 1, GL_FALSE, glm::value_ptr(projection));
        
        camera(shaderProgram);
        
        glBindVertexArray(VAO);        
        for(auto& poly:leds){
            //std::cout<<poly.x<<" "<< poly.y<< " " <<poly.z<<std::endl;
            
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, poly);
            model = glm::scale(model, glm::vec3(0.2, 0.2, 0.2));  

            
            int modelLoc = glGetUniformLocation(shaderProgram, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        
        glfwSwapBuffers(window);
        // Keep running
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
