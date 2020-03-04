#include "LightHelper.fx"

//----------------------------------------------------------------
struct VertexInputType
{
	uint4 ValueIndex : POSITION0;
};
//----------------------------------------------------------------
struct VertexOutputType
{
	float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD;
	float4 PositionInWorld : COLOR0;
	float4 NormalInWorld : COLOR1;
	float3 TangentInWorld : COLOR2;
};
//----------------------------------------------------------------
//接收 VertexShader 所需要的变量
//----------------------------------------------------------------
cbuffer BufferMatrix
{
	matrix g_matWorld;
	matrix g_matViewProj;
	matrix g_matWorldInvTranspose;
	float3 gEyePosW;
	uint g_VertexTextureWidth;
	uint g_VertexTexturePosCount;
	uint g_VertexTextureNormalCount;
	uint g_VertexTextureTangentCount;
	uint g_VertexTextureUVCount;
	bool IshaveTangent;
};
//----------------------------------------------------------------
//接收 PixelShader 所需要的变量
//----------------------------------------------------------------
//0号贴图是BaseTexture
//1号贴图是顶点结构体成员的值
Texture2D g_TextureList[2];
Texture2D g_NormalTexture;
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
//----------------------------------------------------------------
void CalculatePosNormalUV(in uint4 ValueIndex, out float4 thePos, out float4 theNormal,out float3 theTangent, out float2 theUV)
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
	uint NormalPixelIndex = (sizeofPos * g_VertexTexturePosCount + sizeofNormal * ValueIndex[1])/ sizeofPixel;
	tempUV[0] = NormalPixelIndex % g_VertexTextureWidth;
	tempUV[1] = NormalPixelIndex / g_VertexTextureWidth;
	theNormal.x = g_TextureList[1][tempUV];
	//
	tempUV[0] += 1;
	if (tempUV[0] >= g_VertexTextureWidth)
	{
		tempUV[0] = 0;
		tempUV[1] += 1;
	}
	theNormal.y = g_TextureList[1][tempUV];
	//
	tempUV[0] += 1;
	if (tempUV[0] >= g_VertexTextureWidth)
	{
		tempUV[0] = 0;
		tempUV[1] += 1;
	}
	theNormal.z = g_TextureList[1][tempUV];
	theNormal.w = 0.0f;
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>



	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	if (IshaveTangent)
	{
		uint TangentPixelIndex = (sizeofPos * g_VertexTexturePosCount + sizeofNormal * g_VertexTextureNormalCount + sizeofTangent * ValueIndex[2]) / sizeofPixel;
		tempUV[0] = TangentPixelIndex % g_VertexTextureWidth;
		tempUV[1] = TangentPixelIndex / g_VertexTextureWidth;
		theTangent.x = g_TextureList[1][tempUV];
		//
		tempUV[0] += 1;
		if (tempUV[0] >= g_VertexTextureWidth)
		{
			tempUV[0] = 0;
			tempUV[1] += 1;
		}
		theTangent.y = g_TextureList[1][tempUV];
		//
		tempUV[0] += 1;
		if (tempUV[0] >= g_VertexTextureWidth)
		{
			tempUV[0] = 0;
			tempUV[1] += 1;
		}
		theTangent.z = g_TextureList[1][tempUV];
	}
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	uint UVPixelIndex = (sizeofPos * g_VertexTexturePosCount + sizeofNormal * g_VertexTextureNormalCount+ sizeofTangent* g_VertexTextureTangentCount + sizeofUV * ValueIndex[3])/ sizeofPixel;
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
	float4 theNormal;
	float3 theTangent;
	float2 theUV;
	CalculatePosNormalUV(input.ValueIndex, thePos, theNormal, theTangent, theUV);
	

	
	//计算世界空间内的坐标和法线
	thePos = mul(thePos, g_matWorld);
	theNormal = mul(theNormal, g_matWorldInvTranspose);
	

	VertexOutputType output;
	output.Position = mul(thePos, g_matViewProj);
	output.TexCoord = theUV;
	output.PositionInWorld = thePos;
	output.NormalInWorld = theNormal;
	if (IshaveTangent)
	{
		theTangent = mul(float4(theTangent, 0.0f), g_matWorldInvTranspose);
		output.TangentInWorld = theTangent;
	}
	return output;
}
//----------------------------------------------------------------
float4 ShaderPS(VertexOutputType input) : SV_TARGET
{
	// Interpolating normal can unnormalize it, so normalize it.
	input.NormalInWorld = normalize(input.NormalInWorld);

	// The toEye vector is used in lighting.
	float3 toEye = gEyePosW - input.PositionInWorld;

	// Cache the distance to the eye from this surface point.
	float distToEye = length(toEye);

	// Normalize.
	toEye /= distToEye;


	// Default to multiplicative identity.
	float4 texColor = float4(1, 1, 1, 1);
	float3 normalMapSample = float3(1, 1, 1);
	// Sample texture.
	texColor = g_TextureList[0].Sample(g_SampleType, input.TexCoord);
	float3 bumpedNormalW = input.NormalInWorld.rgb;
	if (IshaveTangent)
	{
		normalMapSample = g_NormalTexture.Sample(g_SampleType, input.TexCoord).rgb;
		// Normal mapping bm
		bumpedNormalW = NormalSampleToWorldSpace(normalMapSample, bumpedNormalW, input.TangentInWorld);
	}

	// Lighting.

	float4 litColor = texColor;

	// Start with a sum of zero. 
	float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// Sum the light contribution from each light source.  
	//now only have one direction light

	//Direction light init
	DirectionalLight gDirLights;
	gDirLights.Ambient = float4(0.5f, 0.5f, 0.5f, 1.0f);
	gDirLights.Diffuse = float4(0.6f, 0.6f, 0.6f, 1.0f);
	gDirLights.Specular = float4(0.5f, 0.5f, 0.5f, 1.0f);
	gDirLights.Direction = float3(0.57735f, -0.57735f, -0.57735f);


	//mat init
	Material mMat;
	mMat.Ambient = float4(0.7f, 0.7f, 0.7f, 1.0f);
	mMat.Diffuse = float4(1.0f, 1.0f, 1.0f, 1.0f);
	mMat.Specular = float4(0.5f, 0.5f, 0.5f, 16.0f);


	float4 A, D, S;
	ComputeDirectionalLight(mMat, gDirLights, bumpedNormalW, toEye,A, D, S);
	ambient += A;
	diffuse += D;
	spec += S;

	// Modulate with late add.
	litColor = texColor * (ambient + diffuse) + spec;
	// Common to take alpha from diffuse material and texture.
	litColor.a = mMat.Diffuse.a * texColor.a;

	clip(litColor.a-0.1f);
	return litColor;
}
//----------------------------------------------------------------
technique11 ShaderTec
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, ShaderVS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, ShaderPS()));
	}
}
//----------------------------------------------------------------


