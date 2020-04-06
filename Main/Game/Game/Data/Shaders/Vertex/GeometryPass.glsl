#version 330 core  
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

uniform mat4 gModel;
uniform mat4 gView;
uniform mat4 gProjection;

out VS_OUT {
    vec3 FragPos;
    //vec3 FragPosView;
    vec2 TexCoords;
    mat3 TBN;
} vs_out;

void main()  
{
    gl_Position = gProjection * gView * gModel * vec4(aPosition, 1.0f);  

    vs_out.FragPos = vec3(gModel * vec4(aPosition, 1.0));
    //vs_out.FragPosView = vec3(gView * vec4(vs_out.FragPos, 1.0));
    vs_out.TexCoords = aTexCoord;

    vec3 T = normalize(vec3(gModel * vec4(aTangent, 0.0)));
    vec3 B = normalize(vec3(gModel * vec4(aBitangent, 0.0)));
    vec3 N = normalize(vec3(gModel * vec4(aNormal, 0.0)));
    vs_out.TBN = mat3(T, B, N);
}