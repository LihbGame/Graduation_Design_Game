#include "LightHelper.fx"

//----------------------------------------------------------------
struct VertexInputType
{
	uint4 ValueIndex : POSITION0;
	row_major float4x4 World  : WORLD;//instance world data
	uint InstanceId : SV_InstanceID;
};
//----------------------------------------------------------------
struct VertexOutputType
{
	float4 Position : SV_POSITION;
	float2 Tex  : TEXCOORD;
};
//----------------------------------------------------------------
//接收 VertexShader 所需要的变量
//----------------------------------------------------------------
cbuffer BufferMatrix
{
	matrix g_matViewProj;
	uint g_VertexTextureWidth;
	uint g_VertexTexturePosCount;
	uint g_VertexTextureNormalCount;
	uint g_VertexTextureTangentCount;
	uint g_VertexTextureUVCount;
};
//----------------------------------------------------------------
//接收 PixelShader 所需要的变量
//----------------------------------------------------------------
//0号贴图是BaseTexture
//1号贴图是顶点结构体成员的值
Texture2D g_TextureList[2];
//----------------------------------------------------------------
SamplerState g_SampleType
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 4;
	AddressU = WRAP;
	AddressV = WRAP;
	AddressW = WRAP;
	MipLODBias = 0.0f;
	
	ComparisonFunc = ALWAYS;
	//BorderColor[0] = 0.0f;
	//BorderColor[1] = 0.0f;
	//BorderColor[2] = 0.0f;
	//BorderColor[3] = 0.0f;
	MinLOD = 0.0f;
	MaxLOD = (3.402823466e+38f); //D3D11_FLOAT32_MAX
};

SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

RasterizerState Depth
{
	// [From MSDN]
	// If the depth buffer currently bound to the output-merger stage has a UNORM format or
	// no depth buffer is bound the bias value is calculated like this: 
	//
	// Bias = (float)DepthBias * r + SlopeScaledDepthBias * MaxDepthSlope;
	//
	// where r is the minimum representable value > 0 in the depth-buffer format converted to float32.
	// [/End MSDN]
	// 
	// For a 24-bit depth buffer, r = 1 / 2^24.
	//
	// Example: DepthBias = 100000 ==> Actual DepthBias = 100000/2^24 = .006

	// You need to experiment with these values for your scene.
	DepthBias = 10000;
	DepthBiasClamp = 0.0f;
	SlopeScaledDepthBias = 1.0f;
	CullMode = FRONT;
};





//----------------------------------------------------------------
void CalculatePosUV(in uint4 ValueIndex, out float4 thePos, out float2 theUV)
{
	uint sizeofPixel = 4;
	uint sizeofPos = 12;
	uint sizeofNormal = 12;
	uint sizeofTangent = 12;
	uint sizeofUV = 8;
	uint2 tempUV;
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	uint PosPixelIndex = sizeofPos * ValueIndex[0] / sizeofPixel;
	tempUV[0] = PosPixelIndex % g_VertexTextureWidth;
	tempUV[1] = PosPixelIndex / g_VertexTextureWidth;
	thePos.x = g_TextureList[1][tempUV];
	//
	tempUV[0] += 1;
	if (tempUV[0] >= g_VertexTextureWidth)
	{
		tempUV[0] = 0;
		tempUV[1] += 1;
	}
	thePos.y = g_TextureList[1][tempUV];
	//
	tempUV[0] += 1;
	if (tempUV[0] >= g_VertexTextureWidth)
	{
		tempUV[0] = 0;
		tempUV[1] += 1;
	}
	thePos.z = g_TextureList[1][tempUV];
	thePos.w = 1.0f;
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	uint UVPixelIndex = (sizeofPos * g_VertexTexturePosCount + sizeofNormal * g_VertexTextureNormalCount + sizeofTangent * g_VertexTextureTangentCount + sizeofUV * ValueIndex[3]) / sizeofPixel;
	tempUV[0] = UVPixelIndex % g_VertexTextureWidth;
	tempUV[1] = UVPixelIndex / g_VertexTextureWidth;
	theUV.x = g_TextureList[1][tempUV];
	//
	tempUV[0] += 1;
	if (tempUV[0] >= g_VertexTextureWidth)
	{
		tempUV[0] = 0;
		tempUV[1] += 1;
	}
	theUV.y = g_TextureList[1][tempUV];
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
}
//----------------------------------------------------------------
float4 CalculatePointLight(in float4 thePos, in float4 theNormal)
{
	float3 PointLightColor = {1.0f, 0.0f, 0.0f};
	float3 PointLightPos = {0.0f, 0.0f, 3.0f};
	float PointLightRange = 90.0f;
	float PointLightBright = 0.5f;
	
	float3 Direction = float3(PointLightPos.x-thePos.x, PointLightPos.y-thePos.y, PointLightPos.z-thePos.z);
	float Distance = length(Direction);
	Direction = normalize(Direction);
	float fDot = dot(Direction, float3(theNormal.xyz));
	
	float4 PointLightDiffuse = {0.0f, 0.0f, 0.0f, 0.0f};
	if (Distance < PointLightRange && fDot > 0.01f)
	{
		float scale =  (PointLightRange - Distance) / PointLightRange;
		PointLightDiffuse.xyz = PointLightColor * scale;
		PointLightDiffuse.w = PointLightBright * scale;
	}
	return PointLightDiffuse;
}
//----------------------------------------------------------------
VertexOutputType ShaderVS(VertexInputType input)
{
	float4 thePos;
	float2 theUV;
	CalculatePosUV(input.ValueIndex, thePos, theUV);
	

	
	//计算世界空间内的坐标
	thePos = mul(thePos, input.World);
	VertexOutputType output;
	output.Position = mul(thePos, g_matViewProj);
	output.Tex = theUV;
	return output;
}
//----------------------------------------------------------------
void PS(VertexOutputType pin)
{
	float4 diffuse =g_TextureList[0].Sample(samLinear, pin.Tex);

	// Don't write transparent pixels to the shadow map.
	clip(diffuse.a - 0.15f);
}




technique11 ShaderTec
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, ShaderVS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
		SetRasterizerState(Depth);
	}
}
//----------------------------------------------------------------


