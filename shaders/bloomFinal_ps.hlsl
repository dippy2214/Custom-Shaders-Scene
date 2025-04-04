// Texture and sampler registers
Texture2D texture0 : register(t0);
Texture2D bloomFilter : register(t1);
SamplerState Sampler0 : register(s0);

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};


float4 main(InputType input) : SV_TARGET
{
	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
    float4 textureColor = texture0.Sample(Sampler0, input.tex);
    float4 bloomFilterColor = bloomFilter.Sample(Sampler0, input.tex);
    
    

    return textureColor + bloomFilterColor;
}