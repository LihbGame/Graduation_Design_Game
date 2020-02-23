#pragma once
#include "d3dUtil.h"
#include <DirectXCollision.h>
#include <vector>
#include "Camera.h"
class Ray
{

		Ray();
		Ray(const DirectX::XMFLOAT3& origin, const DirectX::XMFLOAT3& direction);

		static Ray ScreenToViewRay(const Camera& camera, float screenX, float screenY,int ScreenWidth,int ScreenHeight);
		static Ray ViewToLocalRay();
		bool Hit(const DirectX::BoundingBox& box, float* pOutDist = nullptr, float maxDist = FLT_MAX);
		bool Hit(const DirectX::BoundingOrientedBox& box, float* pOutDist = nullptr, float maxDist = FLT_MAX);
		bool Hit(const DirectX::BoundingSphere& sphere, float* pOutDist = nullptr, float maxDist = FLT_MAX);
		bool XM_CALLCONV Hit(DirectX::FXMVECTOR V0, DirectX::FXMVECTOR V1, DirectX::FXMVECTOR V2, float* pOutDist = nullptr, float maxDist = FLT_MAX);

		DirectX::XMFLOAT3 origin;		// 射线原点
		DirectX::XMFLOAT3 direction;	// 单位方向向量
};

