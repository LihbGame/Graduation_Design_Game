#include "Water.h"

Water::Water(ID3D11Device* device, UINT width, UINT height)
	: mWidth(width),
	mHeight(height), 
	mReflectionSRV(0),
	mRefractionSRV(0), 
	mReflectionRTV(0),
	mDepthMapDSV(0)
{									
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = mWidth;
	texDesc.Height = mHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count = 4;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	ID3D11Texture2D* ReflectionMap = 0;
	ID3D11Texture2D* RefractionMap = 0;
	HR(device->CreateTexture2D(&texDesc, 0, &ReflectionMap));
	texDesc.BindFlags =  D3D11_BIND_SHADER_RESOURCE;
	HR(device->CreateTexture2D(&texDesc, 0, &RefractionMap));
	
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
	rtvDesc.Texture2D.MipSlice = 0;
	HR(device->CreateRenderTargetView(ReflectionMap, &rtvDesc, &mReflectionRTV));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	HR(device->CreateShaderResourceView(ReflectionMap, &srvDesc, &mReflectionSRV));
	HR(device->CreateShaderResourceView(RefractionMap, &srvDesc, &mRefractionSRV));


	//depth map
	D3D11_TEXTURE2D_DESC DepthtexDesc;
	DepthtexDesc.Width = mWidth;
	DepthtexDesc.Height = mHeight;
	DepthtexDesc.MipLevels = 1;
	DepthtexDesc.ArraySize = 1;
	DepthtexDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	DepthtexDesc.SampleDesc.Count = 4;
	DepthtexDesc.SampleDesc.Quality = 0;
	DepthtexDesc.Usage = D3D11_USAGE_DEFAULT;
	DepthtexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL ;
	DepthtexDesc.CPUAccessFlags = 0;
	DepthtexDesc.MiscFlags = 0;

	ID3D11Texture2D* depthMap = 0;
	HR(device->CreateTexture2D(&DepthtexDesc, 0, &depthMap));

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = 0;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	dsvDesc.Texture2D.MipSlice = 0;
	HR(device->CreateDepthStencilView(depthMap, &dsvDesc, &mDepthMapDSV));




	//single sample srv
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags =  D3D11_BIND_SHADER_RESOURCE;
	ID3D11Texture2D* SingleReflectionMap = 0;
	ID3D11Texture2D* SingleRefractionMap = 0;
	HR(device->CreateTexture2D(&texDesc, 0, &SingleReflectionMap));
	HR(device->CreateTexture2D(&texDesc, 0, &SingleRefractionMap));

	D3D11_SHADER_RESOURCE_VIEW_DESC SinglesrvDesc;
	SinglesrvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SinglesrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SinglesrvDesc.Texture2D.MipLevels = texDesc.MipLevels;
	SinglesrvDesc.Texture2D.MostDetailedMip = 0;
	HR(device->CreateShaderResourceView(SingleReflectionMap, &SinglesrvDesc, &mSingleReflectionSRV));
	HR(device->CreateShaderResourceView(SingleRefractionMap, &SinglesrvDesc, &mSingleRefractionSRV));



	// View saves a reference to the texture so we can release our reference.
	ReleaseCOM(ReflectionMap);
	ReleaseCOM(RefractionMap);
	ReleaseCOM(depthMap);
	ReleaseCOM(SingleReflectionMap);
	ReleaseCOM(SingleRefractionMap);
}									 

Water::~Water()
{
	ReleaseCOM(mReflectionSRV);
	ReleaseCOM(mRefractionSRV);
	ReleaseCOM(mReflectionRTV);
	ReleaseCOM(mDepthMapDSV);
	ReleaseCOM(mSingleReflectionSRV);
	ReleaseCOM(mSingleRefractionSRV);
}

ID3D11ShaderResourceView* Water::ReflectionSRV()
{
	return mReflectionSRV;
}

ID3D11ShaderResourceView* Water::RefractionSRV()
{
	return mRefractionSRV;
}

ID3D11ShaderResourceView* Water::SingleReflectionSRV()
{
	return mSingleReflectionSRV;
}

ID3D11ShaderResourceView* Water::SingleRefractionSRV()
{
	return mSingleRefractionSRV;
}

ID3D11RenderTargetView* Water::ReflectionRTV()
{
	return mReflectionRTV;
}

ID3D11DepthStencilView* Water::ReflectionDepthMapDSV()
{
	return mDepthMapDSV;
}



