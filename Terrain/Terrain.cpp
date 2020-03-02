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

void Terrain::Init(ID3D11Device* md3dDevice,int MapSize,int UnitMapOffset)
{
	XMMATRIX boxScale = XMMatrixScaling(15.0f, 15.0f, 15.0f);
	XMMATRIX boxOffset = XMMatrixTranslation(0.0f, 0.0f, 0.0f);

	//resize 
	mInstance.resize(MapSize* MapSize);

	float offset_border = -7.5f;
	for (int i = 0; i < MapSize; ++i)
	{		
		for (int j = 0; j < MapSize; ++j)
		{
			boxOffset = XMMatrixTranslation((i * UnitMapOffset - 60.0), offset_border, (j * UnitMapOffset - 60.0));
			XMStoreFloat4x4(&mInstance[i*MapSize+j].World, boxScale * boxOffset);
			mInstance[i*MapSize+j].TexIndex = rand() % 4;
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



void Terrain::Render(ID3D11DeviceContext* md3dImmediateContext, DirectionalLight& DirLight)
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
	

	ID3DX11EffectTechnique* activeTech = Effects::InstancedBasicFX->Light1TexTech;

	D3DX11_TECHNIQUE_DESC techDesc;
	activeTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		// Draw the Terrain.

		md3dImmediateContext->IASetVertexBuffers(0, 2, vbs, stride, offset);
		md3dImmediateContext->IASetIndexBuffer(mBoxIB, DXGI_FORMAT_R32_UINT, 0);

		Effects::InstancedBasicFX->SetViewProj(viewProj);
		Effects::InstancedBasicFX->SetMaterial(mTerrainMat);
		Effects::InstancedBasicFX->SetDiffuseMap(mSRV);
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
		L"Textures/stones.dds", &texResource, &mSRV[0]));
	ReleaseCOM(texResource); // view saves reference

	HR(DirectX::CreateDDSTextureFromFile(md3dDevice,
		L"Textures/3.dds", &texResource, &mSRV[1]));
	ReleaseCOM(texResource); // view saves reference

	HR(DirectX::CreateDDSTextureFromFile(md3dDevice,
		L"Textures/4.dds", &texResource, &mSRV[2]));
	ReleaseCOM(texResource); // view saves reference

	HR(DirectX::CreateDDSTextureFromFile(md3dDevice,
		L"Textures/5.dds", &texResource, &mSRV[3]));
	ReleaseCOM(texResource); // view saves reference

	HR(DirectX::CreateDDSTextureFromFile(md3dDevice,
		L"Textures/stones_nmap.dds", &texResource, &mSRV[4]));
	ReleaseCOM(texResource); // view saves reference
}
