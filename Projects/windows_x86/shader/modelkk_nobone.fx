//----------------------------------------------------------------
struct VertexInputType
{
	uint3 ValueIndex : POSITION0;
};
//----------------------------------------------------------------
struct VertexOutputType
{
	float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD;
	float4 PositionInWorld : COLOR0;
	float4 NormalInWorld : COLOR1;
};
//----------------------------------------------------------------
//接收 VertexShader 所需要的变量
//----------------------------------------------------------------
cbuffer BufferMatrix
{
	matrix g_matWorld;
	matrix g_matViewProj;
	matrix g_matWorldInvTranspose;
	uint g_VertexTextureWidth;
	uint g_VertexTexturePosCount;
	uint g_VertexTextureNormalCount;
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
//----------------------------------------------------------------
void CalculatePosNormalUV(in uint3 ValueIndex, out float4 thePos, out float4 theNormal, out float2 theUV)
{
	uint sizeofPixel = 4;
	uint sizeofPos = 12;
	uint sizeofNormal = 12;
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
	uint UVPixelIndex = (sizeofPos * g_VertexTexturePosCount + sizeofNormal * g_VertexTextureNormalCount + sizeofUV * ValueIndex[2])/ sizeofPixel;
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
	float2 theUV;
	CalculatePosNormalUV(input.ValueIndex, thePos, theNormal, theUV);
	
	
	//计算世界空间内的坐标和法线
	thePos = mul(thePos, g_matWorld);
	theNormal = mul(theNormal, g_matWorldInvTranspose);
	
	VertexOutputType output;
	output.Position = mul(thePos, g_matViewProj);
	output.TexCoord = theUV;
	output.PositionInWorld = thePos;
	output.NormalInWorld = theNormal;
	return output;
}
//----------------------------------------------------------------
float4 ShaderPS(VertexOutputType input) : SV_TARGET
{
	//计算点光源
	//float4 PointLightDiffuse = CalculatePointLight(input.PositionInWorld, input.NormalInWorld);
	
	float4 outputColor = g_TextureList[0].Sample(g_SampleType, input.TexCoord);
	
	//if (PointLightDiffuse.w > 0.01f)
	//{
	//	outputColor = outputColor * (1.0f-PointLightDiffuse.w) + PointLightDiffuse * PointLightDiffuse.w;
	//	outputColor.w = 1.0f;
	//}

	Clip(outputColor.a-0.1f);
	return outputColor;
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


