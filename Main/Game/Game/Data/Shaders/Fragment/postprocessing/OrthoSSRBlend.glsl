#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gMetalnessRoughness;
uniform sampler2D gSSRTexture;

const float roughnessMaxLod = 4.0f;

// distance fade
uniform float fadeBelowY = -3.0f;
uniform float fadeRangeY = 7.0f;

void main()
{ 
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec2 MetlnessRoughness = texture(gMetalnessRoughness, TexCoords).rg;

    float metalness = MetlnessRoughness.r;
    float roughness = MetlnessRoughness.g;

    vec4 ssr = textureLod(gSSRTexture, TexCoords, roughness * roughnessMaxLod).rgba;

    float fadeY = -(FragPos.y - fadeBelowY);
    fadeY = clamp(1.0f - (fadeY/fadeRangeY), 0.0, 1.0f);

    FragColor = vec4(ssr.rgb, metalness * ssr.a * fadeY);
}