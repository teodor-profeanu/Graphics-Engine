#version 460
layout (location = 0) in vec3 position;
layout(location = 2) in vec2 texCoords;

out vec2 TexCoords;
uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(position, 1.0f);
    TexCoords = texCoords;
}