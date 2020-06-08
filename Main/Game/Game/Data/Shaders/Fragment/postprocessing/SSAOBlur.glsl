#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D ssaoInput;
uniform sampler2D gTexture;
uniform sampler2D gAlbedoFade;
uniform vec3 ambient;
uniform int debug;

void main() 
{
    vec2 texelSize = 1.0 / vec2(textureSize(ssaoInput, 0));
    float result = 0.0;
    for (int x = -1; x < 1; ++x) 
    {
        for (int y = -1; y < 1; ++y) 
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(ssaoInput, TexCoords + offset).r;
        }
    }
    float fade = texture(gAlbedoFade, TexCoords).a;
    result /= (2.0 * 2.0);
    result = mix(1.0, result, fade);
    if (debug == 0)
    {
        vec3 final = texture(gTexture, TexCoords).rgb * result;
        FragColor = vec4(final, 1.0);
    } else
    {
        FragColor = vec4(vec3(result), 1.0);
    }
}
