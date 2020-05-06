#pragma once

#include "GED3DShaderBase.h"

class FBXShadowMapShaderkk : public GED3DShaderBase
{
public:
	FBXShadowMapShaderkk();
	~FBXShadowMapShaderkk();

	bool InitShadowShaderKK();
	void ClearShadowShaderKK();
	//重写基类的函数
	void ProcessRender(void* pParam);
private:
	bool CreateInputLayout();

private:
	static FBXShadowMapShaderkk* ms_pInstance;
	ID3D11InputLayout* m_pInputLayout;
	ID3DX11Effect* m_pFxEffect;
	ID3DX11EffectTechnique* m_pFxTech;
	ID3DX11EffectMatrixVariable* m_pFxWorldMatrix;
	ID3DX11EffectMatrixVariable* m_pFxViewProjMatrix;
	ID3DX11EffectMatrixVariable* m_pFxWorldInvTransposeMatrix;
	ID3DX11EffectScalarVariable* m_pFxVertexTextureWidth;
	ID3DX11EffectScalarVariable* m_pFxVertexTexturePosCount;
	ID3DX11EffectScalarVariable* m_pFxAnimTextureWidth;
	ID3DX11EffectScalarVariable* m_pFxAnimTextureBoneCount;
	ID3DX11EffectScalarVariable* m_pFxAnimTextureKeyFrameIndex;
	ID3DX11EffectShaderResourceVariable* m_pFxVertexTexture;
	ID3DX11EffectShaderResourceVariable* m_pFxAnimTexture;
};

