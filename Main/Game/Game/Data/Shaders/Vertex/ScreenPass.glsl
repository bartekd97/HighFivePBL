#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    gl_Position = vec4(aPosition.x, aPosition.y, 0.0, 1.0); 
    TexCoords = aTexCoords;
}  