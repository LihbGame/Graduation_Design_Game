//----------------------------------------------------------------
#ifndef _GED3DShaderModelKK_h_
#define _GED3DShaderModelKK_h_
//----------------------------------------------------------------
#include "GED3DShaderBase.h"
#include "ShadowMap.h"
//----------------------------------------------------------------
struct stShaderModelKKParam
{
	//��������
	ID3D11Buffer* pVB;
	//ʵ��������
	ID3D11Buffer* pInstanceVB;
	//������������
	ID3D11Buffer* pIB;
	//����ṹ���sizeof
	unsigned int uiSizeofVertex;
	//���������ĸ���
	unsigned int uiIndexCount;
	//��ͼ
	ID3D11ShaderResourceView* pTextureSRV;

	//������ͼ
	ID3D11ShaderResourceView* pNormalTextureSRV;

	//��������ϵ����
	XMMATRIX* pMatWorld;
	//
	//��Ŷ���ṹ���Ա��ʵ��ֵ���ڴ�飬����ڴ�鱻��װ������ͼ
	ID3D11ShaderResourceView* pPosSRV;
	//��ͼ���ȣ���λ����
	int nPosSRVWidth;
	//�ڴ�����ж��ٸ���ʵ��λ����ֵ
	int nPosCount;
	//�ڴ�����ж��ٸ���ʵ�ķ�����ֵ
	int nNormalCount;
	int nTangentCount;
	//�ڴ�����ж��ٸ���ʵ��UV��ֵ
	int nUVCount;
	//
	//��ǰ���ڲ����ĸ�������
	//�ö����Ĺ�������ת��λ�����ݣ�����װ������ͼ
	ID3D11ShaderResourceView* pAnimSRV;
	//��ͼ���ȣ���λ����
	int nAnimSRVWidth;
	//�ж��ٸ�����
	int nBoneCount;
	//��ǰ���ڲ��Ŷ�������һ֡��
	//ֵΪ-1��ʾ���������������
	int nKeyFrameIndex;
	//ʵ��������
	int nInstanceCount;
	//��Ӱ
	ShadowMap* mShadowMap;
};
//----------------------------------------------------------------
class GED3DShaderModelKK : public GED3DShaderBase
{
public:
	GED3DShaderModelKK();
	~GED3DShaderModelKK();
	bool InitShaderModelKK();
	void ClearShaderModelKK();
	//��д����ĺ���
	void ProcessRender(void* pParam);

private:
	bool CreateInputLayout();

private:
	static GED3DShaderModelKK*  ms_pInstance;
	ID3D11InputLayout* m_pInputLayout;
	ID3DX11Effect* m_pFxEffect;
	ID3DX11EffectTechnique* m_pFxTech;
	ID3DX11EffectVectorVariable* EyePosW;
	ID3DX11EffectMatrixVariable* m_pFxWorldMatrix;
	ID3DX11EffectMatrixVariable* m_pFxViewProjMatrix;
	ID3DX11EffectMatrixVariable* m_pFxWorldInvTransposeMatrix;
	ID3DX11EffectScalarVariable* m_pFxVertexTextureWidth;
	ID3DX11EffectScalarVariable* m_pFxVertexTexturePosCount;
	ID3DX11EffectScalarVariable* m_pFxVertexTextureNormalCount;
	ID3DX11EffectScalarVariable* m_pFxVertexTextureUVCount;
	ID3DX11EffectScalarVariable* m_pFxVertexTextureTangentCount;
	ID3DX11EffectScalarVariable* m_pFxhaveTangent;
	ID3DX11EffectScalarVariable* m_pFxAnimTextureWidth;
	ID3DX11EffectScalarVariable* m_pFxAnimTextureBoneCount;
	ID3DX11EffectScalarVariable* m_pFxAnimTextureKeyFrameIndex;
	ID3DX11EffectShaderResourceVariable* m_pFxTexture;
	ID3DX11EffectShaderResourceVariable* m_pFxVertexTexture;
	ID3DX11EffectShaderResourceVariable* m_pFxAnimTexture;
	ID3DX11EffectShaderResourceVariable* m_pFxNormalTexture;
};
//----------------------------------------------------------------
#endif //_GED3DShaderModelKK_h_
//----------------------------------------------------------------