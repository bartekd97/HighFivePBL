#version 330 core

in vec2 TexCoords;
out vec4 FragColor;
uniform sampler2D gTexture;
uniform vec3 inverseFilterTextureSize;

uniform float SpanMax;
uniform float ReduceMin;
uniform float ReduceMul;

void main()
{ 
    vec3 luma = vec3(0.299, 0.587, 0.114);  //luminosity vector
    vec2 texCoordOffset = inverseFilterTextureSize.xy;
    //calculate dot(iloczyn skalarny) of luma and pixel with the x shape
    float lumaTL = dot(luma, texture2D(gTexture, TexCoords.xy + (vec2(-1.0, -1.0) * texCoordOffset)).xyz); // top left
    float lumaTR = dot(luma, texture2D(gTexture, TexCoords.xy + (vec2(1.0, -1.0) * texCoordOffset)).xyz);  // top right
    float lumaBL = dot(luma, texture2D(gTexture, TexCoords.xy + (vec2(-1.0, 1.0) * texCoordOffset)).xyz);  // bottom left
    float lumaBR = dot(luma, texture2D(gTexture, TexCoords.xy + (vec2(1.0, 1.0) * texCoordOffset)).xyz);   // bottom right
    float lumaM  = dot(luma, texture2D(gTexture, TexCoords.xy).xyz);  //middle
    
    vec2 blurDir;  // blur direction -- do ogarniecia jeszcze ten kierunek 
    blurDir.x = -((lumaTL + lumaTR) - (lumaBL + lumaBR));
    blurDir.y = ((lumaTL + lumaBL) - (lumaTR + lumaBR));
    
    float dirReduce = max((lumaTL + lumaTR + lumaBL + lumaBR) * (ReduceMul * 0.25), ReduceMin); //to avoid dividing by 0
    float inverseDirAdjustment = 1.0/(min(abs(blurDir.x), abs(blurDir.y)) + dirReduce);

    // clamping????
    blurDir = min(vec2(SpanMax, SpanMax), 
                max(vec2(-SpanMax, -SpanMax), blurDir * inverseDirAdjustment)) * texCoordOffset;

    vec3 result1 = (1.0/2.0) * (texture2D(gTexture, TexCoords.xy + (blurDir * vec2(1.0/3.0 - 0.5))).xyz +
                                texture2D(gTexture, TexCoords.xy + (blurDir * vec2(2.0/3.0 - 0.5))).xyz);
    
    vec3 result2 = result1 * (1.0/2.0) + (1.0/4.0) * (texture2D(gTexture, TexCoords.xy + (blurDir * vec2(0.0/3.0 - 0.5))).xyz +
                                texture2D(gTexture, TexCoords.xy + (blurDir * vec2(3.0/3.0 - 0.5))).xyz);
    
    float lumaMin = min(lumaM, min(min(lumaTL, lumaTR), min(lumaBL, lumaBR)));
    float lumaMax = max(lumaM, max(max(lumaTL, lumaTR), max(lumaBL, lumaBR)));
    
    float lumaResult2 = dot(luma, result2);

    if(lumaResult2 < lumaMin || lumaResult2 > lumaMax)
    {
        FragColor = vec4(result1, 1.0);
    }
    else 
    {
        FragColor = vec4(result2, 1.0);
    }

    
}