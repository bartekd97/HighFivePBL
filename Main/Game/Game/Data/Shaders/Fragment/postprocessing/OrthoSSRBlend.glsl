#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D gAlbedoFade;
uniform sampler2D gMetalnessRoughness;
uniform sampler2D gSSRTexture;

const float roughnessMaxLod = 4.0f;


void main()
{ 
    vec2 MetlnessRoughness = texture(gMetalnessRoughness, TexCoords).rg;

    float metalness = MetlnessRoughness.r;
    float roughness = MetlnessRoughness.g;

    vec4 ssr = textureLod(gSSRTexture, TexCoords, roughness * roughnessMaxLod).rgba;

    float fade = texture(gAlbedoFade, TexCoords).a;

    FragColor = vec4(ssr.rgb, metalness * ssr.a * fade);
}