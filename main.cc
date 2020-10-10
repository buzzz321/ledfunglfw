#include "glad.h" // must be before glfw.h
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

constexpr int32_t SCREEN_WIDTH = 1024;
constexpr int32_t SCREEN_HEIGHT = 768;

constexpr auto vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
)";

constexpr auto fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
} )";

unsigned int loadShaders(const char *shaderSource, GLenum shaderType) {

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

void error_callback(int error, const char *description) {
  std::cerr << "Error: " << description << std::endl;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

int main() {
  float led[] = {
      0.5f,  0.5f, 0.0f,
      0.5f, -0.5f, 0.0f,
      -0.5f, 0.5f, 0.0f,
      
      -0.5f, 0.5f,  0.0f,
      -0.5f, -0.5f, 0.0f,
      0.5f, -0.5f,  0.0f};

  if (!glfwInit()) {
    // Initialization failed
    std::cerr << "Error could not init glfw!" << std::endl;
    exit(1);
  }

  glfwSetErrorCallback(error_callback);

  GLFWwindow *window =
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
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(led), led, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);

  auto vertexShader = loadShaders(vertexShaderSource, GL_VERTEX_SHADER);
  auto fragmentShader = loadShaders(fragmentShaderSource, GL_FRAGMENT_SHADER);
  auto shaderProgram = makeShaderProgram(vertexShader, fragmentShader);

  glUseProgram(shaderProgram);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  while (!glfwWindowShouldClose(window)) {

    // 0. copy our vertices array in a buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(led), led, GL_STATIC_DRAW);
    // 1. then set the vertex attributes pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);
    // 2. use our shader program when we want to render an object
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    processInput(window);
    glfwSwapBuffers(window);
    // Keep running
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
