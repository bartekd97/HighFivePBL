#version 330 core 
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedo;
layout (location = 3) out vec2 gMetalnessRoughness;
layout (location = 4) out vec3 gEmissive;

uniform sampler2D albedoMap;
uniform sampler2D roughnessMap;
uniform sampler2D metalnessMap;
uniform sampler2D normalMap;
uniform sampler2D emissiveMap;

uniform vec3 albedoColor;
uniform float roughnessValue;
uniform float metalnessValue;
uniform vec3 emissiveColor;



in VS_OUT {
    vec3 FragPos;
    //vec3 FragPosView;
    vec2 TexCoords;
    mat3 TBN;
} fs_in;


void main()
{
    vec3 normal = texture(normalMap, fs_in.TexCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0);   
    normal = normalize(fs_in.TBN * normal); 

    float metalness = texture(metalnessMap, fs_in.TexCoords).r * metalnessValue;
    float roughness = texture(roughnessMap, fs_in.TexCoords).r * roughnessValue;

    gPosition = fs_in.FragPos;
    //gPositionView = fs_in.FragPosView;
    gNormal = normal;
    gAlbedo = texture(albedoMap, fs_in.TexCoords).rgb * albedoColor;
    gMetalnessRoughness = vec2(metalness, roughness);
    gEmissive = texture(emissiveMap, fs_in.TexCoords).rgb * emissiveColor;
    //gAlbedo.r = gl_FragCoord.z;
}