#pragma once
#include "d3dUtil.h"
class Water
{
public:
	Water(ID3D11Device* device, UINT width, UINT height);
	~Water();
	


	ID3D11ShaderResourceView* ReflectionSRV();
	ID3D11ShaderResourceView* RefractionSRV();
	ID3D11ShaderResourceView* SingleReflectionSRV();
	ID3D11ShaderResourceView* SingleRefractionSRV();


	ID3D11RenderTargetView* ReflectionRTV();
	ID3D11DepthStencilView* ReflectionDepthMapDSV();
private:
	Water(const Water& rhs);
	Water& operator=(const Water& rhs);
private:
	UINT mWidth;
	UINT mHeight;

	ID3D11ShaderResourceView* mReflectionSRV;
	ID3D11ShaderResourceView* mRefractionSRV;

	ID3D11ShaderResourceView* mSingleReflectionSRV;
	ID3D11ShaderResourceView* mSingleRefractionSRV;

	ID3D11RenderTargetView* mReflectionRTV;
	ID3D11DepthStencilView* mDepthMapDSV;

};