#pragma once
#include "GED3DShaderBase.h"

class FBXShadowMapShaderkkNoBone : public GED3DShaderBase
{
public:
	FBXShadowMapShaderkkNoBone();
	~FBXShadowMapShaderkkNoBone();
	bool InitShadowShaderKKNoBone();
	void ClearShadowShaderKKNoBone();
	//重写基类的函数
	void ProcessRender(void* pParam);

private:
	bool CreateInputLayout();

private:
	static FBXShadowMapShaderkkNoBone* ms_pInstance;
	ID3D11InputLayout* m_pInputLayout;
	ID3DX11Effect* m_pFxEffect;
	ID3DX11EffectTechnique* m_pFxTech;
	ID3DX11EffectMatrixVariable* m_pFxViewProjMatrix;
	ID3DX11EffectScalarVariable* m_pFxVertexTextureWidth;
	ID3DX11EffectScalarVariable* m_pFxVertexTexturePosCount;
	ID3DX11EffectScalarVariable* m_pFxVertexTextureNormalCount;
	ID3DX11EffectScalarVariable* m_pFxVertexTextureUVCount;
	ID3DX11EffectScalarVariable* m_pFxVertexTextureTangentCount;
	ID3DX11EffectShaderResourceVariable* m_pFxTexture;
	ID3DX11EffectShaderResourceVariable* m_pFxVertexTexture;

};