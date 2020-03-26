#include "Terrain.h"
#include "GeometryGenerator.h"
#include "MathHelper.h"
#include <Vertex.h>
#include "Effects.h"
#include "Camera.h"
#include "RenderStates.h"

using namespace std;

Terrain::Terrain():mBoxVB(0), mBoxIB(0)
{
}

Terrain::~Terrain()
{
	ReleaseCOM(mBoxVB);
	ReleaseCOM(mBoxIB);
}

void Terrain::InitTerrain(ID3D11Device* md3dDevice,int MapSize, int UnitMapOffset, std::vector<UnitSence>& sence)
{
	XMMATRIX boxScale = XMMatrixScaling(15.0f, 15.0f, 15.0f);
	XMMATRIX boxOffset = XMMatrixTranslation(0.0f, 0.0f, 0.0f);

	float offsetX = 0.0f;
	float offsetY = 0.0f;
	float offsetZ = 0.0f;
	InstancedData temp;
	for (int index = 0; index < sence.size(); ++index)
	{
		offsetX = sence[index].SenceOffset.x;
		offsetY = sence[index].SenceOffset.y;
		offsetZ = sence[index].SenceOffset.z;
		for (int i = 0; i < MapSize; ++i)
		{
			for (int j = 0; j < MapSize; ++j)
			{
				if (sence[index].SenceData[i][j] == '+')
				{
					boxOffset = XMMatrixTranslation(offsetX + i * UnitMapOffset, 0.0f, offsetZ + j * UnitMapOffset);
					XMStoreFloat4x4(&temp.World, boxScale * boxOffset);
					temp.TexIndex = rand() % 4;
					//实例化数据
					mInstance.push_back(temp);
					//场景数据
					 sence[index].TansformData.push_back(temp.World);
				}
			}
		}
	}


	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(InstancedData) * mInstance.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &mInstance[0];
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mInstancedBuffer));

	//VB AND IB
	BuildCrateGeometryBuffers(md3dDevice);
	//Visible Object Count
	mVisibleObjectCount = mInstance.size();
	//LOAD TEXTRUE
	LoadTexture(md3dDevice);
	//mai init
	mTerrainMat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mTerrainMat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mTerrainMat.Specular = XMFLOAT4(0.9f, 0.9f, 0.9f, 16.0f);
}

