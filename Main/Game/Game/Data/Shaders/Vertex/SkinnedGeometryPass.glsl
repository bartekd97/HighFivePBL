#version 330 core  
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;
layout (location = 5) in ivec4 aBoneIDs;
layout (location = 6) in vec4 aWeights;

const int MAX_BONES = 100;

uniform mat4 gModel;
uniform mat4 gView;
uniform mat4 gProjection;
layout (std140) uniform gBonesBuffer
{
    mat4 gBones[MAX_BONES];
};

uniform mat4 gLightViewProjection;

out VS_OUT {
    vec3 FragPos;
    vec3 FragPosWorld;
    //vec3 FragPosView;
    vec4 LightSpacePos;
    vec2 TexCoords;
    mat3 TBN;
} vs_out;

void main()  
{
    mat4 BoneTransform = gBones[aBoneIDs[0]] * aWeights[0];
    BoneTransform += gBones[aBoneIDs[1]] * aWeights[1];
    BoneTransform += gBones[aBoneIDs[2]] * aWeights[2];
    BoneTransform += gBones[aBoneIDs[3]] * aWeights[3];

    vec4 worldBonedPosition = gModel * BoneTransform * vec4(aPosition, 1.0f);
    gl_Position = gProjection * gView * worldBonedPosition;  

    vs_out.FragPos = vec3(gView * worldBonedPosition);
    vs_out.FragPosWorld = vec3(worldBonedPosition);
    //vs_out.FragPosView = vec3(gView * vec4(vs_out.FragPos, 1.0));
    vs_out.LightSpacePos = gLightViewProjection * worldBonedPosition;  
    vs_out.TexCoords = aTexCoord;

    mat4 gVMBT = gView * gModel * BoneTransform;
    vec3 T = normalize(vec3(gVMBT * vec4(aTangent, 0.0)));
    vec3 B = normalize(vec3(gVMBT * vec4(aBitangent, 0.0)));
    vec3 N = normalize(vec3(gVMBT * vec4(aNormal, 0.0)));
    vs_out.TBN = mat3(T, B, N);
}