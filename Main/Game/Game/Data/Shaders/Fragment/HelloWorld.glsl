#version 330 core 

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D sampleMap;

void main()
{
    FragColor = vec4(texture2D(sampleMap, TexCoords).rgb, 1.0);
}