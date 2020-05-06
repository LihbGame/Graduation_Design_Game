//----------------------------------------------------------------
#ifndef _GED3DShaderManager_h_
#define _GED3DShaderManager_h_
//----------------------------------------------------------------

#include "GED3DShaderModelFbx.h"
#include "GED3DShaderModelKK.h"
#include "GED3DShaderModelKKNoBone.h"
#include "FBXShadowMapShaderkk.h"
#include "FBXShadowMapShaderkkNoBone.h"

//----------------------------------------------------------------
class GED3DShaderManager
{
public:
	static bool CreateD3DShaderManager();
	static void ReleaseD3DShaderManager();
	static GED3DShaderManager* Get();

	GED3DShaderBase* GetShader(int theType);

private:
	GED3DShaderManager();
	~GED3DShaderManager();
	bool InitD3DShaderManager();
	void ClearD3DShaderManager();

private:
	static GED3DShaderManager* ms_pInstance;
	GED3DShaderBase* m_pShaderList[GED3DShaderType_Max];
};
//----------------------------------------------------------------
inline GED3DShaderManager* GED3DShaderManager::Get()
{
	return ms_pInstance;
}
//----------------------------------------------------------------
#endif //_GED3DShaderManager_h_
//----------------------------------------------------------------
