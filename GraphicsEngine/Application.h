#ifndef Application_h
#define Application_h

#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"
#include "SkyBox.hpp"
#include "AppUtil.h"
#include "Light.h"

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void initApp(const char* name, int windowWidth, int windowHeight);
int start();
int init();
void appInit(const char* appName, int appWindowWidth, int appWindowHeight);

void loadModel(const char* modelPath, const char* texturePath);
void incrementFOV(float increment);

void processMovement();
bool initOpenGLWindow();
void initOpenGLState();
void initShaders();
void initUniforms();
void renderScene();
void cleanup();
void loadModels();

#endif

