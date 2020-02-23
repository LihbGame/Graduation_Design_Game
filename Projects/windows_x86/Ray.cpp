#include "Ray.h"
#include "d3dApp.h"
using namespace DirectX;

Ray::Ray()
	: origin(), direction(0.0f, 0.0f, 1.0f)
{
}

Ray::Ray(const DirectX::XMFLOAT3& origin, const DirectX::XMFLOAT3& direction)
	: origin(origin)
{
	// 射线的direction长度必须为1.0f，误差在1e-5f内
	XMVECTOR dirLength = XMVector3Length(XMLoadFloat3(&direction));
	XMVECTOR error = XMVectorAbs(dirLength - XMVectorSplatOne());
	assert(XMVector3Less(error, XMVectorReplicate(1e-5f)));

	XMStoreFloat3(&this->direction, XMVector3Normalize(XMLoadFloat3(&direction)));
}

Ray Ray::ScreenToViewRay(const Camera& camera, float screenX, float screenY, int ScreenWidth, int ScreenHeight)
{
	XMFLOAT4X4 P;
	XMStoreFloat4x4(&P, camera.Proj());


	// Compute picking ray in view space.
	float vx = (+2.0f * screenX / ScreenWidth - 1.0f) / P(0, 0);
	float vy = (-2.0f * screenY / ScreenHeight + 1.0f) / P(1, 1);

	// Ray definition in view space.
	XMVECTOR rayOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR rayDir = XMVectorSet(vx, vy, 1.0f, 0.0f);

	XMMATRIX V = camera.View();
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(V), V);

	rayOrigin = XMVector3TransformCoord(rayOrigin, invView);
	rayDir = XMVector3TransformNormal(rayDir, invView);

	// Make the ray direction unit length for the intersection tests.
	rayDir = XMVector3Normalize(rayDir);




	// 求出射线
	XMFLOAT3 Org;
	XMStoreFloat3(&Org,rayOrigin);
	
	XMFLOAT3 direction;
	XMStoreFloat3(&Org, rayDir);
	
	return Ray(Org, direction);
}

Ray Ray::ViewToLocalRay(XMMATRIX& WorldMX,Ray& ViewRay)
{
	XMMATRIX W = WorldMX;
	XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(W), W);


	XMVECTOR rayOrigin = XMVector3TransformCoord(XMLoadFloat3(&ViewRay.origin), invWorld);
	XMVECTOR rayDir = XMVector3TransformNormal(XMLoadFloat3(&ViewRay.direction), invWorld);


	return Ray();
}

bool Ray::Hit(const DirectX::BoundingBox& box, float* pOutDist, float maxDist)
{

	float dist;
	bool res = box.Intersects(XMLoadFloat3(&origin), XMLoadFloat3(&direction), dist);
	if (pOutDist)
		*pOutDist = dist;
	return dist > maxDist ? false : res;
}

bool Ray::Hit(const DirectX::BoundingOrientedBox& box, float* pOutDist, float maxDist)
{
	float dist;
	bool res = box.Intersects(XMLoadFloat3(&origin), XMLoadFloat3(&direction), dist);
	if (pOutDist)
		*pOutDist = dist;
	return dist > maxDist ? false : res;
}

bool Ray::Hit(const DirectX::BoundingSphere& sphere, float* pOutDist, float maxDist)
{
	float dist;
	bool res = sphere.Intersects(XMLoadFloat3(&origin), XMLoadFloat3(&direction), dist);
	if (pOutDist)
		*pOutDist = dist;
	return dist > maxDist ? false : res;
}

bool XM_CALLCONV Ray::Hit(FXMVECTOR V0, FXMVECTOR V1, FXMVECTOR V2, float* pOutDist, float maxDist)
{
	float dist;
	bool res = TriangleTests::Intersects(XMLoadFloat3(&origin), XMLoadFloat3(&direction), V0, V1, V2, dist);
	if (pOutDist)
		*pOutDist = dist;
	return dist > maxDist ? false : res;
}