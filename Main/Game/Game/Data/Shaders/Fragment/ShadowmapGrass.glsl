#version 330 core 

uniform sampler2D albedoMap;

in VS_OUT {
    vec3 FragPos;
    vec3 FragPosWorld;
    vec4 LightSpacePos;
    vec2 TexCoords;
    vec3 Normal;
} fs_in;

uniform float alphaCutoff = 0.5f;

void main()
{
    vec4 albedo = texture(albedoMap, fs_in.TexCoords).rgba;

    if (albedo.a < alphaCutoff)
        discard;
}