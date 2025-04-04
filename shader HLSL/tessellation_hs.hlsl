// Tessellation Hull Shader
// Prepares control points for tessellation

cbuffer TesselationFactors : register(b0)
{
    float4 tesseleationFactor;
    float2 tessInsides;
    float2 padding;
    float4 camPos;
}

struct InputType
{
    float3 position : POSITION;
    float4 colour : COLOR;
    float2 tex : TEXCOORD0;

};

struct ConstantOutputType
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

struct OutputType
{
    float3 position : POSITION;
    float4 colour : COLOR;
    float2 tex : TEXCOORD0;

};

float4 CalcTessFactor(float3 dir, int currentTessFact)
{
    float4 finalTess = float4(currentTessFact, currentTessFact, currentTessFact, currentTessFact);
    float xtoz = dir.x - dir.z;
    if (abs(xtoz) > 0)
    {
        if (dir.x > 0)
        {
            finalTess[2] = currentTessFact / 2;
        }
        else
        {
            finalTess[0] = currentTessFact / 2;
        }
    }
    if (abs(xtoz < 0)) 
    {
        if (dir.z > 0)
        {
            finalTess[1] = currentTessFact / 2;
        }
        else
        {
            finalTess[3] = currentTessFact / 2;
        }
    }
    
    return finalTess;
}

ConstantOutputType PatchConstantFunction(InputPatch<InputType, 4> inputPatch, uint patchId : SV_PrimitiveID)
{    
    ConstantOutputType output;
    
    float3 dir = inputPatch[0].position.xyz - camPos.xyz;
    float dist = distance(inputPatch[0].position.xyz, camPos.xyz);
    float4 tessFactor;
    float2 tessellationInsides;
    
    if (abs(dist) < 10)
    {
        tessFactor = float4(8, 8, 8, 8);
        tessellationInsides = float2(8, 8);
        if (abs(dist) > 9.7)
        {
            tessFactor = CalcTessFactor(dir, 8);
        }
    }
    else if (abs(dist) < 20)
    {
        tessFactor = float4(4, 4, 4, 4);
        tessellationInsides = float2(4, 4);
        if (abs(dist) > 19.7)
        {
            tessFactor = CalcTessFactor(dir, 4);
        }
    }
    else
    {
        tessFactor = float4(2, 2, 2, 2);
        tessellationInsides = float2(2, 2);
    }

    // Set the tessellation factors for the three edges of the triangle.
    output.edges[0] = tessFactor.x;
    output.edges[1] = tessFactor.y;
    output.edges[2] = tessFactor.z;
    output.edges[3] = tessFactor.w;
    //output.edges[3] = tesseleationFactor.w;

    // Set the tessellation factor for tessallating inside the triangle.
    output.inside[0] = tessellationInsides.x;
    output.inside[1] = tessellationInsides.y;

    return output;
}


[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(4)]
[patchconstantfunc("PatchConstantFunction")]
OutputType main(InputPatch<InputType, 4> patch, uint pointId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    OutputType output;

    // Set the position for this control point as the output position.
    output.position = patch[pointId].position;

    // Set the input colour as the output colour.
    output.colour = patch[pointId].colour;
    
    output.tex = patch[pointId].tex;
    

    return output;
}