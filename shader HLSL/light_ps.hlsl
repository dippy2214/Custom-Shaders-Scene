// Light pixel shader
// Calculate diffuse lighting for a single directional light (also texturing)
#define MAXLIGHTS 8

#define POINTLIGHTTEXNUM 6

Texture2D texture0 : register(t0);
Texture2D depthMapTexture : register(t1);
Texture2D<float4> pointLightTexture[POINTLIGHTTEXNUM] : register(t2);


SamplerState sampler0 : register(s0);
SamplerState shadowSampler : register(s1);


cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    matrix lightViewMatrix[MAXLIGHTS];
    matrix lightProjectionMatrix[MAXLIGHTS];
};

cbuffer LightBuffer : register(b1)
{
    //rgba
    float4 ambient[MAXLIGHTS];
    
    //rgba
    float4 diffuse[MAXLIGHTS];
    
    //xyz (last value stores specular power)
    float4 position[MAXLIGHTS];
    
    //rgba
    float4 specular[MAXLIGHTS];
    
    //xya (last value not used)
    float4 attenuation[MAXLIGHTS];
    
    //xyz (last value stores spotlight cone angle)
    float4 lightDirection[MAXLIGHTS];
    
    //only x value used
    float4 lightType[MAXLIGHTS];
};


struct InputType
{
	float4 position : SV_POSITION;
    float4 colour : COLOR;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 worldPosition : TEXCOORD1;
    float3 viewVector : TEXCOORD2;
    float4 lightViewPos[MAXLIGHTS] : TEXCOORD3;
};

float4 calculateDirectionalLighting(float3 lightDirection, float3 normal, float4 ldiffuse)
{
    float intensity = normalize(dot(normal, lightDirection));
    float4 colour = normalize(ldiffuse * intensity);
    return colour;
}
// Calculate lighting intensity based on direction and normal. Combine with light colour.
float4 calculateLighting(float3 lightDirection, float3 normal, float4 ldiffuse, float distance, int index)
{
	float intensity = saturate(dot(normal, lightDirection));
    float atten = 1.0f / (attenuation[index].x + (attenuation[index].y * distance) + (attenuation[index].z * pow(distance, 2)));
	float4 colour = saturate(ldiffuse * atten * intensity);
	return colour;
}

float4 calculateSpecular(float3 lightDirection, float3 normal, float3 viewVector, float4 specularColor, float specularPower, int index)
{
    if (specularPower > 0)
    {
        float3 halfway = normalize(lightDirection + viewVector);
        float specularIntensity = pow(max(dot(normal, halfway), 0.0), specularPower);
        float4 finalVal = saturate(specularColor * specularIntensity);
        return finalVal;
    }
    return float4(0, 0, 0, 0);
}

float4 calculateSpotSpecular(float3 lightDir, float3 normal, float3 viewVector, float4 specularColor, float specularPower, int index, float4 lightPos, float3 pixelPos)
{
    if (specularPower > 0)
    {
        float3 halfway = normalize(lightDir + viewVector);
        float specularIntensity = pow(max(dot(normal, halfway), 0.0), specularPower);
        float4 finalVal = saturate(specularColor * specularIntensity);
    
        float3 direction = normalize(float3(lightPos.x, lightPos.y, lightPos.z) - pixelPos);
        finalVal *= pow(max(dot(-direction, normalize(lightDirection[index].xyz)), 0.0f), lightDirection[index][3]);

        return finalVal;
    }
    return float4(0, 0, 0, 0);
}

float4 calculateSpotLighting(float3 normal, float3 attenuation, float4 lightPos, float3 pixelPos, int index, float3 lightVector, float3 viewVector)
{
    normal = normalize(normal);
    float3 direction = float3(lightPos.x, lightPos.y, lightPos.z) - pixelPos;
    float distance = length(direction);
    float4 colour = diffuse[index];
	
    float diffuseFact = saturate(dot(normalize(direction), normalize(normal)));
    
    direction /= distance;
	
    if (diffuseFact > 0.0f)
    {
        colour *= pow(saturate(dot(-direction, normalize(lightDirection[index].xyz))), lightDirection[index][3]);
        colour /= (attenuation[0] + (attenuation[1] * distance)) + (attenuation[2] * (distance * distance));
        float4 specularColour = calculateSpecular(lightVector, normal, viewVector, specular[index], position[index][3], index);
        colour += colour * specularColour;
	
    }
    //colour *= diffuse[index];
    return colour;
}

bool hasDepthData(float2 uv)
{
    if (uv.x < 0.f || uv.x > 1.f || uv.y < 0.f || uv.y > 1.f)
    {
        return false;
    }
    return true;
}

bool isInShadow(Texture2D sMap, float2 uv, float4 lightViewPosition, float bias)
{
    // Sample the shadow map (get depth of geometry)
    float depthValue = sMap.Sample(shadowSampler, uv).r;
	// Calculate the depth from the light.
    float lightDepthValue = lightViewPosition.z / lightViewPosition.w;
    lightDepthValue -= bias;

	// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
    if (lightDepthValue < depthValue)
    {
        return false;
    }
    return true;
}

float2 getProjectiveCoords(float4 lightViewPosition)
{
    // Calculate the projected texture coordinates.
    float2 projTex = lightViewPosition.xy / lightViewPosition.w;
    projTex *= float2(0.5, -0.5);
    projTex += float2(0.5f, 0.5f);
    return projTex;
}

bool pointIsInShadow(float depthValue, float4 lightViewPosition, float bias)
{
    float lightDepthValue = lightViewPosition.z / lightViewPosition.w;
    lightDepthValue -= bias;

	// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
    if (lightDepthValue < depthValue)
    {
        return false;
    }
    return true;
}



