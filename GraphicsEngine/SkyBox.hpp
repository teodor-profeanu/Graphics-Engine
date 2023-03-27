#ifndef SkyBox_hpp
#define SkyBox_hpp

#include <stdio.h>
#include "Shader.hpp"
#include <vector>
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace gps {
    class SkyBox
    {
    public:
        SkyBox();
        void load(std::vector<const GLchar*> cubeMapFaces);
        void draw(gps::Shader shader, glm::mat4 viewMatrix);
        GLuint getTextureId();
        void setProjection(glm::mat4 projection);
    private:
        GLuint skyboxVAO;
        GLuint skyboxVBO;
        GLuint cubemapTexture;
        GLuint loadSkyBoxTextures(std::vector<const GLchar*> cubeMapFaces);
        void initSkyBox();
        glm::mat4 projection;
    };
}

#endif
