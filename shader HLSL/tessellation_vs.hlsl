// Tessellation vertex shader.
// Doesn't do much, could manipulate the control points
// Pass forward data, strip out some values not required for example.
Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

struct InputType
{
    float3 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct OutputType
{
    float3 position : POSITION;
    float4 colour : COLOR;
    float2 tex : TEXCOORD0;

};

cbuffer ConstBuffer : register(b1)
{
    float scale;
    float3 cameraPosition;
}

OutputType main(InputType input)
{
    OutputType output;

	 // Pass the vertex position into the hull shader.
    output.position = input.position;
    //output.position.y += sin(input.tex.x *6.0f);
    
    //output.position.y += GetHeightDisplacement(input.tex);

    
    // Pass the input color into the hull shader.
    output.colour = float4(1.0, 0.0, 0.0, 1.0);
    output.tex = input.tex;
    
    return output;
}
