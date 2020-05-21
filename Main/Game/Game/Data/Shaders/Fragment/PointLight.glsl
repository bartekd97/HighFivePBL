#version 330 core
const float PI = 3.14159265359;

layout (location = 4) out vec3 gEmissive;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gMetalnessRoughnessShadow;

uniform vec2 viewportSize;
uniform vec3 gCameraPosition;

struct PointLight
{
    vec3 Position;
    vec3 Color;
    float Radius;
    float Intensity;
    float ShadowIntensity;
};
uniform PointLight gPointLight;



// PBR stuff
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);



void main()
{ 
    vec2 texCoords = gl_FragCoord.xy / viewportSize;

    vec3 FragPos = texture(gPosition, texCoords).rgb;
    vec3 Normal = texture(gNormal, texCoords).rgb;
    vec3 Albedo = texture(gAlbedo, texCoords).rgb;
    vec3 MetlnessRoughnessShadow = texture(gMetalnessRoughnessShadow, texCoords).rgb;

    float metalness = MetlnessRoughnessShadow.r;
    float roughness = MetlnessRoughnessShadow.g;
    float shadow = MetlnessRoughnessShadow.b * gPointLight.ShadowIntensity;


    vec3 viewDir = normalize(gCameraPosition - FragPos);
    vec3 F0 = mix(vec3(0.04), Albedo, metalness);
    vec3 lightDir = normalize(gPointLight.Position - FragPos);
    vec3 Lo = vec3(0.0f);

    float attenuation = clamp(distance(FragPos, gPointLight.Position) / gPointLight.Radius, 0, 1);
    attenuation = sin((PI * 0.5f) - (attenuation * PI));
    attenuation = (attenuation + 1.0f) * 0.5f;

    // cook-torrance for point light
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

        Lo += ((kD * Albedo / PI + specular) * NdotL) * gPointLight.Color * attenuation * gPointLight.Intensity;
    }


    gEmissive = Lo * (1.0f - shadow);
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