#include "Grass.h"
#include "Camera.h"
#include "Effects.h"
#include "Vertex.h"
#include "LightHelper.h"


Grass::Grass()
	:mGrassVB(nullptr),
	mGrassTexSRV(nullptr),
	mNumVertRows(0),
	mNumVertCols(0),
	mGrassWidth(0),
	mGrassDepth(0)
{

}

Grass::~Grass()
{
	ReleaseCOM(mGrassVB);
	ReleaseCOM(mGrassTexSRV);
}



void Grass::BuildGrassVB(ID3D11Device* device)
{
	std::vector<XMFLOAT3> Vertices(mNumVertRows * mNumVertCols);

	float halfWidth = 0.5f * mGrassWidth;
	float halfDepth = 0.5f * mGrassDepth;

	float PerGrassWidth = mGrassWidth / (mNumVertCols - 1);
	float PerGrassDepth = mGrassDepth / (mNumVertRows - 1);

	for (UINT i = 0; i < mNumVertRows; ++i)
	{
		float z = halfDepth - i * PerGrassWidth;
		for (UINT j = 0; j < mNumVertCols; ++j)
		{
			float x = -halfWidth + j * PerGrassDepth;
			Vertices[i * mNumVertCols + j]= XMFLOAT3(x, 0.0f, z);
		}
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(XMFLOAT3) * Vertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &Vertices[0];
	HR(device->CreateBuffer(&vbd, &vinitData, &mGrassVB));
}

void Grass::Init(ID3D11Device* device,float GrassWidth, float GrassDepth, int VertRows, int VertCols, ID3D11ShaderResourceView* RandomTexSRV, ID3D11ShaderResourceView* HeightMapSRV)
{
	mNumVertRows = VertRows;
	mNumVertCols = VertCols;
	mGrassWidth = GrassWidth;
	mGrassDepth = GrassDepth;
	mRandomSRV = RandomTexSRV;
	mHeightMapSRV = HeightMapSRV;
	BuildGrassVB(device);


	ID3D11Resource* texResource = nullptr;
	HR(DirectX::CreateDDSTextureFromFile(device,
		L"Textures/grassBlade.dds", &texResource, &mGrassTexSRV));

	HR(DirectX::CreateDDSTextureFromFile(device,
		L"Textures/grassBladeAlpha.dds", &texResource, &mGrassBlendTexSRV));
	ReleaseCOM(texResource); // view saves reference
}

void Grass::update(float dt)
{
	mGameTime = dt;
}

void Grass::Draw(ID3D11DeviceContext* dc, const Camera& cam, DirectionalLight* lights)
{
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	dc->IASetInputLayout(InputLayouts::Pos);

	UINT stride = sizeof(XMFLOAT3);
	UINT offset = 0;
	dc->IASetVertexBuffers(0, 1, &mGrassVB, &stride, &offset);

	XMMATRIX viewProj = cam.ViewProj();

	// Set per frame constants.
	Effects::GrassFX->SetViewProj(viewProj);
	Effects::GrassFX->SetEyePosW(cam.GetPosition());
	Effects::GrassFX->SetGameTime(mGameTime);
	Effects::GrassFX->SetGrassTex(mGrassTexSRV);
	Effects::GrassFX->SetGrassBlendTex(mGrassBlendTexSRV);
	Effects::GrassFX->SetRandomTex(mRandomSRV);
	Effects::GrassFX->SetHeightMapTex(mHeightMapSRV);
	D3DX11_TECHNIQUE_DESC techDesc;
	Effects::GrassFX->DrawTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		Effects::GrassFX->DrawTech->GetPassByIndex(p)->Apply(0, dc);
		dc->Draw(mNumVertRows * mNumVertCols,0);
	}
}
