#version 330 core 

out vec4 FragColor;

in VS_OUT {
    vec2 TexCoords;
    flat float Lifetime;
} fs_in;

uniform sampler2D gSpriteSheet;
uniform sampler2D gColor;
uniform sampler2D gOpacity;


void main()
{
    if (fs_in.Lifetime >= 0.997f)
        discard;

    vec4 tex = texture2D(gSpriteSheet, fs_in.TexCoords);
    vec2 ltPos = vec2(fs_in.Lifetime, 0.0f);

    vec3 color = tex.rgb * texture2D(gColor, ltPos).rgb;
    float opacity = tex.a * texture2D(gOpacity, ltPos).r;

    FragColor = vec4(color, opacity);
}