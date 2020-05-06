//----------------------------------------------------------------
//1，对象的创建和删除都由GED3DTextureManager负责。
//2，创建贴图有如下几种方式：
//   a)静态贴图，从磁盘上加载一张图片做为贴图；
//   b)动态贴图，申请一块内存，动态设置PixelData；
//3，若干个逻辑使用同一个贴图对象，这是非常普遍的。本对象加入了引用计数功能，
//   在GED3DTextureManager中会判断引用计数，如果计数为0则删除贴图对象。
//----------------------------------------------------------------
#include "GED3DTexture.h"
#include "d3dApp.h"
//----------------------------------------------------------------
GED3DTexture::GED3DTexture()
:m_pTexture2D(NULL)
,m_pTextureSRV(NULL)
,m_nTextureID(-1)
,m_nTextureWidth(0)
,m_nTextureHeight(0)
,m_nRefCount(0)
{

}
//----------------------------------------------------------------
GED3DTexture::~GED3DTexture()
{
	ClearD3DTexture();
}
//----------------------------------------------------------------
bool GED3DTexture::InitD3DTextureFromFile(GED3DTexture::stInitParamFromFile& kParam)
{
	if (kParam.szFileName == 0 || kParam.szFileName[0] == 0)
	{
		return false;
	}
	if (D3DApp::Get() == NULL)
	{
		return false;
	}
	ID3D11Device* pD3DDevice = D3DApp::Get()->GetD3DDevice();
	if (pD3DDevice == NULL)
	{
		return false;
	}

	HRESULT hr = S_OK;
	ID3D11Resource* texResource = nullptr;
	hr=DirectX::CreateDDSTextureFromFile(pD3DDevice,
		kParam.szFileName, &texResource, &m_pTextureSRV);
	ReleaseCOM(texResource);
	//hr = D3DX11CreateShaderResourceViewFromFile(pD3DDevice, kParam.szFileName, &kParam.kLoadInfo, NULL, &m_pTextureSRV, NULL);
	if (FAILED(hr))
	{
		return false;
	}

	m_pTextureSRV->GetResource((ID3D11Resource**)&m_pTexture2D);
	if (m_pTexture2D == NULL)
	{
		ClearD3DTexture();
		return false;
	}

	D3D11_TEXTURE2D_DESC texDesc;
	m_pTexture2D->GetDesc(&texDesc);
	m_nTextureWidth = (int)texDesc.Width;
	m_nTextureHeight = (int)texDesc.Height;
	return true;
}
//----------------------------------------------------------------
bool GED3DTexture::InitD3DTextureDynamic(stInitParamDynamic& kParam)
{
	if (D3DApp::Get() == NULL)
	{
		return false;
	}
	ID3D11Device* pD3DDevice = D3DApp::Get()->GetD3DDevice();
	if (pD3DDevice == NULL)
	{
		return false;
	}

	HRESULT hr = pD3DDevice->CreateTexture2D(&kParam.kTexture2DDesc, &kParam.kResourceData, &m_pTexture2D);
	if (FAILED(hr))
	{
		return false;
	}

	hr = pD3DDevice->CreateShaderResourceView(m_pTexture2D, NULL, &m_pTextureSRV);
	if (FAILED(hr))
	{
		ClearD3DTexture();
		return false;
	}

	m_nTextureWidth = (int)kParam.kTexture2DDesc.Width;
	m_nTextureHeight = (int)kParam.kTexture2DDesc.Height;
	return true;
}
//----------------------------------------------------------------
void GED3DTexture::ClearD3DTexture()
{
	if (m_pTexture2D)
	{
		m_pTexture2D->Release();
		m_pTexture2D = NULL;
	}
	if (m_pTextureSRV)
	{
		m_pTextureSRV->Release();
		m_pTextureSRV = NULL;
	}
	m_nTextureID = -1;
	m_nTextureWidth = 0;
	m_nTextureHeight = 0;
	m_nRefCount = 0;
}
//----------------------------------------------------------------
bool GED3DTexture::SetDynamicTexturePixelData(void* pPixelData, int nSizeOfPixelData)
{
	if (pPixelData == NULL)
	{
		return false;
	}
	if (m_pTexture2D == NULL)
	{
		return false;
	}
	if (D3DApp::Get() == NULL)
	{
		return false;
	}
	ID3D11DeviceContext* pD3DDeviceContext = D3DApp::Get()->GetD3DDeviceContext();
	if (pD3DDeviceContext == NULL)
	{
		return false;
	}

	D3D11_MAPPED_SUBRESOURCE kMappedResource;
	memset(&kMappedResource, 0, sizeof(kMappedResource));
	HRESULT hr = pD3DDeviceContext->Map(m_pTexture2D, 0, D3D11_MAP_WRITE_DISCARD, 0, &kMappedResource);
	if (SUCCEEDED(hr))
	{
		memcpy(kMappedResource.pData, pPixelData, nSizeOfPixelData);
		pD3DDeviceContext->Unmap(m_pTexture2D, 0);
	}
	return SUCCEEDED(hr);
}
//----------------------------------------------------------------
bool GED3DTexture::GetTexturePixelData(void* pBuff, int nSizeOfBuff)
{
	if (pBuff == NULL)
	{
		return false;
	}
	if (m_pTexture2D == NULL)
	{
		return false;
	}
	if (D3DApp::Get() == NULL)
	{
		return false;
	}
	ID3D11DeviceContext* pD3DDeviceContext = D3DApp::Get()->GetD3DDeviceContext();
	if (pD3DDeviceContext == NULL)
	{
		return false;
	}

	D3D11_MAPPED_SUBRESOURCE kMappedResource;
	memset(&kMappedResource, 0, sizeof(kMappedResource));
	HRESULT hr = pD3DDeviceContext->Map(m_pTexture2D, 0, D3D11_MAP_READ, 0, &kMappedResource);
	if (SUCCEEDED(hr))
	{
		memcpy(pBuff, kMappedResource.pData, nSizeOfBuff);
		pD3DDeviceContext->Unmap(m_pTexture2D, 0);
	}
	return SUCCEEDED(hr);
}
//----------------------------------------------------------------
