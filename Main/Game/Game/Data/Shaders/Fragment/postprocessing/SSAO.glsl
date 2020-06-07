#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

uniform vec3 samples[64];

int kernelSize = 64;
float radius = 0.4;
float bias = 0.005;
float power = 1.45;

const vec2 noiseScale = vec2(1280.0/4.0, 720.0/4.0); 

uniform mat4 projectionView;

void main()
{
	vec3 fragPos = texture(gPosition, TexCoords).xyz;
    vec3 normal = normalize(texture(gNormal, TexCoords).rgb);
    vec3 randomVec = normalize(texture(texNoise, TexCoords * noiseScale).xyz);

    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;
    for(int i = 0; i < kernelSize; ++i)
    {
        vec3 sample = TBN * samples[i]; // from tangent to view-space
        sample = fragPos + sample * radius; 
        
        vec4 offset = vec4(sample, 1.0);
        offset = projectionView * offset; // from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0
        
        float sampleDepth = texture(gPosition, offset.xy).z; // get depth value of kernel sample
        
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= sample.z + bias ? 1.0 : 0.0) * rangeCheck;
    }
    occlusion = 1.0 - (occlusion / kernelSize);
    
    //vec4 final = texture(gAlbedo, TexCoords);
    //final += final * 0.3 * occlusion;
    //FragColor = texture(gAlbedo, TexCoords) * 0.3 * occlusion;//occlusion;
    //FragColor = vec4(occlusion, 0, 0, 1.0);
    FragColor = vec4(vec3(pow(occlusion, power)), 1.0);
}
