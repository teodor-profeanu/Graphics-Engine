#include "Light.h"

namespace gps {
	int Light::nrLights = 0;
	unsigned int Light::defaultCubemap = -1;
	Light::Light(gps::LightType type, glm::vec3 pos, glm::vec3 color, gps::Shader* shader, int resolution) {
		if (defaultCubemap == -1) {
			glGenTextures(1, &defaultCubemap);
		}

		this->type = type;
		this->pos = pos;
		this->color = color;
		this->shader = shader;
		this->index = nrLights;
		nrLights++;
		this->shadowMapHeight = resolution;
		this->shadowMapWidth = resolution;

		if (type == gps::DIRECTIONAL) {
			glGenFramebuffers(1, &shadowMapFBO);

			glGenTextures(1, &shadowMap);
			glBindTexture(GL_TEXTURE_2D, shadowMap);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			float clampColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clampColor);

			glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			glm::mat4 orthoProjection = glm::ortho(-side, side, -side, side, shadowNear, shadowFar);
			glm::mat4 lightView = glm::lookAt(lightDistance * pos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			projection = orthoProjection * lightView;
		}
		else if (type == gps::POINT) {
			glGenFramebuffers(1, &shadowMapFBO);
			glGenTextures(1, &shadowMap);
			glBindTexture(GL_TEXTURE_CUBE_MAP, shadowMap);
			for (int i = 0; i < 6; i++)
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

			glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowMap, 0);
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			shader->useShaderProgram();
			glUniform1f(glGetUniformLocation(shader->shaderProgram, "farPlane"), shadowFar);
		}
	}

	void Light::draw(gps::Model3D* objects, glm::mat4 model) {
		shader->useShaderProgram();
		glViewport(0, 0, shadowMapWidth, shadowMapHeight);
		glUniformMatrix4fv(glGetUniformLocation(shader->shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
		if (type == gps::DIRECTIONAL) {
			glUniformMatrix4fv(glGetUniformLocation(shader->shaderProgram, "lightProjection"), 1, GL_FALSE, glm::value_ptr(projection));

			glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
			glClear(GL_DEPTH_BUFFER_BIT);
			objects->draw(*shader);
			glActiveTexture(GL_TEXTURE0 + index);
			glBindTexture(GL_TEXTURE_2D, shadowMap);
		}
		else if (type == gps::POINT) {
			glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, shadowFar);
			glm::mat4 shadowMatrices[] = {
				shadowProj * glm::lookAt(pos, pos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
				shadowProj * glm::lookAt(pos, pos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
				shadowProj * glm::lookAt(pos, pos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
				shadowProj * glm::lookAt(pos, pos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
				shadowProj * glm::lookAt(pos, pos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
				shadowProj * glm::lookAt(pos, pos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
			};

			glUniformMatrix4fv(glGetUniformLocation(shader->shaderProgram, "shadowMatrices"), 6, GL_FALSE, glm::value_ptr(shadowMatrices[0]));
			glUniform3fv(glGetUniformLocation(shader->shaderProgram, "lightPos"), 1, glm::value_ptr(pos));

			glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
			glClear(GL_DEPTH_BUFFER_BIT);
			objects->draw(*shader);
			glActiveTexture(GL_TEXTURE0 + index);
			glBindTexture(GL_TEXTURE_CUBE_MAP, shadowMap);
		}
	}
}