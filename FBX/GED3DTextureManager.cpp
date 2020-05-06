//----------------------------------------------------------------
//1��������������ͼ�ֳ������࣬һ����UI����ʹ�õ���ͼ����һ���Ǹ���ģ��ʹ�õ���ͼ��
//   ����UI��ͼ�ʹ�UI��������ȥ���ң�����ģ����ͼ�ʹ�ģ����������ȥ���ң�Ч�ʻ��һ�㡣
//2�����ʹ��һ����ͼ���󣬾����Ӹ���ͼ��������ü���������ʹ��ʱ���ͷ����ü�����
//   ��������������ü���Ϊ0����ͼɾ������
//----------------------------------------------------------------
#include "GED3DTextureManager.h"
//----------------------------------------------------------------
GED3DTextureManager* GED3DTextureManager::ms_pInstance = NULL;
//----------------------------------------------------------------
bool GED3DTextureManager::CreateD3DTextureManager()
{
	bool br = true;
	if (ms_pInstance == NULL)
	{
		ms_pInstance = new GED3DTextureManager;
		if (ms_pInstance && ms_pInstance->InitD3DTextureManager())
		{
			br = true;
		}
		else
		{
			ReleaseD3DTextureManager();
			br = false;
		}
	}
	return br;
}
//----------------------------------------------------------------
void GED3DTextureManager::ReleaseD3DTextureManager()
{
	if (ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = NULL;
	}
}
//----------------------------------------------------------------
GED3DTextureManager::GED3DTextureManager()
:m_fAccTimeForGC(0.0f)
{

}
//----------------------------------------------------------------
GED3DTextureManager::~GED3DTextureManager()
{
	ClearD3DTextureManager();
}
//----------------------------------------------------------------
bool GED3DTextureManager::InitD3DTextureManager()
{
	if (m_kUITextureArray.InitArray(sizeof(GED3DTexture*), 10, 10) == false)
	{
		return false;
	}
	if (m_kModelTextureArray.InitArray(sizeof(GED3DTexture*), 10, 10) == false)
	{
		return false;
	}
	return true;
}
//----------------------------------------------------------------
void GED3DTextureManager::ClearD3DTextureManager()
{
	const int nUITexCount = m_kUITextureArray.GetCapacity();
	GED3DTexture* pTexture = NULL;
	for (int i = 0; i < nUITexCount; ++i)
	{
		pTexture = GetUITextureByID(i);
		if (pTexture)
		{
			delete pTexture;
		}
	}
	m_kUITextureArray.ClearArray();
	//
	const int nModelTexCount = m_kModelTextureArray.GetCapacity();
	pTexture = NULL;
	for (int i = 0; i < nModelTexCount; ++i)
	{
		pTexture = GetTextureByID(i);
		if (pTexture)
		{
			delete pTexture;
		}
	}
	m_kModelTextureArray.ClearArray();
	m_kModelTexName2Index.clear();
}
//----------------------------------------------------------------
void GED3DTextureManager::UpdateD3DTextureManager(float fDeltaTime)
{
	m_fAccTimeForGC += fDeltaTime;
	if (m_fAccTimeForGC > 1.0f)
	{
		m_fAccTimeForGC = 0.0f;
		ProcessGC();
	}
}
//----------------------------------------------------------------
void GED3DTextureManager::ProcessGC()
{
	//δ�����
}
//----------------------------------------------------------------
GED3DTexture* GED3DTextureManager::CreateUITextureFromFile(const wchar_t* szTexName)
{
	if (szTexName == 0 || szTexName[0] == 0)
	{
		return NULL;
	}

	GED3DTexture::stInitParamFromFile kParam;
	kParam.szFileName = szTexName;
	

	GED3DTexture* pTexture = new GED3DTexture;
	if (pTexture == NULL)
	{
		return NULL;
	}

	if (pTexture->InitD3DTextureFromFile(kParam) == false)
	{
		delete pTexture;
		pTexture = NULL;
		return NULL;
	}

	const int nTextureID = m_kUITextureArray.FillAt(-1, &pTexture);
	pTexture->SetTextureID(nTextureID);
	return pTexture;
}
//----------------------------------------------------------------
GED3DTexture* GED3DTextureManager::CreateUITextureDynamic(int nWidth, int nHeight, void* pPixelData)
{
	GED3DTexture::stInitParamDynamic kParam;

	kParam.kTexture2DDesc.Width = nWidth;
	kParam.kTexture2DDesc.Height = nHeight;
	kParam.kTexture2DDesc.MipLevels = 1;
	kParam.kTexture2DDesc.ArraySize = 1;
	kParam.kTexture2DDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //DXGI_FORMAT_R8G8B8A8_UINT; //DXGI_FORMAT_R16G16_FLOAT
	kParam.kTexture2DDesc.SampleDesc.Count = 1;
	kParam.kTexture2DDesc.SampleDesc.Quality = 0;
	kParam.kTexture2DDesc.Usage = D3D11_USAGE_DYNAMIC;
	kParam.kTexture2DDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	kParam.kTexture2DDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	kParam.kTexture2DDesc.MiscFlags = 0;

	const int nBytePerPixel = 4;
	kParam.kResourceData.pSysMem = pPixelData;
	kParam.kResourceData.SysMemPitch = nWidth * nBytePerPixel;
	kParam.kResourceData.SysMemSlicePitch = 0;

	GED3DTexture* pTexture = new GED3DTexture;
	if (pTexture == NULL)
	{
		return NULL;
	}

	if (pTexture->InitD3DTextureDynamic(kParam) == false)
	{
		delete pTexture;
		pTexture = NULL;
		return NULL;
	}

	const int nTextureID = m_kUITextureArray.FillAt(-1, &pTexture);
	pTexture->SetTextureID(nTextureID);
	return pTexture;
}
//----------------------------------------------------------------
GED3DTexture* GED3DTextureManager::GetUITextureByID(int nTextureID)
{
	void* pElement = m_kUITextureArray.GetAt(nTextureID);
	if (pElement)
	{
		return (*((GED3DTexture**)pElement));
	}
	else
	{
		return NULL;
	}
}
//----------------------------------------------------------------
GED3DTexture* GED3DTextureManager::CreateTextureFromFile(const std::wstring& strTexName)
{
	if (strTexName.empty() == true)
	{
		return NULL;
	}

	const int nExistTexID = GetTextureIDByName(strTexName);
	if (nExistTexID != -1)
	{
		return GetTextureByID(nExistTexID);
	}

	GED3DTexture::stInitParamFromFile kParam;
	kParam.szFileName = strTexName.c_str();
	
	GED3DTexture* pTexture = new GED3DTexture;
	if (pTexture == NULL)
	{
		return NULL;
	}

	if (pTexture->InitD3DTextureFromFile(kParam) == false)
	{
		delete pTexture;
		pTexture = NULL;
		return NULL;
	}

	const int nTextureID = m_kModelTextureArray.FillAt(-1, &pTexture);
	pTexture->SetTextureID(nTextureID);
	m_kModelTexName2Index.insert(std::make_pair(strTexName, nTextureID));
	return pTexture;
}
//----------------------------------------------------------------
GED3DTexture* GED3DTextureManager::CreateAnimDataTextureDynamic(int nWidth, int nHeight, void* pPixelData)
{
	const int nBytePerPixel = 4;

	GED3DTexture::stInitParamDynamic kParam;

	kParam.kTexture2DDesc.Width = nWidth;
	kParam.kTexture2DDesc.Height = nHeight;
	kParam.kTexture2DDesc.MipLevels = 1;
	kParam.kTexture2DDesc.ArraySize = 1;
	kParam.kTexture2DDesc.Format = DXGI_FORMAT_R32_FLOAT; //ÿ��������һ��32λ��floatֵ
	kParam.kTexture2DDesc.SampleDesc.Count = 1;
	kParam.kTexture2DDesc.SampleDesc.Quality = 0;
	kParam.kTexture2DDesc.Usage = D3D11_USAGE_DYNAMIC;
	kParam.kTexture2DDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	kParam.kTexture2DDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	kParam.kTexture2DDesc.MiscFlags = 0;


	kParam.kResourceData.pSysMem = pPixelData;
	kParam.kResourceData.SysMemPitch = nWidth * nBytePerPixel;
	kParam.kResourceData.SysMemSlicePitch = 0;

	GED3DTexture* pTexture = new GED3DTexture;
	if (pTexture == NULL)
	{
		return NULL;
	}

	if (pTexture->InitD3DTextureDynamic(kParam) == false)
	{
		delete pTexture;
		pTexture = NULL;
		return NULL;
	}

	const int nTextureID = m_kUITextureArray.FillAt(-1, &pTexture);
	pTexture->SetTextureID(nTextureID);
	return pTexture;
}
//----------------------------------------------------------------
GED3DTexture* GED3DTextureManager::GetTextureByID(int nTextureID)
{
	void* pElement = m_kModelTextureArray.GetAt(nTextureID);
	if (pElement)
	{
		return (*((GED3DTexture**)pElement));
	}
	else
	{
		return NULL;
	}
}
//----------------------------------------------------------------
int GED3DTextureManager::GetTextureIDByName(const std::wstring& strTexName)
{
	mapName2Index::iterator it = m_kModelTexName2Index.find(strTexName);
	if (it != m_kModelTexName2Index.end())
	{
		return it->second;
	}
	else
	{
		return -1;
	}
}
//----------------------------------------------------------------
