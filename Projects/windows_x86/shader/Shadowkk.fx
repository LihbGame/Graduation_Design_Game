//----------------------------------------------------------------
struct VertexInputType
{
	uint4 ValueIndex : POSITION0;
	uint4 BoneIndex : COLOR0;
	float4 BoneWeight : COLOR1;
};
//----------------------------------------------------------------
struct VertexOutputType
{
	float4 Position : SV_POSITION;
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
	uint g_AnimTextureWidth;
	uint g_AnimTextureBoneCount;
	uint g_AnimTextureKeyFrameIndex;
};
//----------------------------------------------------------------
//接收 PixelShader 所需要的变量
//----------------------------------------------------------------
//0号贴图是BaseTexture
//1号贴图是顶点结构体成员的值
//2号贴图是骨骼动画
Texture2D g_TextureList[3];
//----------------------------------------------------------------
SamplerState g_SampleType
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};
//----------------------------------------------------------------

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
	DepthBias = 100000;
	DepthBiasClamp = 0.0f;
	SlopeScaledDepthBias = 1.0f;
};







void CalculatePos(in uint4 ValueIndex, out float4 thePos)
{
	uint sizeofPixel = 4;
	uint sizeofPos = 12;
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
}
//----------------------------------------------------------------
float4x4 GetSingleBoneAnimMatrix(in uint BoneIndex)
{
	float4x4 finalMat;
	uint sizeofPixel = 4;
	uint ElementSize = 48;
	uint startPixelIndex = (g_AnimTextureKeyFrameIndex * g_AnimTextureBoneCount * ElementSize + BoneIndex * ElementSize) / sizeofPixel;
	uint2 tempUV;
	tempUV[0] = startPixelIndex % g_AnimTextureWidth;
	tempUV[1] = startPixelIndex / g_AnimTextureWidth;
	finalMat._m00 = g_TextureList[2][tempUV];
	//
	tempUV[0] += 1;
	finalMat._m01 = g_TextureList[2][tempUV];
	//
	tempUV[0] += 1;
	finalMat._m02 = g_TextureList[2][tempUV];
	//
	tempUV[0] += 1;
	finalMat._m10 = g_TextureList[2][tempUV];
	//
	tempUV[0] += 1;
	finalMat._m11 = g_TextureList[2][tempUV];
	//
	tempUV[0] += 1;
	finalMat._m12 = g_TextureList[2][tempUV];
	//
	tempUV[0] += 1;
	finalMat._m20 = g_TextureList[2][tempUV];
	//
	tempUV[0] += 1;
	finalMat._m21 = g_TextureList[2][tempUV];
	//
	tempUV[0] += 1;
	finalMat._m22 = g_TextureList[2][tempUV];
	//
	tempUV[0] += 1;
	finalMat._m30 = g_TextureList[2][tempUV];
	//
	tempUV[0] += 1;
	finalMat._m31 = g_TextureList[2][tempUV];
	//
	tempUV[0] += 1;
	finalMat._m32 = g_TextureList[2][tempUV];
	//
	finalMat._m03 = 0.0f;
	finalMat._m13 = 0.0f;
	finalMat._m23 = 0.0f;
	finalMat._m33 = 1.0f;
	//
	return finalMat;
}
//----------------------------------------------------------------
void CalculateBoneAnim(in uint4 BoneIndex, in float4 BoneWeight, inout float4 thePos)
{
	float4 finalPos = {0.0f, 0.0f, 0.0f, 0.0f};
	for (uint i = 0; i < 4; ++i)
	{
		if (BoneIndex[i] != 0xFFFFFFFF)
		{
			float4x4 BoneMat = GetSingleBoneAnimMatrix(BoneIndex[i]);
			//
			finalPos += mul(thePos, BoneMat) * BoneWeight[i];
		}
		else
		{
			break;
		}
	}
	finalPos.w = 1.0f;
	thePos = finalPos;
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
	CalculatePos(input.ValueIndex, thePos);
	
	//计算骨骼动画 
	//动画帧序号为-1，表示无效值，不计算骨骼动画
	if (g_AnimTextureKeyFrameIndex != 0xFFFFFFFF)
	{
		CalculateBoneAnim(input.BoneIndex, input.BoneWeight, thePos);
	}
	
	//计算世界空间内的坐标
	thePos = mul(thePos, g_matWorld);
	
	VertexOutputType output;
	output.Position = mul(thePos, g_matViewProj);

	return output;
}



//----------------------------------------------------------------
technique11 ShaderTec
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, ShaderVS()));
		SetGeometryShader(NULL);
		SetPixelShader(NULL);

		SetRasterizerState(Depth);
	}
}
//----------------------------------------------------------------


