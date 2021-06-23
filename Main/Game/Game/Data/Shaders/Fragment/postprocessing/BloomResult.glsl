#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D scene;
uniform sampler2D bloomBlur;

uniform float gGamma = 2.2f;

void main()
{             
    vec3 hdrColor = texture(scene, TexCoords).rgb;      
    vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;

    hdrColor = pow(hdrColor, vec3(gGamma));
    //bloomColor = pow(bloomColor, vec3(gGamma));

    hdrColor += bloomColor; // additive blending
   
    hdrColor = pow(hdrColor, vec3(1.0f/gGamma));

    FragColor = vec4(hdrColor, 1.0);
}
