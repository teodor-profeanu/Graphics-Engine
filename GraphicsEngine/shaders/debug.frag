#version 460
out vec4 FragColor;
in  vec2 TexCoords;
  
uniform sampler2D fboAttachment;
  
void main()
{
    FragColor = vec4(texture(fboAttachment, TexCoords).r, texture(fboAttachment, TexCoords).r, texture(fboAttachment, TexCoords).r, 1.0f);
    //FragColor = glm::vec4(1.0f);
} 
