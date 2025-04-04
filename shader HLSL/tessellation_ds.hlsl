// Tessellation domain shader
// After tessellation the domain shader processes the all the vertices
Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

//calling this maxlights is misleading since we're using a lot of these for treating a pointlight as 6 spotlights (for shadow purposes)
static const int maxLights = 8;

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    matrix lightViewMatrix[maxLights];
    matrix lightProjectionMatrix[maxLights];
};

cbuffer ConstBuffer : register(b1)
{
    float scale;
    float3 cameraPosition;
    float3 pointLightWorldPos;
    float padding;
}

struct ConstantOutputType
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

struct InputType
{
    float3 position : POSITION;
    float4 colour : COLOR;
    float2 tex : TEXCOORD1;
};

struct OutputType
{
    float4 position : SV_POSITION;
    float4 colour : COLOR;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
    float3 viewVector : TEXCOORD2;
    float4 lightViewPos[maxLights] : TEXCOORD3;
};

float3 CalculateNormals(float2 texCoords)
{
    float offset = 0.005f;
	
    float xpos = texture0.SampleLevel(sampler0, float2(texCoords.x + offset, texCoords.y), 0).r * scale;
    float xneg = texture0.SampleLevel(sampler0, float2(texCoords.x - offset, texCoords.y), 0).r * scale;
    float ypos = texture0.SampleLevel(sampler0, float2(texCoords.x, texCoords.y + offset), 0).r * scale;
    float yneg = texture0.SampleLevel(sampler0, float2(texCoords.x, texCoords.y - offset), 0).r * scale;
	
    float3 tangent = float3(1.0f, (xpos - xneg), 0.0f);
    float3 biTangent = float3(0.0f, (ypos - yneg), 1.0f);
	
    float3 normal = normalize(cross(biTangent, tangent));
	
	
    return normal;
}


float GetHeightDisplacement(float2 texCoords)
{
    float offset = texture0.SampleLevel(sampler0, texCoords, 0).r;
    return offset * scale;
}


[domain("quad")]
OutputType main(ConstantOutputType input, float2 uvwCoord : SV_DomainLocation, const OutputPatch<InputType, 4> patch)
{
    float3 vertexPosition;
    float4 vertexColor;
    float2 vertexTex;
    OutputType output;
 
    // Determine the position of the new vertex.
	// Invert the y and Z components of uvwCoord as these coords are generated in UV space and therefore y is positive downward.
	// Alternatively you can set the output topology of the hull shader to cw instead of ccw (or vice versa).
	//vertexPosition = uvwCoord.x * patch[0].position + -uvwCoord.y * patch[1].position + -uvwCoord.z * patch[2].position;
		
    float3 p1 = lerp(patch[0].position, patch[1].position, uvwCoord.y);
    float3 p2 = lerp(patch[2].position, patch[3].position, uvwCoord.y);
    vertexPosition = lerp(p1, p2, uvwCoord.x);
    
    float4 c1 = lerp(patch[0].colour, patch[1].colour, uvwCoord.y);
    float4 c2 = lerp(patch[2].colour, patch[3].colour, uvwCoord.y);
    vertexColor = lerp(c1, c2, uvwCoord.x);
    
    float2 t1 = lerp(patch[0].tex, patch[1].tex, uvwCoord.y);
    float2 t2 = lerp(patch[2].tex, patch[3].tex, uvwCoord.y);
    vertexTex = lerp(t1, t2, uvwCoord.x);
    
    
    vertexPosition.y += GetHeightDisplacement(vertexTex);
    //output.normal = CalculateNormals(input.tex);
    
    //float3 center = ComputePatchMidPoint(patch[0].position, patch[1].position, patch[2].position, patch[3].position);

    
    // Calculate the position of the new vertex against the world, view, and projection matrices.
    output.position = mul(float4(vertexPosition, 1.0f), worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    for (int i = 0; i < maxLights; i++)
    {
    
        output.lightViewPos[i] = mul(float4(vertexPosition, 1.0f), worldMatrix);
        output.lightViewPos[i] = mul(output.lightViewPos[i], lightViewMatrix[i]);
        output.lightViewPos[i] = mul(output.lightViewPos[i], lightProjectionMatrix[i]);
    }
    

    // Send the input color into the pixel shader.
    output.colour = vertexColor;
    output.tex = vertexTex;
    
    output.normal = CalculateNormals(vertexTex);
    
    output.worldPosition = mul(float4(vertexPosition, 1.0f), worldMatrix);
    
    output.viewVector = output.viewVector = cameraPosition.xyz - output.worldPosition.xyz;
    output.viewVector = normalize(output.viewVector);
    
    

    return output;
}

