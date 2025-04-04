cbuffer lightPosBuffer
{
    float3 lightPos;
    float padding;
};

struct InputType
{
    float4 position : SV_POSITION;
    float4 depthPosition : TEXCOORD0;
    float4 worldPosition : TEXCOORD1;
};

float4 main(InputType input) : SV_TARGET
{
    float3 depthValue;
	// Get the depth value of the pixel by dividing the Z pixel depth by the homogeneous W coordinate.
    depthValue = (lightPos - input.worldPosition.xyz);
    return float4(depthValue, 1);
}