#version 330 core  
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
//layout (location = 3) in vec3 aTangent;
//layout (location = 4) in vec3 aBitangent;

const int RANDOM_SIZE = 1024;
const float PI = 3.14159265359;

out VS_OUT {
    vec3 FragPos;
    vec3 FragPosWorld;
    vec4 LightSpacePos;
    vec2 TexCoords;
    vec3 Normal;
} vs_out;


uniform mat4 gModel;
uniform mat4 gView;
uniform mat4 gProjection;

uniform mat4 gLightViewProjection;


layout (std140) uniform gRandomBuffer
{
    vec4 gRandoms[RANDOM_SIZE];
};
uniform vec3 worldPositionBase;
uniform vec4 worldBounds;
uniform float grassDistance;
uniform float grassScale;
uniform int grassDensity;
uniform vec2 randomFactor;
uniform int randomBufferOffset;

uniform float time;

uniform sampler2D grassDensityTexture;
uniform sampler2D simulationTexture;
uniform sampler2D windNoise;


uniform float GWindInfluence = 0.005;
uniform float GWindForce = 0.2;
uniform float GWindSpeed = 0.5;
uniform float NoiseSize = 64;


vec2 rotate2(vec2 v, float a) {
	float s = sin(a);
	float c = cos(a);
	mat2 m = mat2(c, -s, s, c);
	return m * v;
}
float inverseMix(float x, float a, float b)
{
    return (x - a) / (b - a);
}
mat4 rotationMatrix(vec3 axis, float angle) {
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}

vec3 rotate3(vec3 v, vec3 axis, float angle) {
	mat4 m = rotationMatrix(axis, angle);
	return (m * vec4(v, 1.0)).xyz;
}

vec2 GetGlobalWind(vec3 worldPos)
{
    float fx = worldPos.x + time * GWindSpeed;
    float fz = worldPos.z + time * GWindSpeed;

    vec2 v1 = vec2(fx, worldPos.z);
    vec2 v2 = vec2(worldPos.x, fz);

    float n1 = textureLod(windNoise, v1 / NoiseSize, 0.0).r;
    float n2 = textureLod(windNoise, v2 / NoiseSize, 0.0).r;

    n1 = n1 * 2.0 - 1.0;
    n2 = n2 * 2.0 - 1.0;

    return vec2(n1, n2);
}
float GetWindSway(vec2 dir)
{
    return clamp(length(dir) * GWindForce, 0, 1);
}

void main()  
{
    vec3 worldPosition;

    vec4 random = gRandoms[gl_InstanceID + randomBufferOffset];
    vec2 rotatedNormal2 = rotate2(vec2(aNormal.x, aNormal.z), random.w);
    vec3 normal = normalize(vec3(rotatedNormal2.x, aNormal.y, rotatedNormal2.y));

    vec3 objectPosition = worldPositionBase + vec3(
        float(gl_InstanceID % grassDensity) * grassDistance + (grassDistance * 0.5f) + (random.x * randomFactor.x),
        0.0,
        float(gl_InstanceID / grassDensity) * grassDistance + (grassDistance * 0.5f) + (random.y * randomFactor.y)
    );
    vec2 uvTextureObject = vec2(
        clamp(inverseMix(objectPosition.x, worldBounds.x, worldBounds.z), 0, 1),
        1.0 - clamp(inverseMix(objectPosition.z, worldBounds.y, worldBounds.w), 0, 1)
    );
    float densitySample = textureLod(grassDensityTexture, uvTextureObject, 0.0).r;

    if (densitySample > 0.9)
    {
        vec2 rotatedPosition2 = rotate2(vec2(aPosition.x, aPosition.z), random.w);
        vec3 localPosition = vec3(rotatedPosition2.x, aPosition.y, rotatedPosition2.y);
        localPosition *= ((1.0 + random.z * randomFactor.y) * grassScale);

        vec2 uvTextureVertex = vec2(
            clamp(inverseMix(objectPosition.x + localPosition.x, worldBounds.x, worldBounds.z), 0, 1),
            1.0 - clamp(inverseMix(objectPosition.z + localPosition.z, worldBounds.y, worldBounds.w), 0, 1)
        );



        vec2 windDir = GetGlobalWind(localPosition + objectPosition);
        float windSway = GetWindSway(windDir);


        vec4 simulationSample = textureLod(simulationTexture, uvTextureVertex, 0.0);

        simulationSample.w = max(simulationSample.w, windSway);
        simulationSample.xy = mix(simulationSample.xy + windDir * GWindInfluence, simulationSample.xy, smoothstep(0,1,simulationSample.w));

        simulationSample.x += 0.0001; // avoid normalization problem

        float alpha = (simulationSample).w * (PI * 0.5) * pow(aTexCoord.y, 0.25);
        float alphaNormal = (simulationSample).w * (PI * 0.5) * pow(mix(0.1, 1.0, aTexCoord.y), 0.25);

        vec2 dir = normalize(simulationSample.xy);
        localPosition.xz += (dir.xy * sin(alpha) * localPosition.y);
        localPosition.y *= cos(alpha);

        vec3 normalDirAxis = cross(vec3(dir.x, 0.0, dir.y), vec3(0,1,0));
        normal = rotate3(normal, normalDirAxis, alphaNormal);

        worldPosition = localPosition + objectPosition;
    }
    else
    {
        worldPosition = vec3(0,0,0);
    }

    vec4 worldPosition4 = vec4(worldPosition, 1.0);
    gl_Position = gProjection * gView * worldPosition4;

    vs_out.FragPos = vec3(gView * worldPosition4);
    vs_out.FragPosWorld = vec3(worldPosition4);
    vs_out.LightSpacePos = gLightViewProjection * worldPosition4;  
    vs_out.TexCoords = aTexCoord;
    vs_out.Normal = normal;
}