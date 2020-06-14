#version 330 core
out vec4 BrightColor;

in  vec2 TexCoords;

uniform sampler2D DiffuseTexture;

uniform float BrightnessThreshold;

void main()
{           
    vec4 color = texture(DiffuseTexture, TexCoords);
    // float brightness = (color.r * 0.2126) + (color.g * 0.7162) + (color.b * 0.0722);
    // //FragColor = color;
    // BrightColor = color * brightness;

    float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
        if(brightness > BrightnessThreshold)
            BrightColor = vec4(color.rgb, 1.0);
        else
            BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}