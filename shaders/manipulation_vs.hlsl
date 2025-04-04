// Light vertex shader
// Standard issue vertex shader, apply matrices, pass info to pixel shader
Texture2D texture0 : register(t0);
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
    float3 cameraPosition;
}

struct InputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	//float2 tex2 : TEXCOORD1;
};

struct OutputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
    //float2 tex2 : TEXCOORD1;
	float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
    float3 viewVector : TEXCOORD2;
};

float GetHeightDisplacement(float2 texCoords)
{
    float offset = texture0.SampleLevel(sampler0, texCoords, 0).r;
    return offset * scale;
}

float3 CalculateNormals(float2 texCoords)
{
    float offset = 0.01f;
	
    float xpos = texture0.SampleLevel(sampler0, float2(texCoords.x + offset, texCoords.y), 0).r;
    float xneg = texture0.SampleLevel(sampler0, float2(texCoords.x - offset, texCoords.y), 0).r;
    float ypos = texture0.SampleLevel(sampler0, float2(texCoords.x, texCoords.y + offset), 0).r;
    float yneg = texture0.SampleLevel(sampler0, float2(texCoords.x, texCoords.y - offset), 0).r;
	
    float3 tangent = float3(1.0f,(xpos - xneg) * scale, 0.0f);
    float3 biTangent = float3(0.0f,(ypos - yneg) * scale,  1.0f);
	
    float3 normal = normalize(cross(biTangent, tangent));
	
	
    return normal;
}

OutputType main(InputType input)
{
    //float4 pos = texture0.Sample(sampler0, input.tex);
	
	OutputType output;
    output.position = input.position;
    //output.position = output.position + float4(input.normal, 0.0f) * (sin(time * speed + output.position.x * frequency) * amplitude+2);
	//(sin(time * speed + output.position.x * frequency) * amplitude) + (cos(time * speed + output.position.z * frequency) * amplitude);
	// Calculate the position of the vertex against the world, view, and projection matrices.
	
    output.position.y += GetHeightDisplacement(input.tex);
	
	//output.position = mul(output.position, worldMatrix);
	//output.position = mul(output.position, viewMatrix);
	//output.position = mul(output.position, projectionMatrix);
	
    

	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;
    //output.tex2 = input.tex2;

	// Calculate the normal vector against the world matrix only and normalise.
    //output.normal = input.normal;
    //output.normal += float4(input.normal, 0.0f) * -(amplitude * 2) * sin(time * speed + output.position.x * frequency);
	
	//output.normal = mul(input.normal, (float3x3)worldMatrix);
	//output.normal = normalize(output.normal);

    output.normal = CalculateNormals(input.tex);
		
    float4 worldPosition = mul(input.position, worldMatrix);
    //worldPosition.y += GetHeightDisplacement(scale);
	output.worldPosition = worldPosition.xyz;
    output.viewVector = cameraPosition.xyz - worldPosition.xyz;
    output.viewVector = normalize(output.viewVector);
	
	return output;
}