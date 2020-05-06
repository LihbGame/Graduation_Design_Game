//----------------------------------------------------------------
#include "GED3DShaderManager.h"
//----------------------------------------------------------------
GED3DShaderManager* GED3DShaderManager::ms_pInstance = 0;
//----------------------------------------------------------------
bool GED3DShaderManager::CreateD3DShaderManager()
{
	bool br = true;
	if (ms_pInstance == 0)
	{
		ms_pInstance = new GED3DShaderManager;
		if (ms_pInstance && ms_pInstance->InitD3DShaderManager())
		{
			br = true;
		}
		else
		{
			ReleaseD3DShaderManager();
			br = false;
		}
	}
	return br;
}
//----------------------------------------------------------------
void GED3DShaderManager::ReleaseD3DShaderManager()
{
	if (ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = 0;
	}
}
//----------------------------------------------------------------
GED3DShaderManager::GED3DShaderManager()
{
	for (int i = 0; i < GED3DShaderType_Max; ++i)
	{
		m_pShaderList[i] = 0;
	}
}
//----------------------------------------------------------------
GED3DShaderManager::~GED3DShaderManager()
{
	ClearD3DShaderManager();
}
//----------------------------------------------------------------
bool GED3DShaderManager::InitD3DShaderManager()
{
	
	//
	GED3DShaderModelFbx* pShaderModelFbx = new GED3DShaderModelFbx;
	if (pShaderModelFbx && pShaderModelFbx->InitShaderModelFbx())
	{
		pShaderModelFbx->SetType(GED3DShaderType_ModelFbx);
		m_pShaderList[GED3DShaderType_ModelFbx] = pShaderModelFbx;
	}
	else
	{
		return false;
	}
	//
	GED3DShaderModelKK* pShaderModelKK = new GED3DShaderModelKK;
	if (pShaderModelKK && pShaderModelKK->InitShaderModelKK())
	{
		pShaderModelKK->SetType(GED3DShaderType_ModelKK);
		m_pShaderList[GED3DShaderType_ModelKK] = pShaderModelKK;
	}
	else
	{
		return false;
	}
	//
	GED3DShaderModelKKNoBone* pShaderModelKKNoBone = new GED3DShaderModelKKNoBone;
	if (pShaderModelKKNoBone && pShaderModelKKNoBone->InitShaderModelKKNoBone())
	{
		pShaderModelKKNoBone->SetType(GED3DShaderType_ModelKKNoBone);
		m_pShaderList[GED3DShaderType_ModelKKNoBone] = pShaderModelKKNoBone;
	}
	else
	{
		return false;
	}
	//shadow kk
	FBXShadowMapShaderkk* pShadowMapShaderkk = new FBXShadowMapShaderkk;
	if (pShadowMapShaderkk && pShadowMapShaderkk->InitShadowShaderKK())
	{
		pShadowMapShaderkk->SetType(GED3DShaderType_ShadowKK);
		m_pShaderList[GED3DShaderType_ShadowKK] = pShadowMapShaderkk;
	}
	else
	{
		return false;
	}
	//shadow kk no bone
	FBXShadowMapShaderkkNoBone* pShadowMapShaderkkNoBone = new FBXShadowMapShaderkkNoBone;
	if (pShadowMapShaderkkNoBone && pShadowMapShaderkkNoBone->InitShadowShaderKKNoBone())
	{
		pShadowMapShaderkkNoBone->SetType(GED3DShaderType_ShadowKKNoBone);
		m_pShaderList[GED3DShaderType_ShadowKKNoBone] = pShadowMapShaderkkNoBone;
	}
	else
	{
		return false;
	}

	return true;
}
//----------------------------------------------------------------
void GED3DShaderManager::ClearD3DShaderManager()
{
	for (int i = 0; i < GED3DShaderType_Max; ++i)
	{
		if (m_pShaderList[i])
		{
			delete m_pShaderList[i];
			m_pShaderList[i] = 0;
		}
	}
}
//----------------------------------------------------------------
GED3DShaderBase* GED3DShaderManager::GetShader(int theType)
{
	if (theType >= 0 && theType < GED3DShaderType_Max)
	{
		return m_pShaderList[theType];
	}
	else
	{
		return 0;
	}
}
//----------------------------------------------------------------
