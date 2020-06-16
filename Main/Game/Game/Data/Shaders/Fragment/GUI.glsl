#version 330 core
const float PI = 3.14159265359;
const float PIx2 = 3.14159265359 * 2.0;

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;
uniform vec4 uColor;
uniform float circleFilling;

void main()
{
	vec2 uvNdc = (TexCoords * 2.0f) - vec2(1.0f);
	float angle = atan(uvNdc.x, -uvNdc.y) + PI; // from (-pi,pi) to (0,2pi)

	FragColor = uColor * texture(texture1, TexCoords);
	FragColor.a *= angle <= circleFilling * PIx2 ? 1.0f : 0.0f;
}