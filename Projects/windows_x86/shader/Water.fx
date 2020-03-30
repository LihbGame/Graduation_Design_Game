#include "LightHelper.fx"
 
cbuffer cbPerFrame
{
	//DirectionalLight gDirLights;
	float3 gEyePosW; 
    float4 gWaveParams;
};

cbuffer cbPerObject
{
    float4x4 gWorld;
	float4x4 gWorldViewProj;
}; 



// Nonnumeric values cannot be added to a cbuffer.
Texture2D gNormalMap;
Texture2D gReflectionMap;
Texture2D gRefractionMap;

SamplerState samLinearBump
{
    Filter = MIN_MAG_MIP_LINEAR;
	MaxAnisotropy = 1;

    AddressU = WRAP;
    AddressV = WRAP;
	
};

SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    MaxAnisotropy = 1;

    AddressU = MIRROR;
    AddressV = MIRROR;
};

struct VertexIn
{
    float3 Pos       : POSITION;
    float3 Normal    : NORMAL;
    float2 TexCoord0 : TEXCOORD;
    float3 Tangent   : TANGENT;
};

struct VertexOut
{
    float4 PosH    : SV_POSITION;
    float3 Eye        : TEXCOORD0;
    float4 Wave0      : TEXCOORD1;
    float2 Wave1      : TEXCOORD2;
    float2 Wave2      : TEXCOORD3;
    float2 Wave3      : TEXCOORD4;
    float4 ScreenPos  : TEXCOORD5;
};


float3x3 GetTangentSpaceBasis(float3 T, float3 N)
{
    float3x3 objToTangentSpace;

    objToTangentSpace[0] = T;           // tangent
    objToTangentSpace[1] = cross(T, N); // binormal
    objToTangentSpace[2] = N;           // normal  

    return objToTangentSpace;
}

// This assumes NdotL comes clamped
half Fresnel(half NdotL, half fresnelBias, half fresnelPow)
{
    half facing = (1.0 - NdotL);
    return  max(fresnelBias + (1.0 - fresnelBias) * pow(facing, fresnelPow), 0.0);
}

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

    float3 PosW = vin.Pos;

    // Transform to homogeneous clip space.
    vout.PosH = mul(float4(vin.Pos, 1.0f), gWorldViewProj);
    float tans = fmod(gWaveParams.w, 100) * 0.01;
    float2 fTranslation = float2(tans, 0);
    float2 vTexCoords = PosW.xz * 0.001;

    // Output bump layers texture coordinates    
    //float fSinTranslation = sin(fTranslation * 100) * 0.005;//<-0.005f,+0.005f>
    //float2 vTranslation0 = fTranslation + fSinTranslation;
    //float2 vTranslation1 = fTranslation - fSinTranslation;
    //float2 vTranslation2 = fTranslation;
	
    // Scale texture coordinates to get mix of low/high frequency details
    vout.Wave0.xy = vTexCoords.xy + fTranslation * 2.0;
    vout.Wave1.xy = vTexCoords.xy * 2.0 + fTranslation * 4.0;
    vout.Wave2.xy = vTexCoords.xy * 4.0 + fTranslation * 2.0;
    vout.Wave3.xy = vTexCoords.xy * 8.0 + fTranslation;

    // perspective corrected projection
    float4 vHPos = mul(float4(vin.Pos, 1.0f), gWorldViewProj);
    vout.Wave0.zw = vHPos.w;

    vHPos.y = -vHPos.y;
    vout.ScreenPos.xy = (vHPos.xy + vHPos.w) * 0.5;
    vout.ScreenPos.zw = float2(1, vHPos.w);

    // get tangent space basis    
    float3x3 objToTangentSpace = GetTangentSpaceBasis(vin.Tangent.xyz, vin.Normal.xyz);

    float3 EyeVec = gEyePosW.xyz - PosW;
    vout.Eye.xyz = mul(EyeVec,objToTangentSpace);


	return vout;
}
 
float4 PS(VertexOut pin) : SV_Target
{
    float3 vEye = normalize(pin.Eye);

    // Get bump layers
    float3 vBumpTexA = gNormalMap.Sample(samLinearBump, pin.Wave0.xy).xyz;
    float3 vBumpTexB = gNormalMap.Sample(samLinearBump, pin.Wave1.xy).xyz;
    float3 vBumpTexC = gNormalMap.Sample(samLinearBump, pin.Wave2.xy).xyz;
    float3 vBumpTexD = gNormalMap.Sample(samLinearBump, pin.Wave3.xy).xyz;

    // Average bump layers
    float3 vBumpTex = normalize(2.0 * (vBumpTexA.xyz + vBumpTexB.xyz + vBumpTexC.xyz + vBumpTexD.xyz) - 4.0);

    // Apply individual bump scale for refraction and reflection
    float3 vRefrBump = vBumpTex.xyz * float3(0.02, 0.02, 1.0);
    float3 vReflBump = vBumpTex.xyz * float3(0.1, 0.1, 1.0);


    // Compute projected coordinates
    float2 vProj = (pin.ScreenPos.xy / pin.ScreenPos.w);
    float4 vReflection = gReflectionMap.Sample(samLinear, vProj.xy + vReflBump.xy);
    float4 vRefrA = gRefractionMap.Sample(samLinear, vProj.xy + vRefrBump.xy);
    float4 vRefrB = gRefractionMap.Sample(samLinear, vProj.xy);

    // Mask occluders from refraction map
    float4 vRefraction = vRefrB * vRefrA.w + vRefrA * (1 - vRefrA.w);

    // Compute Fresnel term
    float NdotL = max(dot(vEye, vReflBump), 0);
    float facing = (1.0 - NdotL);
    float fresnel = Fresnel(NdotL, 0.2, 5.0);

    // Use distance to lerp between refraction and deep water color
    float fDistScale = saturate(10.0 / pin.Wave0.w);
    float3 WaterDeepColor = (vRefraction.xyz * fDistScale + (1 - fDistScale) * float3(0, 0.1, 0.125));
    // Lerp between water color and deep water color
    float3 WaterColor = float3(0, 0.1, 0.15);
    float3 waterColor = (WaterColor * facing + WaterDeepColor * (1.0 - facing));
    float3 cReflect = fresnel * vReflection;

    // final water = reflection_color * fresnel + water_color
    return float4(cReflect + waterColor, 1);
}

technique11 Water
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}

