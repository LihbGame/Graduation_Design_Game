#ifndef TERRAIN_H
#define TERRAIN_H

#include <vector>
#include "d3dUtil.h"
#include "d3d11.h"
#include"ShadowMap.h"
struct InstancedData
{
	XMFLOAT4X4 World;
	int TexIndex;
};




class Terrain
{
public:
	Terrain();
	~Terrain();
	void Init(ID3D11Device* md3dDevice,int MapSize,int UnitMapOffset);
	void Render(ID3D11DeviceContext* DeviceContext, DirectionalLight& DirLight,ShadowMap* shadowmap);
	void BuildCrateGeometryBuffers(ID3D11Device* md3dDevice);
	void LoadTexture(ID3D11Device* md3dDevice);

	std::vector<InstancedData> mInstance;
private:
	ID3D11Buffer* mInstancedBuffer;
	ID3D11Buffer* mBoxVB;
	ID3D11Buffer* mBoxIB;
	Material mTerrainMat;
	int mIndexCount;
	int mVisibleObjectCount;
	ID3D11ShaderResourceView* mDiffuseSRV[5];
	ID3D11ShaderResourceView* mNormalSRV[5];
};





#endif // TERRAIN_H