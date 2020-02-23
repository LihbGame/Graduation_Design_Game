//----------------------------------------------------------------
#ifndef _SoD3DModelFbx_h_
#define _SoD3DModelFbx_h_
//----------------------------------------------------------------
#include "SoD3DModelBase.h"
//----------------------------------------------------------------
class SoD3DTexture;
class SoD3DShaderBase;
struct StFBXMeshData;
class StFBXModel;
//----------------------------------------------------------------
struct SoD3DModelInitParam_Fbx
{
	const StFBXMeshData* pMeshData;
};
//----------------------------------------------------------------
class SoD3DModelFbx : public SoD3DModelBase
{
public:
	SoD3DModelFbx();
	~SoD3DModelFbx();
	//��д����ĺ���
	bool InitModel(void* pInitParam);
	void ClearModel();
	void UpdateModel(float fDeltaTime, StFBXModel* pAnimatedModel);
	void RenderModel(Model_Tansform_Info *mode_info,int instance_num);

	void SetWorldMatrix(const XMFLOAT4X4* pMatWorld);
	void SetTexture(const wchar_t* szFileName);

private:
	bool CreateVertexBuffer(const StFBXMeshData* pMeshData);
	void ReleaseVertexBuffer();
	bool CreateIndexBuffer(const StFBXMeshData* pMeshData);
	void ReleaseIndexBuffer();
	bool CreateTexture(const wchar_t* szFileName);
	void ReleaseTexture();

private:
	struct stVertexType
	{
		XMFLOAT3 kPosition;
		XMFLOAT3 kNormal;
		XMFLOAT2 kUV;
	};

private:
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;
	SoD3DShaderBase* m_pShader;
	SoD3DTexture* m_pTexture;
	int m_nVertexCount;
	int m_nIndexCount;
	//��������ϵ����任��
	XMFLOAT4X4 m_matWorld;

	float m_fAccTime;
};
//----------------------------------------------------------------
#endif //_SoD3DModelFbx_h_
//----------------------------------------------------------------
