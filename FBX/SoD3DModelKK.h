//----------------------------------------------------------------
#ifndef _SoD3DModelKK_h_
#define _SoD3DModelKK_h_
//----------------------------------------------------------------
#include "SoD3DModelBase.h"
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
	//��д����ĺ���
	bool InitModel(void* pInitParam);
	void ClearModel();
	void UpdateModel(float fDeltaTime, int nAnimID);
	void RenderModel(Model_Tansform_Info* mode_info, int instance_num, int nAnimID);

	void SetWorldMatrix(const XMFLOAT4X4* pMatWorld);
	void SetTexture(const wchar_t* szFileName);

	//��������ϵ����任��
	XMFLOAT4X4 m_matWorld;

private:
	bool CreateTexture(const wchar_t* szFileName);
	void ReleaseTexture();

private:
	const StKKModel* m_pKKModel;
	SoD3DShaderBase* m_pShader;
	SoD3DTexture* m_pTexture;
	
	float m_fAccTime;
	int m_nCurrentKeyFrameIndex;

};
//----------------------------------------------------------------
#endif //_SoD3DModelKK_h_
//----------------------------------------------------------------
