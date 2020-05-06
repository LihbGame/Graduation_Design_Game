//----------------------------------------------------------------
#ifndef _GED3DModelKK_h_
#define _GED3DModelKK_h_
//----------------------------------------------------------------
#include "GED3DModelBase.h"
#include "ShadowMap.h"
//----------------------------------------------------------------
class GED3DTexture;
class GED3DShaderBase;
class StKKModel;
//----------------------------------------------------------------
struct GED3DModelInitParam_KK
{
	const StKKModel* pKKModel;
};
//----------------------------------------------------------------
class GED3DModelKK : public GED3DModelBase
{
public:
	GED3DModelKK();
	~GED3DModelKK();
	//重写基类的函数
	bool InitModel(void* pInitParam);
	void ClearModel();
	void UpdateModel(float fDeltaTime, int nAnimID);
	void RenderModel(Model_Tansform_Info* mode_info, int instance_num, int nAnimID);
	void RenderShadowMap(Model_Tansform_Info* mode_info, int instance_num, int nAnimID, ShadowMap* shadowmap,ID3D11Buffer* InstanceVB=nullptr);
	void SetWorldMatrix(const XMFLOAT4X4* pMatWorld);
	void SetTexture(const wchar_t* szFileName);
	void SetNormalTexture(const wchar_t* szFileName);
	//世界坐标系矩阵变换。
	XMFLOAT4X4 m_matWorld;

private:
	bool CreateTexture(const wchar_t* szFileName);
	void ReleaseTexture();

private:
	const StKKModel* m_pKKModel;
	GED3DShaderBase* m_pShader;
	GED3DShaderBase* m_pShadowShader;
	GED3DTexture* m_pTexture;
	ID3D11ShaderResourceView* m_pNormalTexture;
	float m_fAccTime;
	int m_nCurrentKeyFrameIndex;

};
//----------------------------------------------------------------
#endif //_GED3DModelKK_h_
//----------------------------------------------------------------
