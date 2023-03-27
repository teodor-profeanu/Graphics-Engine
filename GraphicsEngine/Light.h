#ifndef Light_h
#define Light_h

#include <glm/glm.hpp>
#include "AppUtil.h"
#include "Shader.hpp"
#include "Model3D.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

namespace gps {
	enum LightType {
		DIRECTIONAL, POINT, SPOT
	};

	struct Light {
		static int nrLights;
		static unsigned int defaultCubemap;
		Light(gps::LightType type, glm::vec3 pos, glm::vec3 color, gps::Shader* shader, int resolution);
		void draw(gps::Model3D* objects, glm::mat4 model);

		gps::LightType type;
		unsigned int shadowMapFBO;
		unsigned int shadowMap;
		glm::vec3 pos;
		glm::vec3 color;
		glm::mat4 projection;
		gps::Shader* shader;
		unsigned int shadowMapWidth, shadowMapHeight;
		float side = 20.0f, shadowNear = 0.1f, shadowFar = 40.0f, lightDistance = 20.0f;
		int index;
	};
}

#endif