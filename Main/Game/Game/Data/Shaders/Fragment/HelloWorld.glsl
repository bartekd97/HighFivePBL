#version 330 core 

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metalnessMap;
uniform sampler2D roughnessMap;
uniform sampler2D emissiveMap;

void main()
{
    FragColor = vec4(vec3(texture2D(metalnessMap, TexCoords).r), 1.0);
}