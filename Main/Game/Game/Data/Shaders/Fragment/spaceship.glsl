#version 330 core 

out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 FragPosView;
    vec2 TexCoords;
    //mat3 TBN;
} fs_in;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metalnessMap;
uniform sampler2D roughnessMap;
uniform sampler2D emissiveMap;

void main()
{
    vec3 albedo = texture2D(albedoMap, fs_in.TexCoords).rgb;
    vec3 emiss = texture2D(emissiveMap, fs_in.TexCoords).rgb;
    FragColor = vec4(albedo + emiss, 1.0);
}