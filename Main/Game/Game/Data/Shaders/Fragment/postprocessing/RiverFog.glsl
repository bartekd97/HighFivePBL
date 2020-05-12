#version 330 core 

const float PI = 3.14159265359;

out vec4 FragColor;
in VS_OUT {
    vec3 FragPos;
    //vec3 FragPosView;
    vec4 LightSpacePos;
    vec2 TexCoords;
    mat3 TBN;
} fs_in;

uniform sampler2D depthMap;
uniform sampler2D noiseMap;

uniform vec2 viewportSize;
uniform float fogZoom = 40.0f;
uniform vec2 noiseOffsets[3];

uniform float fogFadeDist = 0.025f;

float getFogValue(vec2 uv)
{
    vec3 map = vec3(
        texture(noiseMap, uv + noiseOffsets[0]).r,
        texture(noiseMap, uv + noiseOffsets[1]).g,
        texture(noiseMap, uv + noiseOffsets[2]).b
        );

    map = (map * vec3(1.2f)) - vec3(0.2f);
    //float fog = (map.r * 0.5f) + (map.g * 0.5f) + (map.b * 0.5f);
    float fog = (map.r * 0.7f) + (map.g * 0.5f) + (map.b * 0.3f);
    return fog;
}

void main()
{
    float dist = texture(depthMap, gl_FragCoord.xy / viewportSize).r - gl_FragCoord.z;
    float fogDepth = clamp(dist / fogFadeDist, 0.0f, 1.0f);
    //fogDepth = pow(fogDepth, 1.5f);
    fogDepth = sin((-PI * 0.5f) + (fogDepth * PI));
    fogDepth = (fogDepth + 1.0f) * 0.5f;

    float fogValue = getFogValue(fs_in.FragPos.xz / fogZoom);
    float fog = clamp(fogDepth * fogValue, 0.0f, 1.0f);

    FragColor = vec4(vec3(1.0f), fog);
}