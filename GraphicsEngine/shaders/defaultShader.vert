#version 460

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 textcoord;

out DATA {
	out vec2 textCoord;
	out vec3 vNormal;
	out vec3 vPos;
	out vec4 fragPosLight[2];
} data_out;

uniform mat4 model;
uniform mat4 lightProjection[2];
uniform int nrDir;

void main() {
    data_out.vPos = vec3(model * vec4(vertexPosition, 1.0f));
	data_out.textCoord = textcoord;
    data_out.vNormal = vec3(model * vec4(vertexNormal, 0.0f));
	for(int i=0;i<nrDir;i++)
		data_out.fragPosLight[i] = lightProjection[i] * model * vec4(vertexPosition, 1.0f);
    gl_Position = model * vec4(vertexPosition, 1.0);
}
