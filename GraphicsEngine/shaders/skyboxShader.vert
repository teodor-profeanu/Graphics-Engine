#version 460

layout (location = 0) in vec3 vertexPosition;
out vec3 textureCoordinates;

uniform mat4 projectionView;
uniform mat4 model;

void main()
{
    vec4 tempPos = projectionView * vec4(vertexPosition, 1.0);
    gl_Position = tempPos.xyww;
    textureCoordinates = vertexPosition;
}
