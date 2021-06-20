#version 330 core 
out vec4 FragColor;

const float PI = 3.14159265359;

in vec2 TexCoords;

uniform vec2 direction;
uniform float targetHeight;
uniform float innerRadius;
uniform float ring;
uniform float innerRing;
uniform int directionMode;

uniform float _G_GrassHeight = 1.0;

void main()
{
    vec2 uvNdc = (TexCoords.xy * 2.0f) - vec2(1.0f, 1.0f);
    float dist = length(uvNdc);

    vec2 finalDirection;
    if (directionMode == 0)
    {
        finalDirection = direction;
    }
    else if (directionMode == 1)
    {
        //float angle = atan(uvNdc.x, -uvNdc.y) + PI;
        //finalDirection = vec2(sin(angle), cos(angle));
        finalDirection = normalize(uvNdc);
    }

    float strength = 0.0;
    if (dist <= 1.0 && dist > innerRadius)
    {
        strength = 1.0 - smoothstep(innerRadius, 1.0f, dist);
    }
    else if (dist <= innerRadius && dist > ring)
    {
        strength = 1.0;
    }
    else if (dist <= ring && dist > innerRing)
    {
        strength = smoothstep(innerRing, ring, dist);
    }

    float realTargetHeight = mix(_G_GrassHeight, targetHeight, strength);

    float swayLevel = 1.0 - clamp(realTargetHeight / _G_GrassHeight, 0, 1);
    swayLevel = sin(swayLevel * 1.57079632679);

    vec4 col = vec4(finalDirection.x, finalDirection.y, 0, swayLevel);

    FragColor = col;
}