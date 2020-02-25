//----------------------------------------------------------------
#ifndef _SoD3DLogicFlowHelp_h_
#define _SoD3DLogicFlowHelp_h_
//----------------------------------------------------------------
#include "SoD3DLogicFlowHelp.h"
#include "d3dApp.h"
#include "SoD3DTextureManager.h"
#include "SoD3DShaderManager.h"
#include "SoD3DModelManager.h"
#include "Camera.h"
#include "StFBXManager.h"
#include "StFBXModel.h"
#include "StKKFileKkbWrite.h"
#include "StKKFileKkbRead.h"
#include "StKKFileKkfWrite.h"
#include "StKKFileKkfRead.h"
#include "StKKModel.h"
#include "StKKFileKkmRead.h"
#include <SoStringHelp.h>

#define Model_num  2

bool SoD3DLogicFlowHelp_Create();
void SoD3DLogicFlowHelp_PreRender();
void SoD3DLogicFlowHelp_EndRender();



class Model
{
public:
	Model();
	~Model() { Release(); };
	//have animation
	bool BeginRender(int Instance_num, int nAnimID);
	//no animation
	bool BeginRender(int Instance_num);
	void Update(float fDeltaTime,int nAnimID);
	void Release();
	void CreateFileFbx(const char* szFileName);
	void CreateFileKkb(const char* szFileName);
	void CreateFileKkf(const char* szFileName, int nAnimID);
	void CreateImage(const wchar_t* szFileName);
	void ConvertFbx2KK();
	void LoadFbxAnim();
	void SetModelTansInfo(Model_Tansform_Info *ModeInfo);

private:
	SoD3DModelFbx* g_pD3DModelFbx = 0;
	StFBXModel* g_pFbxModel = 0;
	SoD3DModelKK* g_pD3DModelKK = 0;
	StKKModel* g_pKKModel = 0;
	char g_buff[512] = "";
	Model_Tansform_Info m_ModeInfo;
};
//----------------------------------------------------------------
#endif //_SoD3DLogicFlowHelp_h_
//----------------------------------------------------------------
