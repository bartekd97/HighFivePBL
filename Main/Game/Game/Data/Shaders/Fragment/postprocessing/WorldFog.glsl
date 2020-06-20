#version 330 core 

const float PI = 3.14159265359;

out vec4 FragColor;
in VS_OUT {
    vec3 FragPos;
    vec3 FragPosWorld;
    //vec3 FragPosView;
    vec4 LightSpacePos;
    vec2 TexCoords;
    mat3 TBN;
} fs_in;

uniform sampler2D depthMap;
uniform sampler2D noiseMap;
//uniform sampler2D gPosition;

uniform vec2 viewportSize;
uniform float fogZoom = 30.0f;
uniform vec2 noiseOffsets[3];

uniform vec3 fogColor = vec3(0.1, 0.27, 0.35);
uniform float fogFadeDist = 0.024f;
uniform float fogMaxDist = 0.027f;
uniform float fogMaxFadeDist = 0.018f;
uniform float fogOpacityFactor = 0.4f;

float getFogValue(vec2 uv)
{
    vec3 map = vec3(
        texture(noiseMap, uv + noiseOffsets[0]).r,
        texture(noiseMap, uv + noiseOffsets[1]).g,
        texture(noiseMap, uv + noiseOffsets[2]).b
        );

    map = (map * vec3(1.0f)) - vec3(0.0f);
    //float fog = (map.r * 0.5f) + (map.g * 0.5f) + (map.b * 0.5f);
    float fog = (map.r * 0.7f) + (map.g * 0.5f) + (map.b * 0.3f);
    return fog;
}

void main()
{
    float dist = texture(depthMap, gl_FragCoord.xy / viewportSize).r - gl_FragCoord.z;
    float fogDepth = 1.0f;
    if (dist < fogFadeDist)
        fogDepth = clamp(dist / fogFadeDist, 0.0f, 1.0f);
    else if (dist > fogMaxDist)
        fogDepth = 1.0f - clamp((dist - fogMaxDist) / fogMaxFadeDist, 0.0f, 1.0f);

    //fogDepth = pow(fogDepth, 1.5f);
    fogDepth = sin((-PI * 0.5f) + (fogDepth * PI));
    fogDepth = (fogDepth + 1.0f) * 0.5f;

    float fogValue = getFogValue(fs_in.FragPosWorld.xz / fogZoom);
    float fog = clamp(fogDepth * fogValue, 0.0f, 1.0f);

    FragColor = vec4(fogColor, fog * fogOpacityFactor);
}