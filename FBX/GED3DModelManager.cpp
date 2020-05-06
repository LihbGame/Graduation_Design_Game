//----------------------------------------------------------------
#include "GED3DModelManager.h"
//----------------------------------------------------------------
GED3DModelManager* GED3DModelManager::ms_pInstance = NULL;
//----------------------------------------------------------------
bool GED3DModelManager::CreateD3DModelManager()
{
	bool br = true;
	if (ms_pInstance == NULL)
	{
		ms_pInstance = new GED3DModelManager;
		if (ms_pInstance && ms_pInstance->InitD3DModelManager())
		{
			br = true;
		}
		else
		{
			ReleaseD3DModelManager();
			br = false;
		}
	}
	return br;
}
//----------------------------------------------------------------
void GED3DModelManager::ReleaseD3DModelManager()
{
	if (ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = NULL;
	}
}
//----------------------------------------------------------------
GED3DModelManager::GED3DModelManager()
{

}
//----------------------------------------------------------------
GED3DModelManager::~GED3DModelManager()
{
	ClearD3DModelManager();
}
//----------------------------------------------------------------
bool GED3DModelManager::InitD3DModelManager()
{
	if (m_kModelArray.InitArray(sizeof(GED3DModelBase*), 10, 10) == false)
	{
		return false;
	}
	return true;
}
//----------------------------------------------------------------
void GED3DModelManager::ClearD3DModelManager()
{
	GED3DModelBase* pModel = 0;
	const int nModelCount = m_kModelArray.GetCapacity();
	for (int i = 0; i < nModelCount; ++i)
	{
		pModel = GetModel(i);
		if (pModel)
		{
			DoDeleteModel(pModel);
		}
	}
	m_kModelArray.ClearArray();
}
//----------------------------------------------------------------
GED3DModelBase* GED3DModelManager::CreateModel(int theType, void* pInitParam)
{
	GED3DModelBase* pModel = NULL;
	switch (theType)
	{
		case GED3DModelType_Cube:
		{
			//pModel = new GED3DModelCube;
		} break;
		default:
		{
			GELogError("GED3DModelManager::CreateModel : Unknown ModelType [%d]", theType);
		} break;
	}

	if (pModel)
	{
		if (pModel->InitModel(pInitParam))
		{
			pModel->SetModelType((GED3DModelType)theType);
			const int nModelID = m_kModelArray.FillAt(-1, &pModel);
			pModel->SetModelID(nModelID);
		}
		else
		{
			DoDeleteModel(pModel);
			pModel = NULL;
		}
	}
	return pModel;
}
//----------------------------------------------------------------
void GED3DModelManager::DeleteModel(int nModelID)
{
	GED3DModelBase* pModel = GetModel(nModelID);
	if (pModel)
	{
		DoDeleteModel(pModel);
		m_kModelArray.ClearAt(nModelID);
	}
}
//----------------------------------------------------------------
void GED3DModelManager::DoDeleteModel(GED3DModelBase* pModel)
{
	if (pModel)
	{
		pModel->ClearModel();
		delete pModel;
		pModel = 0;
	}
}
//----------------------------------------------------------------
GED3DModelBase* GED3DModelManager::GetModel(int nModelID)
{
	void* pElement = m_kModelArray.GetAt(nModelID);
	if (pElement)
	{
		return *((GED3DModelBase**)pElement);
	}
	else
	{
		return NULL;
	}
}
//----------------------------------------------------------------