float2 getPointProjectiveCoords(float4 lightViewPosition)
{
    // Calculate the projected texture coordinates.
    float2 projTex = lightViewPosition.xy / lightViewPosition.w;
    projTex *= float2(0.5, -0.5);
    projTex += float2(0.5f, 0.5f);
    return projTex;
}

int GetIndex(float3 worldVecLightToPoint)
{
    if (abs(worldVecLightToPoint.x) > abs(worldVecLightToPoint.y) && abs(worldVecLightToPoint.x) > abs(worldVecLightToPoint.z))
    {
        if (worldVecLightToPoint.x < 0.0f)
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }
    else if (abs(worldVecLightToPoint.y) > abs(worldVecLightToPoint.x) && abs(worldVecLightToPoint.y) > abs(worldVecLightToPoint.z))
    {
        if (worldVecLightToPoint.y < 0.0f)
        {
            return 2;
        }
        else
        {
            return 3;
        }
    }
    else
    {
        if (worldVecLightToPoint.z < 0.0f)
        {
            return 4;
        }
        else
        {
            return 5;
        }
    }
}

float4 main(InputType input) : SV_TARGET
{
    //return float4(input.lightViewPos[0].xyz, 0);
    float shadowMapBias = 0.005f;
    float4 finalColour = 0.0f;
    float4 finalSpecular = 0.0f;
    float4 textureColour = texture0.Sample(sampler0, input.tex);
    //[unroll(8)] 
    for (int i = 0; i < MAXLIGHTS; i++)
    {
        //return float4(input.lightViewPos[i].xyz, 1);
        float2 pTexCoord = getProjectiveCoords(input.lightViewPos[i]);
        //return float4(pTexCoord.x, pTexCoord.x, pTexCoord.x, 1);
        switch (lightType[i].x)
        {
            case 0:
            {
                //case 0 means no light
                break;
            }
            case 1:
            {
                float3 worldVecLightToPoint = position[i].xyz - input.worldPosition;
                    int index = GetIndex(worldVecLightToPoint);
                
                
                    pTexCoord = getPointProjectiveCoords(input.lightViewPos[i+index]);
                    float sample;
                    
                    //return float4(pTexCoord, 0, 1);
                    
                
                    if (hasDepthData(pTexCoord))
                    {
                        if (abs(worldVecLightToPoint.x) > abs(worldVecLightToPoint.y) && abs(worldVecLightToPoint.x) > abs(worldVecLightToPoint.z))
                        {
                            if (worldVecLightToPoint.x < 0.0f)
                            {
                                
                                sample = pointLightTexture[0].Sample(shadowSampler, pTexCoord).r;
                            
                            }
                            else
                            {
                                sample = pointLightTexture[1].Sample(shadowSampler, pTexCoord).r;
                            }
                        }
                        else if (abs(worldVecLightToPoint.y) > abs(worldVecLightToPoint.x) && abs(worldVecLightToPoint.y) > abs(worldVecLightToPoint.z))
                        {
                            if (worldVecLightToPoint.y < 0.0f)
                            {
                                sample = pointLightTexture[2].Sample(shadowSampler, pTexCoord).r;
                            }
                            else
                            {
                                sample = pointLightTexture[3].Sample(shadowSampler, pTexCoord).r;
                            }
                        }
                        else
                        {
                            if (worldVecLightToPoint.z < 0.0f)
                            {
                                sample = pointLightTexture[4].Sample(shadowSampler, pTexCoord).r;
                            }
                            else
                            {
                                sample = pointLightTexture[5].Sample(shadowSampler, pTexCoord).r;
                            }
                        }
                    //Texture2D tex = getPointLightTex(texindex);
                        // Has depth map data
                        if (!pointIsInShadow(sample, input.lightViewPos[i+index], shadowMapBias))
                            {
                            //case 1 means point light calculations                
                                float3 lightVector = normalize(position[i].xyz - input.worldPosition);
                                float distance = length(position[i].xyz - input.worldPosition);
                                float4 lightColour = ambient[i] + calculateLighting(lightVector, input.normal, diffuse[i], distance, i);
                                float4 specularColour = calculateSpecular(lightVector, input.normal, input.viewVector, specular[i], position[i][3], i);
                                finalColour += lightColour;
                                finalSpecular += specularColour;
                            break;
                        }
                    }
                    break;
                }
            
            case 2:
            {
                //case 2 means spotlight calculations
                float3 lightVector = normalize(position[i].xyz - input.worldPosition);

                float4 lightColour = ambient[i] + calculateSpotLighting(input.normal, attenuation[i].xyz, position[i], input.worldPosition, i, lightVector, input.viewVector);
                float4 specularColour = calculateSpotSpecular(lightVector, input.normal, input.viewVector, specular[i], position[i][3], i, position[i], input.worldPosition);
                finalColour += lightColour;
                finalSpecular += specularColour;
                break;
                
            }
            case 3:
            {

                    if (hasDepthData(pTexCoord))
                    {
                    // Has depth map data
                        if (!isInShadow(depthMapTexture, pTexCoord, input.lightViewPos[i], shadowMapBias))
                        {
                        float4 lightColour = ambient[i] + calculateDirectionalLighting(-lightDirection[i].xyz, input.normal, diffuse[i]);
                        finalColour += lightColour;
                        break;
                        }

                    }
                break;
            }
            default:
            {
                //default catches invalid light values
                break;
            }
        }
        
    }
    //return float4(input.normal, 1);
    //if (abs(input.normal.x)+0.95f > abs(input.normal.y) || abs(input.normal.z)+0.95f > abs(input.normal.y))
    //{
    //    return (finalColour * textureColour)* float4(1.0, 0.5, 0.f, 1) + finalSpecular;
    //}
        return (finalColour * textureColour) + finalSpecular;
}



