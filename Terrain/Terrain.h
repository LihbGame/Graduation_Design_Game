#ifndef TERRAIN_H
#define TERRAIN_H

#include <vector>
#include "d3dUtil.h"
#include "d3d11.h"
#include"ShadowMap.h"
#include "SenceManager.h"
#include "d3dApp.h"
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
	void InitTerrain(ID3D11Device* md3dDevice, int mapSize, int UnitMapOffset, std::vector<UnitSence>& sence);
	void InitModel(ID3D11Device* md3dDevice, int mapSize, int UnitMapOffset, std::vector<UnitSence>& sence, std::vector <Model_Tansform_Info*>& PlayerInfo);

	void Render(ID3D11DeviceContext* DeviceContext, DirectionalLight& DirLight, ShadowMap* shadowmap,bool isClip);
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