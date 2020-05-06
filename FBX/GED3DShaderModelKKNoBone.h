//----------------------------------------------------------------
#ifndef _GED3DShaderModelKKNoBone_h_
#define _GED3DShaderModelKKNoBone_h_
//----------------------------------------------------------------
#include "GED3DShaderBase.h"
//----------------------------------------------------------------
//���ﲻ����һ���µĽṹ�壬����ʹ�� stShaderModelKKParam
//struct stShaderModelKKNoBoneParam
//----------------------------------------------------------------
class GED3DShaderModelKKNoBone : public GED3DShaderBase
{
public:
	GED3DShaderModelKKNoBone();
	~GED3DShaderModelKKNoBone();
	bool InitShaderModelKKNoBone();
	void ClearShaderModelKKNoBone();
	//��д����ĺ���
	void ProcessRender(void* pParam);

private:
	bool CreateInputLayout();

private:
	static GED3DShaderModelKKNoBone* ms_pInstance;
	ID3D11InputLayout* m_pInputLayout;
	ID3DX11Effect* m_pFxEffect;
	ID3DX11EffectTechnique* m_pFxTech;
	ID3DX11EffectVectorVariable* EyePosW;
	ID3DX11EffectMatrixVariable* m_pFxWorldMatrix;
	ID3DX11EffectMatrixVariable* m_pFxViewProjMatrix;
	ID3DX11EffectMatrixVariable* m_pFxWorldInvTransposeMatrix;
	ID3DX11EffectScalarVariable* m_pFxVertexTextureWidth;
	ID3DX11EffectScalarVariable* m_pFxhaveTangent;
	ID3DX11EffectScalarVariable* m_pFxVertexTexturePosCount;
	ID3DX11EffectScalarVariable* m_pFxVertexTextureNormalCount;
	ID3DX11EffectScalarVariable* m_pFxVertexTextureUVCount;
	ID3DX11EffectScalarVariable* m_pFxVertexTextureTangentCount;
	ID3DX11EffectShaderResourceVariable* m_pFxTexture;
	ID3DX11EffectShaderResourceVariable* m_pFxVertexTexture;
	ID3DX11EffectShaderResourceVariable* m_pFxNormalTexture;

};
//----------------------------------------------------------------
#endif //_GED3DShaderModelKKNoBone_h_
//----------------------------------------------------------------
