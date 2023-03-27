#version 460
layout (location = 0) in vec3 vPos;

uniform mat4 lightProjection;
uniform mat4 model;

void main(){
	gl_Position = lightProjection * model * vec4(vPos, 1.0f);
}