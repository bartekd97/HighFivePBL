#version 330 core 

out vec4 FragColor;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D emissiveMap;

uniform vec3 albedoColor;
uniform vec3 emissiveColor;

uniform float specularShininess = 8.0f;
uniform float specularValue;
uniform float opacityValue;

uniform float gGamma = 2.2f;

struct DirectionalLight
{
    vec3 ViewDirection;
    vec3 Color;
    vec3 Ambient;
    float ShadowIntensity;
};
uniform DirectionalLight gDirectionalLight;

in VS_OUT {
    vec3 FragPos;
    vec3 FragPosWorld;
    //vec3 FragPosView;
    vec4 LightSpacePos;
    vec2 TexCoords;
    mat3 TBN;
} fs_in;


float calcDiff(vec3 lightDir, vec3 normal)
{
     return max(dot(normal, lightDir), 0.0);
}
float calcSpec(vec3 lightDir, vec3 normal, vec3 viewDir)
{
    vec3 halfwayDir = normalize(lightDir + viewDir);
    return pow(max(dot(normal, halfwayDir), 0.0), specularShininess);
}

void main()
{
    vec3 normal = texture(normalMap, fs_in.TexCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0);   
    normal = normalize(fs_in.TBN * normal); 

    vec3 albedo = texture(albedoMap, fs_in.TexCoords).rgb * albedoColor;
    vec3 emissive = texture(emissiveMap, fs_in.TexCoords).rgb * emissiveColor;

    vec3 viewDir = normalize(-fs_in.FragPos);

    vec3 lightDir = normalize(-gDirectionalLight.ViewDirection);

    // ambient
    vec3 ambient = gDirectionalLight.Ambient * albedo;
    // diffuse
    vec3 diffuse = calcDiff(lightDir, normal) * gDirectionalLight.Color * albedo;
    // specular
    vec3 specular = calcSpec(lightDir, normal, viewDir) * gDirectionalLight.Color * specularValue;

    vec3 color = ambient + diffuse + specular + emissive;

    // gamma correction
    color = pow(color, vec3(1.0f/gGamma));

    FragColor = vec4(color, opacityValue);
}