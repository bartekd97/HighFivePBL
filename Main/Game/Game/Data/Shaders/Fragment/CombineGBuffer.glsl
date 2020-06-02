#version 330 core
const float PI = 3.14159265359;

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gMetalnessRoughnessShadow;
uniform sampler2D gEmissive;

uniform vec3 gCameraPosition;

struct DirectionalLight
{
    vec3 Direction;
    vec3 Color;
    vec3 Ambient;
    float ShadowIntensity;
};
uniform DirectionalLight gDirectionalLight;

uniform float gGamma = 2.2f;

// distance fade
uniform float fadeBelowY = -3.0f;
uniform float fadeRangeY = 7.0f;

// PBR stuff
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);



void main()
{ 
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Albedo = texture(gAlbedo, TexCoords).rgb;
    vec3 MetlnessRoughnessShadow = texture(gMetalnessRoughnessShadow, TexCoords).rgb;
    vec3 Emissive = texture(gEmissive, TexCoords).rgb;

    float metalness = MetlnessRoughnessShadow.r;
    float roughness = MetlnessRoughnessShadow.g;
    float shadow = MetlnessRoughnessShadow.b * gDirectionalLight.ShadowIntensity;


    vec3 viewDir = normalize(gCameraPosition - FragPos);
    vec3 F0 = mix(vec3(0.04), Albedo, metalness);
    vec3 lightDir = normalize(-gDirectionalLight.Direction);
    vec3 Lo = vec3(0.0f);

    // cook-torrance for directional light
    {
        vec3 halfwayDir = normalize(lightDir + viewDir);

        // cook-torrance brdf
        float NDF = DistributionGGX(Normal, halfwayDir, roughness);        
        float G  = GeometrySmith(Normal, viewDir, lightDir, roughness);      
        vec3 F = fresnelSchlick(max(dot(halfwayDir, viewDir), 0.0), F0);  

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metalness;	  

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(Normal, viewDir), 0.0) * max(dot(Normal, lightDir), 0.0);
        vec3 specular = numerator / max(denominator, 0.001);  

        float NdotL = max(dot(Normal, lightDir), 0.0);    

        Lo += ((kD * Albedo / PI + specular) * NdotL) * gDirectionalLight.Color;
    }

    vec3 Ambient = gDirectionalLight.Ambient * Albedo;

    vec3 color = vec3(0.0);

    color += Ambient;
    color += Lo * (1.0f - shadow);
    color += Emissive;

    float fadeY = -(FragPos.y - fadeBelowY);
    fadeY = clamp(1.0f - (fadeY/fadeRangeY), 0.0, 1.0f);

    // tone mappimg
    //color = color / (color + vec3(1.0));
    // gamma correction
    color = pow(color, vec3(1.0f/gGamma));
    
    FragColor = vec4(color, fadeY);
}



float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}  