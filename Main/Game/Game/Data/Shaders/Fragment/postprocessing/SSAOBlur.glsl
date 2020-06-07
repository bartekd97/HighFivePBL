#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D ssaoInput;
uniform sampler2D gTexture;
uniform vec3 ambient;
uniform int debug;

void main() 
{
    vec2 texelSize = 1.0 / vec2(textureSize(ssaoInput, 0));
    vec3 result = vec3(0.0);
    for (int x = -1; x < 1; ++x) 
    {
        for (int y = -1; y < 1; ++y) 
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(ssaoInput, TexCoords + offset).rgb;
        }
    }
    //vec3 final = texture(gTexture, TexCoords).rgb;
    //final += final * (result / (4.0 * 4.0)) * 0.3;
    //FragColor = vec4(final, 1.0);// * (result / (4.0 * 4.0) * 0.3);
    if (debug == 0)
    {
        vec3 ambientDiff = (result / (2.0 * 2.0)) - ambient;
        vec3 final = texture(gTexture, TexCoords).rgb * ambientDiff;
        FragColor = vec4(final, 1.0);
    } else
    {
        vec3 xD = vec3(result / (2.0 * 2.0));
        FragColor = vec4(xD, 1.0);
    }
}
