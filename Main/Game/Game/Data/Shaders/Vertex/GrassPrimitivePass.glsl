#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

uniform vec4 rectBounds;

void main()
{
    float px = aPosition.x * 0.5 + 0.5;
    float py = aPosition.y * 0.5 + 0.5;

    px = mix(rectBounds.x, rectBounds.z, px);
    py = mix(rectBounds.y, rectBounds.w, py);

    px = px * 2.0 - 1.0;
    py = py * 2.0 - 1.0;

    gl_Position = vec4(px, py, 0.0, 1.0); 
    TexCoords = aTexCoords;
}  