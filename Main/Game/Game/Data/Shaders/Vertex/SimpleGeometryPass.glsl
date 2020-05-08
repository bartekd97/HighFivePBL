#version 330 core  
layout (location = 0) in vec3 aPosition;

uniform mat4 gModel;
uniform mat4 gView;
uniform mat4 gProjection;

void main()  
{
    gl_Position = gProjection * gView * gModel * vec4(aPosition, 1.0f);  
}