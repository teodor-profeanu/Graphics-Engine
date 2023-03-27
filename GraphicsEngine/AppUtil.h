#ifndef AppUtil_h
#define AppUtil_h

#include <GL/glew.h>
#include <string>
#include <iostream>
#include "Shader.hpp"
#include "Model3D.hpp"
#include <glm/gtc/type_ptr.hpp>

struct Location {
	GLuint model;
	GLuint view;
	GLuint projection;
	GLuint cameraPos;

	GLuint ambientTexture;
	GLuint diffuseTexture;
	GLuint specularTexture;
	GLuint normalTexture;
	GLuint aoTexture;
};

GLenum glCheckError_(const char* file, int line);
#define glCheckError() glCheckError_(__FILE__, __LINE__)

#define genIndexedName(x,y) (std::string(x) + "[" + std::to_string(y) + "]").c_str()
#define strIntCat(x,y) (std::string(x) + std::to_string(y)).c_str()

struct DebugFB {
	void init();
	void draw(int textID);
	gps::Model3D plane;
	gps::Shader shader;
};


#endif