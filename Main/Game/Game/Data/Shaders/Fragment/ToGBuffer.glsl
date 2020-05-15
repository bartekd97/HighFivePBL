#version 330 core 
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedo;
layout (location = 3) out vec3 gMetalnessRoughnessShadow;
layout (location = 4) out vec3 gEmissive;

uniform sampler2DShadow shadowmap;

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
    vec4 LightSpacePos;
    vec2 TexCoords;
    mat3 TBN;
} fs_in;


float calculateShadowFactor()
{
    vec3 projCoords = fs_in.LightSpacePos.xyz / fs_in.LightSpacePos.w;
    projCoords = projCoords * 0.5 + 0.5;
    float bias = 0.001;
    projCoords.z -= bias;

    vec2 offset = vec2(1.0) / textureSize(shadowmap, 0);
    float factor = 0.0;

    for (int y = -1 ; y <= 1 ; y++) {
        for (int x = -1 ; x <= 1 ; x++) {
            vec2 sampleOffset = vec2(x * offset.x, y * offset.y);
            vec3 UVC = vec3(projCoords.xy + sampleOffset, projCoords.z);
            factor += texture(shadowmap, UVC);
        }
    }

    return (1.0f - factor / 9.0f);
}


void main()
{
    vec3 normal = texture(normalMap, fs_in.TexCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0);   
    normal = normalize(fs_in.TBN * normal); 

    float metalness = texture(metalnessMap, fs_in.TexCoords).r * metalnessValue;
    float roughness = texture(roughnessMap, fs_in.TexCoords).r * roughnessValue;
    float shadow = calculateShadowFactor();

    gPosition = fs_in.FragPos;
    //gPositionView = fs_in.FragPosView;
    gNormal = normal;
    gAlbedo = texture(albedoMap, fs_in.TexCoords).rgb * albedoColor;
    gMetalnessRoughnessShadow = vec3(metalness, roughness, shadow);
    gEmissive = texture(emissiveMap, fs_in.TexCoords).rgb * emissiveColor;
    //gAlbedo.r = gl_FragCoord.z;
}