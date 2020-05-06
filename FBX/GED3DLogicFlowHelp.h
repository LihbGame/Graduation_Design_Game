//----------------------------------------------------------------
#ifndef _GED3DLogicFlowHelp_h_
#define _GED3DLogicFlowHelp_h_
//----------------------------------------------------------------
#include "GED3DLogicFlowHelp.h"
#include "d3dApp.h"
#include "GED3DTextureManager.h"
#include "GED3DShaderManager.h"
#include "GED3DModelManager.h"
#include "Camera.h"
#include "StFBXManager.h"
#include "StFBXModel.h"
#include "StKKFileKkbWrite.h"
#include "StKKFileKkbRead.h"
#include "StKKFileKkfWrite.h"
#include "StKKFileKkfRead.h"
#include "StKKModel.h"
#include "StKKFileKkmRead.h"
#include "ShadowMap.h"
#include <GEStringHelp.h>

#define Model_num  2

bool GED3DLogicFlowHelp_Create();
void GED3DLogicFlowHelp_PreRender();
void GED3DLogicFlowHelp_EndRender();



class Model
{
public:
	Model();
	~Model() { Release(); };
	//have animation
	bool BeginRender(int Instance_num, int nAnimID);
	bool ShadowRender(int Instance_num, int nAnimID, ShadowMap* shadowmap);
	//no animation
	bool BeginRender(int Instance_num);
	bool ShadowRender(int InstanceCount, ShadowMap* shadowmap,ID3D11Buffer* InstanceVB);

	void Update(float fDeltaTime,int nAnimID);
	void Release();
	void CreateFileFbx(const char* szFileName);
	void CreateFileKkb(const char* szFileName);
	void CreateFileKkf(const char* szFileName, int nAnimID);
	void CreateImage(const wchar_t* szFileName);
	void CreateNormalTexture(const wchar_t* szFileName);
	void ConvertFbx2KK();
	void LoadFbxAnim();
	void SetModelTansInfo(Model_Tansform_Info *ModeInfo);



	GED3DModelFbx* g_pD3DModelFbx = 0;
	StFBXModel* g_pFbxModel = 0;
	GED3DModelKK* g_pD3DModelKK = 0;
	StKKModel* g_pKKModel = 0;
private:
	char g_buff[512] = "";
	Model_Tansform_Info m_ModeInfo;
};
//----------------------------------------------------------------
#endif //_GED3DLogicFlowHelp_h_
//----------------------------------------------------------------
