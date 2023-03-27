#version 460
in vec4 fragPos;

uniform vec3 lightPos;
uniform float farPlane;

void main(){
	gl_FragDepth = length(fragPos.xyz - lightPos) / farPlane;
}