void Terrain::InitModel(ID3D11Device* md3dDevice, int MapSize, int UnitMapOffset, std::vector<UnitSence>& sence, std::vector <Model_Tansform_Info*>& PlayerInfo)
{
	float offsetX = 0.0f;
	float offsetY = 0.0f;
	float offsetZ = 0.0f;
	for (int index = 0; index < sence.size(); ++index)
	{
		offsetX = sence[index].SenceOffset.x;
		offsetY = sence[index].SenceOffset.y;
		offsetZ = sence[index].SenceOffset.z;
		for (int i = 0; i < MapSize; ++i)
		{
			for (int j = 0; j < MapSize; ++j)
			{
				offsetX += i * UnitMapOffset;
				offsetZ += j * UnitMapOffset;
				char temp = sence[index].SenceData[i][j];
				
				//Model 1  (蜘蛛怪)
				if (temp == '1')
				{
					sence[index].ModelInfo[temp].Mat_tansform_Rot_Scal.push_back(XMMatrixRotationX(-MathHelper::Pi / 2) * XMMatrixScaling(0.055f, 0.055f, 0.055f));
					sence[index].ModelInfo[temp].Mat_tansform_Translation.push_back(XMMatrixTranslation(offsetX, 7.5f, offsetZ));
					//动态物体
				}
				//Model 2 (树)
				else if (temp == '2')
				{
					//静态物体
					sence[index].ModelInfo[temp].Mat_World.push_back(XMMatrixRotationY(MathHelper::RandF(0.0f, 3.14f))
						* XMMatrixScaling(0.15f, 0.15f, 0.15f)
						* XMMatrixTranslation(offsetX, 7.5f, offsetZ));
				}
				//Model 3 (白鹤)
				else if (temp == '3')
				{
					//静态物体
					sence[index].ModelInfo[temp].Mat_World.push_back(XMMatrixRotationX(-MathHelper::Pi / 2)
						* XMMatrixScaling(5.2f, 5.2f, 5.2f)
						* XMMatrixTranslation(offsetX, 77.5f, offsetZ));
				}
				//Model 4 (石山)
				else if (temp == '4')
				{
					//静态物体
					sence[index].ModelInfo[temp].Mat_World.push_back(XMMatrixRotationX(MathHelper::Pi / 2)
						* XMMatrixRotationY(MathHelper::RandF(0.0f, 3.14f))
						* XMMatrixScaling(0.007f, 0.007f, 0.007f)
						* XMMatrixTranslation(offsetX, 3.5f, offsetZ));
				}
				//Model 5  (房屋1)
				else if (temp == '5')
				{
					//静态物体
					sence[index].ModelInfo[temp].Mat_World.push_back(XMMatrixRotationX(-MathHelper::Pi / 2)
						* XMMatrixScaling(2.0f, 2.0f, 2.0f)
						* XMMatrixTranslation(offsetX, 7.5f, offsetX));
				}
				//Model 6 (神像)
				else if (temp == '6')
				{
					//静态物体
					sence[index].ModelInfo[temp].Mat_World.push_back(XMMatrixRotationX(-MathHelper::Pi / 2)
						* XMMatrixRotationY(MathHelper::RandF(0.0f, 3.14f))
						* XMMatrixScaling(5.0f, 5.0f, 5.0f)
						* XMMatrixTranslation(offsetX, 7.5f, offsetX));
				}
				//Model 7 (小孩石像)
				else if (temp == '7')
				{
					//静态物体
					sence[index].ModelInfo[temp].Mat_World.push_back(XMMatrixRotationX(-MathHelper::Pi / 2)
						* XMMatrixRotationY(MathHelper::RandF(0.0f, 3.14f))
						* XMMatrixScaling(6.0f, 6.0f, 6.0f)
						* XMMatrixTranslation(offsetX, 7.5f, offsetX));
				}
				//Model 8 PLAYER MODEL
				else if (temp == '8')
				{
					//动态物体
					sence[index].ModelInfo[temp].Mat_tansform_Rot_Scal.push_back(XMMatrixRotationX(-MathHelper::Pi / 2) * XMMatrixScaling(0.1f, 0.1f, 0.1f));
					sence[index].ModelInfo[temp].Mat_tansform_Translation.push_back(XMMatrixTranslation(offsetX, 7.5f, offsetZ));
				}
				//reset offset
				offsetX = sence[index].SenceOffset.x;
				offsetZ = sence[index].SenceOffset.z;
			}
		}
	}

	//PLAYER info
	PlayerInfo.push_back(&sence[0].ModelInfo['8']);
}



void Terrain::Render(ID3D11DeviceContext* md3dImmediateContext, DirectionalLight& DirLight,ShadowMap* pShadowMap,bool isClip)
{
	md3dImmediateContext->IASetInputLayout(InputLayouts::InstancedBasic32);
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride[2] = { sizeof(Vertex::PosNormalTexTan), sizeof(InstancedData) };
	UINT offset[2] = { 0,0 };

	ID3D11Buffer* vbs[2] = { mBoxVB, mInstancedBuffer };

	XMMATRIX view =Camera::Get()->View();
	XMMATRIX proj = Camera::Get()->Proj();
	XMMATRIX viewProj = Camera::Get()->ViewProj();

	// Set per frame constants.
	Effects::InstancedBasicFX->SetDirLights(DirLight);
	Effects::InstancedBasicFX->SetEyePosW(Camera::Get()->GetPosition());
	ID3DX11EffectTechnique* activeTech;
	if (!isClip)
	{
		 activeTech = Effects::InstancedBasicFX->Light1TexTech;
	}
	else
	{
		activeTech = Effects::InstancedBasicFX->Light1TexClipTech;
	}
	D3DX11_TECHNIQUE_DESC techDesc;
	activeTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		// Draw the Terrain.

		md3dImmediateContext->IASetVertexBuffers(0, 2, vbs, stride, offset);
		md3dImmediateContext->IASetIndexBuffer(mBoxIB, DXGI_FORMAT_R32_UINT, 0);

		Effects::InstancedBasicFX->SetShadowTransform(XMLoadFloat4x4(&pShadowMap->GetShadowTransform()));
		Effects::InstancedBasicFX->SetViewProj(viewProj);
		Effects::InstancedBasicFX->SetMaterial(mTerrainMat);
		Effects::InstancedBasicFX->SetDiffuseMap(mDiffuseSRV);
		Effects::InstancedBasicFX->SetNormalMap(mNormalSRV);
		Effects::InstancedBasicFX->SetShadowMap(pShadowMap->DepthMapSRV());
		activeTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexedInstanced(mIndexCount, mVisibleObjectCount, 0, 0, 0);
	}
}


