//----------------------------------------------------------------
#include "SoD3DShaderManager.h"
//----------------------------------------------------------------
SoD3DShaderManager* SoD3DShaderManager::ms_pInstance = 0;
//----------------------------------------------------------------
bool SoD3DShaderManager::CreateD3DShaderManager()
{
	bool br = true;
	if (ms_pInstance == 0)
	{
		ms_pInstance = new SoD3DShaderManager;
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
void SoD3DShaderManager::ReleaseD3DShaderManager()
{
	if (ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = 0;
	}
}
//----------------------------------------------------------------
SoD3DShaderManager::SoD3DShaderManager()
{
	for (int i = 0; i < SoD3DShaderType_Max; ++i)
	{
		m_pShaderList[i] = 0;
	}
}
//----------------------------------------------------------------
SoD3DShaderManager::~SoD3DShaderManager()
{
	ClearD3DShaderManager();
}
//----------------------------------------------------------------
bool SoD3DShaderManager::InitD3DShaderManager()
{
	
	//
	SoD3DShaderModelFbx* pShaderModelFbx = new SoD3DShaderModelFbx;
	if (pShaderModelFbx && pShaderModelFbx->InitShaderModelFbx())
	{
		pShaderModelFbx->SetType(SoD3DShaderType_ModelFbx);
		m_pShaderList[SoD3DShaderType_ModelFbx] = pShaderModelFbx;
	}
	else
	{
		return false;
	}
	//
	SoD3DShaderModelKK* pShaderModelKK = new SoD3DShaderModelKK;
	if (pShaderModelKK && pShaderModelKK->InitShaderModelKK())
	{
		pShaderModelKK->SetType(SoD3DShaderType_ModelKK);
		m_pShaderList[SoD3DShaderType_ModelKK] = pShaderModelKK;
	}
	else
	{
		return false;
	}
	//
	SoD3DShaderModelKKNoBone* pShaderModelKKNoBone = new SoD3DShaderModelKKNoBone;
	if (pShaderModelKKNoBone && pShaderModelKKNoBone->InitShaderModelKKNoBone())
	{
		pShaderModelKKNoBone->SetType(SoD3DShaderType_ModelKKNoBone);
		m_pShaderList[SoD3DShaderType_ModelKKNoBone] = pShaderModelKKNoBone;
	}
	else
	{
		return false;
	}
	//shadow kk
	FBXShadowMapShaderkk* pShadowMapShaderkk = new FBXShadowMapShaderkk;
	if (pShadowMapShaderkk && pShadowMapShaderkk->InitShadowShaderKK())
	{
		pShadowMapShaderkk->SetType(SoD3DShaderType_ShadowKK);
		m_pShaderList[SoD3DShaderType_ShadowKK] = pShadowMapShaderkk;
	}
	else
	{
		return false;
	}
	//shadow kk no bone
	/*FBXShadowMapShaderkkNoBone* pShadowMapShaderkkNoBone = new FBXShadowMapShaderkkNoBone;
	if (pShadowMapShaderkkNoBone && pShadowMapShaderkkNoBone->InitShadowShaderKKNoBone())
	{
		pShadowMapShaderkkNoBone->SetType(SoD3DShaderType_ShadowKKNoBone);
		m_pShaderList[SoD3DShaderType_ShadowKKNoBone] = pShadowMapShaderkkNoBone;
	}
	else
	{
		return false;
	}*/

	return true;
}
//----------------------------------------------------------------
void SoD3DShaderManager::ClearD3DShaderManager()
{
	for (int i = 0; i < SoD3DShaderType_Max; ++i)
	{
		if (m_pShaderList[i])
		{
			delete m_pShaderList[i];
			m_pShaderList[i] = 0;
		}
	}
}
//----------------------------------------------------------------
SoD3DShaderBase* SoD3DShaderManager::GetShader(int theType)
{
	if (theType >= 0 && theType < SoD3DShaderType_Max)
	{
		return m_pShaderList[theType];
	}
	else
	{
		return 0;
	}
}
//----------------------------------------------------------------
