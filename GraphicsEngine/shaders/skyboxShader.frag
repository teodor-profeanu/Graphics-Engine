#version 460

in vec3 textureCoordinates;
out vec4 color;

uniform samplerCube skybox;

void main()
{
	color = texture(skybox, textureCoordinates);
	//color = vec4(texture(skybox, textureCoordinates).r, texture(skybox, textureCoordinates).r, texture(skybox, textureCoordinates).r, 1.0f);
	//color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
}
