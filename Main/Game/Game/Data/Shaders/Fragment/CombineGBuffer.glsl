#version 330 core
const float PI = 3.14159265359;

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gMetalnessRoughness;
uniform sampler2D gEmissive;

uniform float gGamma = 2.2f;

void main()
{ 
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Albedo = texture(gAlbedo, TexCoords).rgb;
    vec2 MetlnessRoughness = texture(gMetalnessRoughness, TexCoords).rg;
    vec3 Emissive = texture(gEmissive, TexCoords).rgb;

    float metalness = MetlnessRoughness.r;
    float roughness = MetlnessRoughness.g;


    vec3 color = vec3(0.0);

    color += Albedo;
    color += Emissive;

    // tone mappimg
    //color = color / (color + vec3(1.0));
    // gamma correction
    color = pow(color, vec3(1.0f/gGamma));
    
    FragColor = vec4(color, 1.0f);
}