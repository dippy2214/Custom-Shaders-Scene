Texture2D texture0 : register(t0);
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
    if ((textureColor.r * 0.299f) + (textureColor.g * 0.587f) + (textureColor.b * 0.0722) < 0.7f)
    {
        return float4(0, 0, 0, 1);
    }

    return float4(1, 1, 1, 1);
}