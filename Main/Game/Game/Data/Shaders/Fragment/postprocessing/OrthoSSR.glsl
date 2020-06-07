#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D gDepth;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gMetalnessRoughness;
uniform sampler2D gScreenTexture;


uniform mat4 gProjection;


const float maxDistance = 7.0;
const float maxSteps = 48.0;
const float thicknessSearch = 0.001;


vec3 ssToUV(vec3 ss) // [-1,1] to [0,1]
{
    return (ss * 0.5f) + vec3(0.5f);
}

void main()
{ 
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec2 MetlnessRoughness = texture(gMetalnessRoughness, TexCoords).rg;

    float metalness = MetlnessRoughness.r;

    if (metalness < 0.001)
        discard;

    vec3 viewDir = vec3(0,0,1);
    vec3 reflected = normalize(reflect(-viewDir, Normal));

    vec3 ssThisPosition = vec3(gProjection * vec4(FragPos, 1.0f));
    vec3 ssNextPosition = vec3(gProjection * vec4(FragPos + (reflected * maxDistance), 1.0f));
    
    vec3 uvThis = vec3(TexCoords, ssToUV(ssThisPosition).z);
    vec3 uvDir = ssToUV(ssNextPosition) - uvThis;
    vec3 uvDirStep = uvDir / maxSteps;


    // ray march
    vec3 uvHit = uvThis;
    vec3 lastUvHit;
    float hit = 0.0f;

    float lastDepthDiff;
    float depthDiff;
    for (int i = 0; i < maxSteps; i++)
    {
        uvHit += uvDirStep;
        lastDepthDiff = depthDiff;
        depthDiff = uvHit.z - texture(gDepth, uvHit.xy).r;
        if (depthDiff > 0 && depthDiff < thicknessSearch)
        {
            hit = 1.0f;
            break;
        }
        else
        {
            lastUvHit = uvHit;
        }
    }

    vec2 dCoords = smoothstep(0.1, 0.5, abs(vec2(0.5, 0.5) - uvHit.xy));
    float screenEdgefactor = clamp(1.0 - pow((dCoords.x + dCoords.y),4), 0.0, 1.0);

    vec3 reflectColor = texture(gScreenTexture, uvHit.xy).rgb;
    float reflectFactor = screenEdgefactor * hit;
    //vec3 resultColor = mix(baseColor, reflectColor, metalness * screenEdgefactor);
    
    FragColor = vec4(reflectColor, reflectFactor);
}