Texture2D heightmap : register(t0);
SamplerState sampler0 : register(s0);


cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer ConstBuffer : register(b1)
{
    float scale;
    float3 padding;
};

struct InputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct OutputType
{
    float4 position : SV_POSITION;
    float4 depthPosition : TEXCOORD0;
    float4 worldPosition : TEXCOORD1;
};

float GetHeightDisplacement(float2 texCoords)
{
    float offset = heightmap.SampleLevel(sampler0, texCoords, 0).r;
    return offset * scale;
}

OutputType main(InputType input)
{
    OutputType output;

    float4 pos = input.position;
    pos.y += GetHeightDisplacement(input.tex);
    
    output.position = mul(pos, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // Store the position value in a second input value for depth value calculations.
    output.depthPosition = output.position;
    
    output.worldPosition = mul(pos, worldMatrix);
	
    return output;
}