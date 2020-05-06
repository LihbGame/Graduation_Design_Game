//----------------------------------------------------------------
#ifndef _GED3DModelFbx_h_
#define _GED3DModelFbx_h_
//----------------------------------------------------------------
#include "GED3DModelBase.h"
//----------------------------------------------------------------
class GED3DTexture;
class GED3DShaderBase;
struct StFBXMeshData;
class StFBXModel;
//----------------------------------------------------------------
struct GED3DModelInitParam_Fbx
{
	const StFBXMeshData* pMeshData;
};
//----------------------------------------------------------------
class GED3DModelFbx : public GED3DModelBase
{
public:
	GED3DModelFbx();
	~GED3DModelFbx();
	//重写基类的函数
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
	GED3DShaderBase* m_pShader;
	GED3DTexture* m_pTexture;
	int m_nVertexCount;
	int m_nIndexCount;
	//世界坐标系矩阵变换。
	XMFLOAT4X4 m_matWorld;

	float m_fAccTime;
};
//----------------------------------------------------------------
#endif //_GED3DModelFbx_h_
//----------------------------------------------------------------
