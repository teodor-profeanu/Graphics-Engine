#include "AppUtil.h"

GLenum glCheckError_(const char* file, int line) {
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR) {
		std::string error;
		switch (errorCode) {
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}

void DebugFB::init() {
	plane.loadModel("assets/models/test/debug.obj");
	shader.loadShader("shaders/debug.vert", "shaders/debug.frag");
	shader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 1.0f)));
}

void DebugFB::draw(int textID) {
	shader.useShaderProgram();
	glUniform1i(glGetUniformLocation(shader.shaderProgram, "fboAttachment"), textID);
	plane.draw(shader);
}