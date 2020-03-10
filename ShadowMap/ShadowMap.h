#ifndef SHADOW_MAPPER_H
#define SHADOW_MAPPER_H

#include "d3dUtil.h"
#include "Camera.h"
#include "LightHelper.h"
struct Boundingsphere
{
	Boundingsphere() : Center(0.0f, 0.0f, 0.0f), Radius(0.0f) {}
	XMFLOAT3 Center;
	float Radius;
};

class ShadowMap
{
public:
	ShadowMap(ID3D11Device* device, UINT width, UINT height);
	~ShadowMap();
	ShadowMap& operator=(const ShadowMap& rhs);


	ID3D11ShaderResourceView* DepthMapSRV();

	void BindDsvAndSetNullRenderTarget(ID3D11DeviceContext* dc);
	void BuildShadowTransform(DirectionalLight& mDirLights);

	XMFLOAT4X4& GetLightViewProj() {return mLightViewProj;}
	XMFLOAT4X4& GetShadowTransform() {return mShadowTransform;}
private:
	ShadowMap(const ShadowMap& rhs);
	
private:
	UINT mWidth;
	UINT mHeight;

	ID3D11ShaderResourceView* mDepthMapSRV;
	ID3D11DepthStencilView* mDepthMapDSV;

	D3D11_VIEWPORT mViewport;

	Boundingsphere mSceneBounds;
	XMFLOAT4X4 mLightViewProj;
	XMFLOAT4X4 mShadowTransform;
};

#endif // SHADOW_MAPPER_H