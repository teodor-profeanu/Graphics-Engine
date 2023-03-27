#version 460

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

out vec2 textCoord;
out vec3 T;
out vec3 B;
out vec3 N;
out vec3 vPos;
out vec4 fragPosLight[2];

uniform mat4 view;
uniform mat4 projection;
uniform int nrDir;

in DATA {
	in vec2 textCoord;
	in vec3 vNormal;
	in vec3 vPos;
	in vec4 fragPosLight[2];
} data_in[];

void main() {
	vec3 edge0 = gl_in[2].gl_Position.xyz - gl_in[1].gl_Position.xyz;
	vec3 edge1 = gl_in[0].gl_Position.xyz - gl_in[1].gl_Position.xyz;

	vec2 deltaUV0 = data_in[2].textCoord - data_in[1].textCoord;
	vec2 deltaUV1 = data_in[0].textCoord - data_in[1].textCoord;

	float invDet = 1.0f / (deltaUV0.x * deltaUV1.y - deltaUV1.x * deltaUV0.y);
	T = vec3(invDet * (deltaUV1.y * edge0 - deltaUV0.y * edge1));

	gl_Position = projection * view * gl_in[0].gl_Position;
	for(int i=0;i<nrDir;i++)
		fragPosLight[i] = data_in[0].fragPosLight[i];
	N = data_in[0].vNormal;
	B = cross(N, T);
	textCoord = data_in[0].textCoord;
	vPos = data_in[0].vPos;
	EmitVertex();

	gl_Position = projection * view * gl_in[1].gl_Position;
	for(int i=0;i<nrDir;i++)
		fragPosLight[i] = data_in[1].fragPosLight[i];
	N = data_in[1].vNormal;
	B = cross(N, T);
	textCoord = data_in[1].textCoord;
	vPos = data_in[1].vPos;
	EmitVertex();

	gl_Position = projection * view * gl_in[2].gl_Position;
	for(int i=0;i<nrDir;i++)
		fragPosLight[i] = data_in[2].fragPosLight[i];
	N = data_in[2].vNormal;
	B = cross(N, T);
	textCoord = data_in[2].textCoord;
	vPos = data_in[2].vPos;
	EmitVertex();

	EndPrimitive();
}