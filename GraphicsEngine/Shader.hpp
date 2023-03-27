#ifndef Shader_hpp
#define Shader_hpp

#define GLEW_STATIC
#include <GL/glew.h>

#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

namespace gps {
    
class Shader
{
public:
    GLuint shaderProgram;
    void loadShader(std::string vertexShaderFileName, std::string fragmentShaderFileName);
    void loadShader(std::string vertexShaderFileName, std::string geometryShaderFileName,std::string fragmentShaderFileName);
    void useShaderProgram();
    
private:
    std::string readShaderFile(std::string fileName);
    void shaderCompileLog(GLuint shaderId);
    void shaderLinkLog(GLuint shaderProgramId);
};
    
}

#endif