void Terrain::BuildCrateGeometryBuffers(ID3D11Device* md3dDevice)
{
	GeometryGenerator::MeshData box;

	GeometryGenerator geoGen;
	geoGen.CreateBox(1.0f, 1.0f, 1.0f, box);

	//
	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	//

	std::vector<Vertex::PosNormalTexTan> vertices(box.Vertices.size());

	for (UINT i = 0; i < box.Vertices.size(); ++i)
	{
		vertices[i].Pos = box.Vertices[i].Position;
		vertices[i].Normal = box.Vertices[i].Normal;
		vertices[i].Tex = box.Vertices[i].TexC;
		vertices[i].TangentU = box.Vertices[i].TangentU;
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::PosNormalTexTan) * box.Vertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mBoxVB));

	//
	// Pack the indices of all the meshes into one index buffer.
	//

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * box.Indices.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &box.Indices[0];
	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mBoxIB));

	//index count
	mIndexCount = box.Indices.size();
}

void Terrain::LoadTexture(ID3D11Device* md3dDevice)
{
	ID3D11Resource* texResource = nullptr;
	HR(DirectX::CreateDDSTextureFromFile(md3dDevice,
		L"Textures/2.dds", &texResource, &mDiffuseSRV[0]));
	ReleaseCOM(texResource); // view saves reference

	HR(DirectX::CreateDDSTextureFromFile(md3dDevice,
		L"Textures/3.dds", &texResource, &mDiffuseSRV[1]));
	ReleaseCOM(texResource); // view saves reference

	HR(DirectX::CreateDDSTextureFromFile(md3dDevice,
		L"Textures/4.dds", &texResource, &mDiffuseSRV[2]));
	ReleaseCOM(texResource); // view saves reference

	HR(DirectX::CreateDDSTextureFromFile(md3dDevice,
		L"Textures/5.dds", &texResource, &mDiffuseSRV[3]));
	ReleaseCOM(texResource); // view saves reference

	HR(DirectX::CreateDDSTextureFromFile(md3dDevice,
		L"Textures/6.dds", &texResource, &mDiffuseSRV[4]));
	ReleaseCOM(texResource); // view saves reference

	//normal
	HR(DirectX::CreateDDSTextureFromFile(md3dDevice,
		L"Textures/2_nmap.dds", &texResource, &mNormalSRV[0]));
	ReleaseCOM(texResource); // view saves reference

	HR(DirectX::CreateDDSTextureFromFile(md3dDevice,
		L"Textures/3_nmap.dds", &texResource, &mNormalSRV[1]));
	ReleaseCOM(texResource); // view saves reference

	HR(DirectX::CreateDDSTextureFromFile(md3dDevice,
		L"Textures/4_nmap.dds", &texResource, &mNormalSRV[2]));
	ReleaseCOM(texResource); // view saves reference

	HR(DirectX::CreateDDSTextureFromFile(md3dDevice,
		L"Textures/5_nmap.dds", &texResource, &mNormalSRV[3]));
	ReleaseCOM(texResource); // view saves reference

	HR(DirectX::CreateDDSTextureFromFile(md3dDevice,
		L"Textures/6_nmap.dds", &texResource, &mNormalSRV[4]));
	ReleaseCOM(texResource); // view saves reference


}
