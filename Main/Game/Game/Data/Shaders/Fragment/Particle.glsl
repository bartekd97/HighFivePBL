#version 330 core 

out vec4 FragColor;

in VS_OUT {
    vec2 TexCoords;
    float Opacity;
    vec3 Color;
} fs_in;


void main()
{
    FragColor = vec4(fs_in.Color, fs_in.Opacity);
}