//----------------------------------------------------------------
#ifndef _SoD3DModelKK_h_
#define _SoD3DModelKK_h_
//----------------------------------------------------------------
#include "SoD3DModelBase.h"
#include "ShadowMap.h"
//----------------------------------------------------------------
class SoD3DTexture;
class SoD3DShaderBase;
class StKKModel;
//----------------------------------------------------------------
struct SoD3DModelInitParam_KK
{
	const StKKModel* pKKModel;
};
//----------------------------------------------------------------
class SoD3DModelKK : public SoD3DModelBase
{
public:
	SoD3DModelKK();
	~SoD3DModelKK();
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
	SoD3DShaderBase* m_pShader;
	SoD3DShaderBase* m_pShadowShader;
	SoD3DTexture* m_pTexture;
	ID3D11ShaderResourceView* m_pNormalTexture;
	float m_fAccTime;
	int m_nCurrentKeyFrameIndex;

};
//----------------------------------------------------------------
#endif //_SoD3DModelKK_h_
//----------------------------------------------------------------
