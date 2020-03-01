#ifndef TERRAIN_H
#define TERRAIN_H

#include "d3dUtil.h"
#include <vector>
#include "d3d11.h"
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
	void Render(ID3D11DeviceContext* DeviceContext, DirectionalLight& DirLight);
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
	ID3D11ShaderResourceView* mSRV[5];
};





#endif // TERRAIN_H