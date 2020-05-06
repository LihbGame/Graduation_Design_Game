//----------------------------------------------------------------
#ifndef _GED3DTextureManager_h_
#define _GED3DTextureManager_h_
//----------------------------------------------------------------
#include "GED3DTexture.h"
//----------------------------------------------------------------
class GED3DTextureManager
{
public:
	static bool CreateD3DTextureManager();
	static void ReleaseD3DTextureManager();
	static GED3DTextureManager* Get();

	void UpdateD3DTextureManager(float fDeltaTime);

	GED3DTexture* CreateUITextureFromFile(const wchar_t* szTexName);
	GED3DTexture* CreateUITextureDynamic(int nWidth, int nHeight, void* pPixelData);
	GED3DTexture* GetUITextureByID(int nTextureID);

	GED3DTexture* CreateTextureFromFile(const std::wstring& strTexName);
	GED3DTexture* CreateAnimDataTextureDynamic(int nWidth, int nHeight, void* pPixelData);
	GED3DTexture* GetTextureByID(int nTextureID);
	int GetTextureIDByName(const std::wstring& strTexName);

private:
	GED3DTextureManager();
	~GED3DTextureManager();
	bool InitD3DTextureManager();
	void ClearD3DTextureManager();
	void ProcessGC();

private:
	typedef std::map<std::wstring, int> mapName2Index;

private:
	static GED3DTextureManager* ms_pInstance;
	GEArrayUID m_kUITextureArray;
	GEArrayUID m_kModelTextureArray;
	mapName2Index m_kModelTexName2Index;

	float m_fAccTimeForGC;
};
//----------------------------------------------------------------
inline GED3DTextureManager* GED3DTextureManager::Get()
{
	return ms_pInstance;
}
//----------------------------------------------------------------
#endif //_GED3DTextureManager_h_
//----------------------------------------------------------------
