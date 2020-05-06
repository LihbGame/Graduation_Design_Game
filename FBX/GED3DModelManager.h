//----------------------------------------------------------------
#ifndef _GED3DModelManager_h_
#define _GED3DModelManager_h_
//----------------------------------------------------------------
//#include "GED3DModelCube.h"
#include "GED3DModelFbx.h"
#include "GED3DModelKK.h"
//----------------------------------------------------------------
class GED3DModelManager
{
public:
	static bool CreateD3DModelManager();
	static void ReleaseD3DModelManager();
	static GED3DModelManager* Get();

	GED3DModelBase* CreateModel(int theType, void* pInitParam);
	void DeleteModel(int nModelID);
	GED3DModelBase* GetModel(int nModelID);

private:
	GED3DModelManager();
	~GED3DModelManager();
	bool InitD3DModelManager();
	void ClearD3DModelManager();
	void DoDeleteModel(GED3DModelBase* pModel);

private:
	static GED3DModelManager* ms_pInstance;
	GEArrayUID m_kModelArray;
};
//----------------------------------------------------------------
inline GED3DModelManager* GED3DModelManager::Get()
{
	return ms_pInstance;
}
//----------------------------------------------------------------
#endif //_GED3DModelManager_h_
//----------------------------------------------------------------
