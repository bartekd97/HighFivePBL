#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexCoords;

out VS_OUT {
    vec2 TexCoords;
    flat float Lifetime;
} vs_out;

const int MAX_PARTICLES = 256;

uniform mat4 gView;
uniform mat4 gProjection;
uniform vec3 gCameraRight;
uniform vec3 gCameraUp;

uniform int gSpriteSheetCount = 1;

struct Particle
{
    vec4 Position; // xyz world pos, w size
    vec4 Velocity; // xyz direction, w speed
    vec4 Lifetime; // x current, y max, zw padding
};
layout (std140) uniform gParticlesBuffer
{
    Particle gParticles[MAX_PARTICLES];
};

void main()
{
    Particle particle = gParticles[gl_InstanceID];
    vec3 aPositionBillboard = particle.Position.xyz
        + gCameraRight * aPosition.x * particle.Position.w
        + gCameraUp * aPosition.y * particle.Position.w;

    gl_Position = gProjection * gView * vec4(aPositionBillboard, 1.0f);

    float textureUVPart = 1.0f / float(gSpriteSheetCount);
    float currentUVOffset = textureUVPart * (gl_InstanceID % gSpriteSheetCount);

    vs_out.TexCoords = aTexCoords;
    vs_out.TexCoords.x *= textureUVPart;
    vs_out.TexCoords.x += currentUVOffset;
    vs_out.Lifetime = clamp(particle.Lifetime.x / particle.Lifetime.y, 0.0f, 1.0f);
}  