#include "app.h"
#include <NsApp/EntryPoint.h>
#include <thread>
#include <DirectXCollision.h>

#include "d3dApp.h"
#include "d3dx11Effect.h"
#include "GeometryGenerator.h"
#include "MathHelper.h"
#include "LightHelper.h"
#include "Effects.h"
#include "Vertex.h"
#include "RenderStates.h"
#include "Waves.h"
#include "Camera.h"
#include "SoD3DLogicFlowHelp.h"
#include "CGameMapElement.h"


//debug
std::string str;

////////////////////////////////////////////////////////////////////////////////////////////////////
//model num
#define Model_Num  8
#define Model1_Instance 5
#define Model2_Instance 25
#define Model3_Instance 200
#define Model4_Instance 20
#define Model5_Instance 1
#define Model6_Instance 4
#define Model7_Instance 1
#define Model8_Instance 1
//map size
#define Map_size  21
#define Unit_MapOffset 16

AppLauncher launcher;
bool menu = true;
extern HWND  hWnd;

HINSTANCE ghInstance;
HINSTANCE ghPrevInstance;
LPSTR    glpCmdLine;
int      gnCmdShow;


//thread function
void MenuGUI();
void MainApp();

std::thread th1(MenuGUI);
std::thread th2(MainApp);

enum Render_Options
{
	Lighting = 0,
	Textures = 1,
	TexturesAndFog = 2
};

//animation state
enum Anim_State
{
	Run = 1,
	Idle = 2,
	Attack = 3
};


class GameApp : public D3DApp
{
public:
	GameApp(HINSTANCE hInstance);
	~GameApp();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

private:
	float GetHillHeight(float x, float z)const;
	XMFLOAT3 GetHillNormal(float x, float z)const;
	void BuildLandGeometryBuffers();
	void BuildWaveGeometryBuffers();
	void BuildCrateGeometryBuffers();

	XMINT2 RayTOModel(float sx, float sy);
	void FindPath();
	void RenderFbxModel();
	void UpdateFbxAnimation(float dt);
	void GetKeyState(float dt);
	//fbx model tansform information initialization
	void InitFbxModel();
	//Create Blend State
	void CreateBlendState();
	//Load Texture
	void LoadTexture();
private:
	ID3D11Buffer* mLandVB;
	ID3D11Buffer* mLandIB;

	ID3D11Buffer* mWavesVB;
	ID3D11Buffer* mWavesIB;

	ID3D11Buffer* mBoxVB;
	ID3D11Buffer* mBoxIB;

	ID3D11ShaderResourceView* mSRV[6];

	Waves mWaves;

	DirectionalLight mDirLights[3];
	Material mLandMat;
	Material mWavesMat;
	Material mBoxMat;

	XMFLOAT4X4 mGrassTexTransform;
	XMFLOAT4X4 mWaterTexTransform;
	XMFLOAT4X4 mLandWorld;
	XMFLOAT4X4 mWavesWorld;
	XMFLOAT4X4 mBoxWorld[Map_size][Map_size];
	int texture_index[Map_size][Map_size];
	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;
	//camera
	Camera* mCamera;

	UINT mLandIndexCount;

	XMFLOAT2 mWaterTexOffset;

	Render_Options mRenderOptions;

	XMFLOAT3 mEyePosW;

	float mTheta;
	float mPhi;
	float mRadius;

	POINT mLastMousePos;

	ID3D11BlendState* m_pBlendState;

	//FBX Models
	std::vector<Model*> m_Models;
	Model_Tansform_Info m_ModeInfo[Model_Num];


	//box bounding
	BoundingBox MapBox;
	//Path end
	XMINT2 Mapindex;
	XMINT2 Endindex;
	XMINT2 PlayerPositionIndex;

	//Game map
	CGameMap m_GameMap;
	VPath m_Path;
	DIRECT m_Dir;
	ROT_DIR ROT;
	bool First;
	bool isMove;
	Anim_State mAnim;
	int unitlen;
	XMFLOAT3 PlayerWorldPosition;
};


GameApp::GameApp(HINSTANCE hInstance)
	: D3DApp(hInstance), mLandVB(0), mLandIB(0), mWavesVB(0), mWavesIB(0), mBoxVB(0), mBoxIB(0),
	mWaterTexOffset(0.0f, 0.0f), mEyePosW(0.0f, 0.0f, 0.0f), mLandIndexCount(0), mRenderOptions(Render_Options::TexturesAndFog),
	mTheta(1.3f * MathHelper::Pi), mPhi(0.4f * MathHelper::Pi), mRadius(80.0f), 
	mCamera(nullptr), PlayerPositionIndex(0,0), Mapindex(0,0), First(true), Endindex(0,0), unitlen(0),isMove(false), PlayerWorldPosition(-65.0f, -5.0f, -60.0f)
{
	mMainWndCaption = L"Game Demo";
	mEnable4xMsaa = true;

	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mLandWorld, I);
	XMStoreFloat4x4(&mWavesWorld, I);
	XMStoreFloat4x4(&mView, I);
	XMStoreFloat4x4(&mProj, I);

	
	//camera init
	if (mCamera == NULL)
	{
		mCamera = Camera::Get();
	}

	

	XMMATRIX boxScale = XMMatrixScaling(15.0f, 15.0f, 15.0f);
	MapBox.Center=XMFLOAT3(0.0f,0.0f,0.0f);
	MapBox.Extents= XMFLOAT3(0.5f, 0.5f, 0.5f);
	XMMATRIX boxOffset = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
	
	float offset_border = -7.5f;
	for (int i = 0; i < Map_size; ++i)
	{
		for (int j = 0; j < Map_size; ++j)
		{
			boxOffset = XMMatrixTranslation((i * Unit_MapOffset - 60.0), offset_border, (j * Unit_MapOffset - 60.0));
			XMStoreFloat4x4(&mBoxWorld[i][j], boxScale * boxOffset);
			texture_index[i][j] = rand() % 5;
		}
	}

	XMMATRIX grassTexScale = XMMatrixScaling(100.0f, 100.0f, 0.0f);
	XMStoreFloat4x4(&mGrassTexTransform, grassTexScale);


	//model init
	m_Models.resize(Model_Num);
	for (int i = 0; i < Model_Num; ++i)
	{
		m_Models[i] = new Model;
	}

	//GUI init
	m_pGameGUI = new GameGUI;



	mDirLights[0].Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[0].Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mDirLights[0].Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mDirLights[0].Direction = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);

	mDirLights[1].Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[1].Diffuse = XMFLOAT4(0.20f, 0.20f, 0.20f, 1.0f);
	mDirLights[1].Specular = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
	mDirLights[1].Direction = XMFLOAT3(-0.57735f, -0.57735f, 0.57735f);

	mDirLights[2].Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[2].Diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[2].Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[2].Direction = XMFLOAT3(0.0f, -0.707f, -0.707f);

	mLandMat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mLandMat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mLandMat.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);

	mWavesMat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mWavesMat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);
	mWavesMat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 32.0f);

	mBoxMat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mBoxMat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mBoxMat.Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 16.0f);
}

GameApp::~GameApp()
{
	md3dImmediateContext->ClearState();
	ReleaseCOM(mLandVB);
	ReleaseCOM(mLandIB);
	ReleaseCOM(mWavesVB);
	ReleaseCOM(mWavesIB);
	ReleaseCOM(mBoxVB);
	ReleaseCOM(mBoxIB);
	for (int i = 0; i < 6; ++i)
	{
		ReleaseCOM(mSRV[i]);
	}

	Effects::DestroyAll();
	InputLayouts::DestroyAll();
	RenderStates::DestroyAll();


	if (mCamera)
	{
		delete mCamera;
		mCamera = nullptr;
	}

	//gui release
	delete m_pGameGUI;
	m_pGameGUI = nullptr;

	//fbx model release
	for (auto& e : m_Models)
	{
		delete e;
		e = nullptr;
	}
}

bool GameApp::Init()
{
	//��ʼ������
	if (!D3DApp::Init())
		return false;

	//ת����ͼ
	m_GameMap.ConvertMap();

	// Must init Effects first since InputLayouts depend on shader signatures.
	Effects::InitAll(md3dDevice);
	InputLayouts::InitAll(md3dDevice);
	RenderStates::InitAll(md3dDevice);

	//load textures
	LoadTexture();

	//Build Geometry Buffers
	BuildLandGeometryBuffers();
	BuildCrateGeometryBuffers();


	//FBX Create
	if (!SoD3DLogicFlowHelp_Create())
		return false;

	//FBX Init and Load
	InitFbxModel();

	//��ʼ�����
	mCamera->SetPosition(XMFLOAT3(PlayerWorldPosition.x - 100, 100, PlayerWorldPosition.z - 100));
	mCamera->LookAt(XMFLOAT3(PlayerWorldPosition.x - 100, 100, PlayerWorldPosition.z - 100), PlayerWorldPosition,XMFLOAT3(0.0f,1.0f,0.0f));

	//Create Blend State
	CreateBlendState();
	
	return true;
}

void GameApp::OnResize()
{
	D3DApp::OnResize();

	mCamera->SetLens(0.25f * MathHelper::Pi, AspectRatio(), 1.0f, 1500.0f);
	
}

void GameApp::UpdateScene(float dt)
{
	
	

	//�ж϶���״̬
	if ((PlayerPositionIndex.x == Mapindex.x) && (PlayerPositionIndex.y == Mapindex.y)&&!isMove)
	{
		mAnim = Anim_State::Idle;
	}
	else
	{
		mAnim = Anim_State::Run;
	}
	if (Mapindex.x < 0)
	{
		mAnim = Anim_State::Idle;
	}

	
	
	//ÿ֡Ѱ·
	FindPath();


	


	//// Convert Spherical to Cartesian coordinates.
	//float x = mRadius * sinf(mPhi) * cosf(mTheta);
	//float z = mRadius * sinf(mPhi) * sinf(mTheta);
	//float y = mRadius * cosf(mPhi);

	mCamera->SetPosition(PlayerWorldPosition.x - 100, 100, PlayerWorldPosition.z - 100);

	mEyePosW = mCamera->GetPosition();

	// Tile water texture.
	XMMATRIX wavesScale = XMMatrixScaling(100.0f, 100.0f, 0.0f);

	// Translate texture over time.
	mWaterTexOffset.y += 0.5f * dt;
	mWaterTexOffset.x += 0.5f * dt;
	XMMATRIX wavesOffset = XMMatrixTranslation(mWaterTexOffset.x, mWaterTexOffset.y, 0.0f);

	// Combine scale and translation.
	XMStoreFloat4x4(&mWaterTexTransform, wavesScale * wavesOffset);

	//��ȡ������Ϣ
	GetKeyState(dt);

	//Update Fbx Animation
	UpdateFbxAnimation(dt);
	
	//gui update
	m_pGameGUI->Update(dt);
}

void GameApp::DrawScene()
{
	//GUI pre render(if need Render Offscreen)
	m_pGameGUI->PreRender();


	//�ڷǲ˵�״̬����Ⱦ��Ϸ����
	if (!menu)
	{
		md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&GColors::Silver));
		md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		md3dImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);
		md3dImmediateContext->OMSetDepthStencilState(0, 0);
		md3dImmediateContext->RSSetState(0);
		md3dImmediateContext->IASetInputLayout(InputLayouts::Basic32);
		md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		md3dImmediateContext->RSSetViewports(1, &mScreenViewport);

		float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		md3dImmediateContext->OMSetBlendState(m_pBlendState, blendFactor, 0xFFFFFFFF);

		UINT stride = sizeof(Vertex::Basic32);
		UINT offset = 0;

		XMMATRIX view = mCamera->View();
		XMMATRIX proj = mCamera->Proj();
		XMMATRIX viewProj = view * proj;

		// Set per frame constants.
		Effects::BasicFX->SetDirLights(mDirLights);
		Effects::BasicFX->SetEyePosW(mEyePosW);
		Effects::BasicFX->SetFogColor(GColors::Silver);
		Effects::BasicFX->SetFogStart(mTimer.TotalTime());
		Effects::BasicFX->SetFogRange(175.0f);

		ID3DX11EffectTechnique* boxTech;
		ID3DX11EffectTechnique* landAndWavesTech;

		switch (mRenderOptions)
		{
		case Render_Options::Lighting:
			boxTech = Effects::BasicFX->Light3Tech;
			landAndWavesTech = Effects::BasicFX->Light3Tech;
			break;
		case Render_Options::Textures:
			boxTech = Effects::BasicFX->Light3TexAlphaClipTech;
			landAndWavesTech = Effects::BasicFX->Light3TexTech;
			break;
		case Render_Options::TexturesAndFog:
			boxTech = Effects::BasicFX->Light3TexAlphaClipFogTech;
			landAndWavesTech = Effects::BasicFX->Light3TexFogTech;
			break;
		}

		D3DX11_TECHNIQUE_DESC techDesc;

		//
		// Draw the LAND with alpha clipping.
		// 

		boxTech->GetDesc(&techDesc);
		for (UINT p = 0; p < techDesc.Passes; ++p)
		{
			md3dImmediateContext->IASetVertexBuffers(0, 1, &mBoxVB, &stride, &offset);
			md3dImmediateContext->IASetIndexBuffer(mBoxIB, DXGI_FORMAT_R32_UINT, 0);

			//Set per object constants.
			for (int i = 0; i < Map_size; ++i)
			{
				for (int j = 0; j < Map_size; ++j)
				{
					XMMATRIX world = XMLoadFloat4x4(&mBoxWorld[i][j]);
					XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
					XMMATRIX worldViewProj = world * view * proj;

					Effects::BasicFX->SetWorld(world);
					Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
					Effects::BasicFX->SetWorldViewProj(worldViewProj);
					Effects::BasicFX->SetTexTransform(XMMatrixIdentity());
					Effects::BasicFX->SetMaterial(mBoxMat);
					Effects::BasicFX->SetDiffuseMap(mSRV[texture_index[i][j]]);
					Effects::BasicFX->SetWave(false);
					md3dImmediateContext->RSSetState(RenderStates::NoCullRS);
					boxTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
					md3dImmediateContext->DrawIndexed(36, 0, 0);
				}
			}
			// Restore default render state.
			//md3dImmediateContext->RSSetState(RenderStates::WireframeRS);
		}

		//
		// Draw the  water with texture and fog (no alpha clipping needed).
		//

		landAndWavesTech->GetDesc(&techDesc);
		for (UINT p = 0; p < techDesc.Passes; ++p)
		{
			//
			// Draw the hills.
			//
			md3dImmediateContext->IASetVertexBuffers(0, 1, &mLandVB, &stride, &offset);
			md3dImmediateContext->IASetIndexBuffer(mLandIB, DXGI_FORMAT_R32_UINT, 0);

			// Set per object constants.
			XMMATRIX world = XMMatrixTranslation(0.0f, -10.0f, 0.0f) * XMLoadFloat4x4(&mLandWorld);
			XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
			XMMATRIX worldViewProj = world * view * proj;
			Effects::BasicFX->SetWave(true);
			Effects::BasicFX->SetWorld(world);
			Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::BasicFX->SetWorldViewProj(worldViewProj);
			Effects::BasicFX->SetTexTransform(XMLoadFloat4x4(&mWaterTexTransform));
			Effects::BasicFX->SetMaterial(mWavesMat);
			Effects::BasicFX->SetDiffuseMap(mSRV[0]);
			Effects::BasicFX->SetFogRange(175.0f);
			landAndWavesTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
			md3dImmediateContext->DrawIndexed(mLandIndexCount, 0, 0);

		}
		//fbx model
		RenderFbxModel();
		//render gui
		m_pGameGUI->Render();
	}

	HR(mSwapChain->Present(0, 0));
}

void GameApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	if (!menu)
	{
		//��¼���ѡ�еĵ�ͼ����(�Ա�Ѱ·�㷨ʹ��)
		Mapindex = RayTOModel((float)mLastMousePos.x, (float)mLastMousePos.y);
		

		std::stringstream ss;
		ss << "X:" << Mapindex.x << "Y:" << Mapindex.y << std::flush;

		ss >> ::str;


	}    
	

	SetCapture(mhMainWnd);
}

void GameApp::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void GameApp::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f * static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(y - mLastMousePos.y));

		// Update angles based on input to orbit camera around box.
		float Theta = dx;
		float	Phi = dy;

		// Restrict the angle mPhi.
		//mPhi = MathHelper::Clamp(mPhi, 0.1f, MathHelper::Pi - 0.1f);
		mCamera->Pitch(Phi);
		mCamera->RotateY(Theta);
	
	}
	else if ((btnState & MK_RBUTTON) != 0)
	{
		// Make each pixel correspond to 0.01 unit in the scene.
		float dx = 0.1f * static_cast<float>(x - mLastMousePos.x);
		float dy = 0.1f * static_cast<float>(y - mLastMousePos.y);

		// Update the camera radius based on input.
		mRadius += dx - dy;

		// Restrict the radius.
		mRadius = MathHelper::Clamp(mRadius, 20.0f, 500.0f);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;


}

float GameApp::GetHillHeight(float x, float z)const
{
	return 0.3f * (z * sinf(0.1f * x) + x * cosf(0.1f * z));
}

XMFLOAT3 GameApp::GetHillNormal(float x, float z)const
{
	// n = (-df/dx, 1, -df/dz)
	XMFLOAT3 n(
		-0.03f * z * cosf(0.1f * x) - 0.3f * cosf(0.1f * z),
		1.0f,
		-0.3f * sinf(0.1f * x) + 0.03f * x * sinf(0.1f * z));

	XMVECTOR unitNormal = XMVector3Normalize(XMLoadFloat3(&n));
	XMStoreFloat3(&n, unitNormal);

	return n;
}

void GameApp::BuildLandGeometryBuffers()
{
	GeometryGenerator::MeshData grid;

	GeometryGenerator geoGen;

	geoGen.CreateGrid(2000.0f, 2000.0f, 300, 300, grid);

	mLandIndexCount = grid.Indices.size();

	//
	// Extract the vertex elements we are interested and apply the height function to
	// each vertex.  
	//

	std::vector<Vertex::Basic32> vertices(grid.Vertices.size());
	for (UINT i = 0; i < grid.Vertices.size(); ++i)
	{
		XMFLOAT3 p = grid.Vertices[i].Position;

		//p.y = GetHillHeight(p.x, p.z);

		vertices[i].Pos = p;
		vertices[i].Normal = GetHillNormal(p.x, p.z);
		vertices[i].Tex = grid.Vertices[i].TexC;
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::Basic32) * grid.Vertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mLandVB));

	//
	// Pack the indices of all the meshes into one index buffer.
	//

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * mLandIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &grid.Indices[0];
	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mLandIB));
}

void GameApp::BuildWaveGeometryBuffers()
{
	// Create the vertex buffer.  Note that we allocate space only, as
	// we will be updating the data every time step of the simulation.

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(Vertex::Basic32) * mWaves.VertexCount();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;
	HR(md3dDevice->CreateBuffer(&vbd, 0, &mWavesVB));


	// Create the index buffer.  The index buffer is fixed, so we only 
	// need to create and set once.

	std::vector<UINT> indices(3 * mWaves.TriangleCount()); // 3 indices per face

	// Iterate over each quad.
	UINT m = mWaves.RowCount();
	UINT n = mWaves.ColumnCount();
	int k = 0;
	for (UINT i = 0; i < m - 1; ++i)
	{
		for (DWORD j = 0; j < n - 1; ++j)
		{
			indices[k] = i * n + j;
			indices[k + 1] = i * n + j + 1;
			indices[k + 2] = (i + 1) * n + j;

			indices[k + 3] = (i + 1) * n + j;
			indices[k + 4] = i * n + j + 1;
			indices[k + 5] = (i + 1) * n + j + 1;

			k += 6; // next quad
		}
	}

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * indices.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mWavesIB));
}

void GameApp::BuildCrateGeometryBuffers()
{
	GeometryGenerator::MeshData box;

	GeometryGenerator geoGen;
	geoGen.CreateBox(1.0f, 1.0f, 1.0f, box);

	//
	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	//

	std::vector<Vertex::Basic32> vertices(box.Vertices.size());

	for (UINT i = 0; i < box.Vertices.size(); ++i)
	{
		vertices[i].Pos = box.Vertices[i].Position;
		vertices[i].Normal = box.Vertices[i].Normal;
		vertices[i].Tex = box.Vertices[i].TexC;
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::Basic32) * box.Vertices.size();
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
}

XMINT2 GameApp::RayTOModel(float sx,float sy)
{
	XMFLOAT4X4 P;
	XMStoreFloat4x4(&P, Camera::Get()->Proj());

	// Compute picking ray in view space.
	float vx = (+2.0f * sx / mClientWidth - 1.0f) / P(0, 0);
	float vy = (-2.0f * sy / mClientHeight + 1.0f) / P(1, 1);

	// Ray definition in view space.
	XMVECTOR rayOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR rayDir = XMVectorSet(vx, vy, 1.0f, 0.0f);

	XMMATRIX V = Camera::Get()->View();
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(V), V);

	float lmin = MathHelper::Infinity;
	int Map_x = -1;
	int Map_y = -1;
	//�ڵ�λ��ͼ��Χ�ڼ���������ͼ����ײ
	for (int i = 0; i < Map_size; ++i)
	{
		for (int j = 0; j < Map_size; ++j)
		{
			// Ray definition in view space.
			 rayOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
			 rayDir = XMVectorSet(vx, vy, 1.0f, 0.0f);


			XMMATRIX W = XMLoadFloat4x4(&mBoxWorld[i][j]);
			XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(W), W);
			
			// Tranform ray to vi space of Mesh.
			XMMATRIX toLocal = XMMatrixMultiply(invView, invWorld);

			rayOrigin = XMVector3TransformCoord(rayOrigin, toLocal);
			rayDir = XMVector3TransformNormal(rayDir, toLocal);

			// Make the ray direction unit length for the intersection tests.
			rayDir = XMVector3Normalize(rayDir);

			float tmin = 0.0f;
			if (MapBox.Intersects(rayOrigin, rayDir, tmin))
			{
				if (tmin<lmin)
				{
					Map_x = i;
					Map_y = j;
					lmin = tmin;
				}
			}
		}
	}

	//�����ཻ��ͼ�����;
	return XMINT2(Map_x, Map_y);
}

void GameApp::FindPath()
{
	if (!menu)
	{
		//����Ѱ·
		if (unitlen == 0 || unitlen == Unit_MapOffset)
		{
			if (Mapindex.x >= 0)
			{
				Endindex = Mapindex;
				First = true;
			}
		}


		static float x = -65.0f;
		static float z = -60.0f;
		static bool findpath = false;
		if (Mapindex.x >= 0)
		{
			if (First || unitlen >= Unit_MapOffset)
			{
				findpath = m_GameMap.FindNextDirection(&m_Dir, PlayerPositionIndex.x, PlayerPositionIndex.y, Endindex.x, Endindex.y, ROT);
				First = false;
				unitlen = 0;

				if (findpath)
				{
					switch (m_Dir)
					{
					case MOVE_DOWN:
						PlayerPositionIndex.x += 1.0f;
						break;
					case MOVE_LEFT:
						PlayerPositionIndex.y -= 1.0f;
						break;
					case MOVE_RIGHT:
						PlayerPositionIndex.y += 1.0f;
						break;
					case MOVE_UP:
						PlayerPositionIndex.x -= 1.0f;
						break;
					default:
						break;
					}


					if (ROT == TurnRight)
					{
						m_ModeInfo[7].Mat_tansform_Rot_Scal[0] *= XMMatrixRotationY(MathHelper::Pi / 2);

					}
					else if (ROT == TurnLeft)
					{
						m_ModeInfo[7].Mat_tansform_Rot_Scal[0] *= XMMatrixRotationY(-MathHelper::Pi / 2);
					}
					else if (ROT == BackWard)
					{
						m_ModeInfo[7].Mat_tansform_Rot_Scal[0] *= XMMatrixRotationY(MathHelper::Pi);
					}
				}
				m_Models[7]->SetModelTansInfo(&m_ModeInfo[7]);
				
			}
			if (findpath)
			{
				switch (m_Dir)
				{
				case MOVE_DOWN:
					x += 1.0f;
					break;
				case MOVE_LEFT:
					z -= 1.0f;
					break;
				case MOVE_RIGHT:
					z += 1.0f;
					break;
				case MOVE_UP:
					x -= 1.0f;
					break;
				default:
					break;
				}
				unitlen++;

				if ((PlayerPositionIndex.x == Mapindex.x) && (PlayerPositionIndex.y == Mapindex.y) && unitlen == Unit_MapOffset)
				{
					isMove = false;
				}
				else
				{
					isMove = true;
				}
				m_ModeInfo[7].Mat_tansform_Translation[0] = XMMatrixTranslation(x, -5.0f, z);
				//�������λ����Ϣ
				PlayerWorldPosition = XMFLOAT3(x, -5.0f, z);
				m_Models[7]->SetModelTansInfo(&m_ModeInfo[7]);
			}
		}
	}

}

void GameApp::RenderFbxModel()
{
	//render fbx model
	if (m_Models[0])
	{
		for (int i = 0; i < Model1_Instance; ++i)
		{
			if (!m_Models[0]->BeginRender(i,Anim_State::Idle))
				return;
		}
	}

	if (m_Models[2])
	{
		for (int i = 0; i < Model3_Instance; ++i)
		{
			if (!m_Models[2]->BeginRender(i))
				return;
		}
	}

	if (m_Models[3])
	{
		for (int i = 0; i < Model4_Instance; ++i)
		{
			if (!m_Models[3]->BeginRender(i))
				return;
		}
	}

	if (m_Models[4])
	{
		for (int i = 0; i < Model5_Instance; ++i)
		{
			if (!m_Models[4]->BeginRender(i))
				return;
		}
	}

	if (m_Models[5])
	{
		for (int i = 0; i < Model6_Instance; ++i)
		{
			if (!m_Models[5]->BeginRender(i))
				return;
		}
	}

	if (m_Models[6])
	{
		for (int i = 0; i < Model7_Instance; ++i)
		{
			if (!m_Models[6]->BeginRender(i))
				return;
		}
	}

	if (m_Models[1])
	{
		for (int i = 0; i < Model2_Instance; ++i)
		{
			if (!m_Models[1]->BeginRender(i))
				return;
		}
	}

	if (m_Models[7])
	{
		for (int i = 0; i < Model8_Instance; ++i)
		{
			if (mAnim==Anim_State::Idle)
			{
				if (!m_Models[7]->BeginRender(i, Anim_State::Idle))
					return;
			}
			else if (mAnim == Anim_State::Run)
			{
				if (!m_Models[7]->BeginRender(i, Anim_State::Run))
					return;
			}
		}
	}

	//reset input layout
	md3dImmediateContext->IASetInputLayout(InputLayouts::Basic32);





}

void GameApp::UpdateFbxAnimation(float dt)
{
	//fbx update
	if (m_Models[0])
		m_Models[0]->Update(dt,Anim_State::Idle);
	//player model update
	if (m_Models[7])
	{
		
		if (mAnim== Anim_State::Idle)
		{
			m_Models[7]->Update(dt, Anim_State::Idle);
		}
		else if(mAnim == Anim_State::Run)
		{
			m_Models[7]->Update(dt * 1.2f, Anim_State::Run);
		}
	}
}

void GameApp::GetKeyState(float dt)
{
	// Switch the render mode based in key input.
	/*if (GetAsyncKeyState('W') & 0x8000)
		mCamera->Walk(2.0f);

	if (GetAsyncKeyState('S') & 0x8000)
		mCamera->Walk(-2.0f);

	if (GetAsyncKeyState('A') & 0x8000)
		mCamera->Strafe(-2.0f);

	if (GetAsyncKeyState('D') & 0x8000)
		mCamera->Strafe(2.0f);*/

	//���������ͼ����
	mCamera->UpdateViewMatrix();
}

void GameApp::InitFbxModel()
{
	///fbx model tansform information initialization(��ģ���±�ͳһ)
	{
		//Model 1
		m_ModeInfo[0].Model_Instance_Num = Model1_Instance;
		m_ModeInfo[0].Mat_tansform_Rot_Scal.resize(Model1_Instance);
		m_ModeInfo[0].Mat_tansform_Translation.resize(Model1_Instance);
		for (int i = 0; i < Model1_Instance; ++i)
		{
			int location_offset_x = (int)MathHelper::RandF(-2.0f, 19.0f) * Unit_MapOffset;
			int location_offset_z = (int)MathHelper::RandF(-2.0f, 19.0f) * Unit_MapOffset;
			m_ModeInfo[0].Mat_tansform_Rot_Scal[i] = XMMatrixRotationX(-MathHelper::Pi / 2) * XMMatrixScaling(3.2f, 3.2f, 3.2f);
			m_ModeInfo[0].Mat_tansform_Translation[i] = XMMatrixTranslation(-1.5f + location_offset_x, -4.0f, 3.0f + location_offset_z);
		}

		//Model 2
		m_ModeInfo[1].Model_Instance_Num = Model2_Instance;
		m_ModeInfo[1].Mat_tansform_Rot_Scal.resize(Model2_Instance);
		m_ModeInfo[1].Mat_tansform_Translation.resize(Model2_Instance);
		for (int i = 0; i < Model2_Instance; ++i)
		{
			int location_offset_x = (int)MathHelper::RandF(-5.0f, 16.0f) * Unit_MapOffset;
			int location_offset_z = (int)MathHelper::RandF(-5.0f, 16.0f) * Unit_MapOffset;
			m_ModeInfo[1].Mat_tansform_Rot_Scal[i] = XMMatrixRotationX(-MathHelper::Pi / 2) * XMMatrixRotationY(location_offset_x % 3) * XMMatrixScaling(6.0f, 6.0f, 6.0f);
			m_ModeInfo[1].Mat_tansform_Translation[i] = XMMatrixTranslation(-2.0f + location_offset_x, -4.0f, 5.0f + location_offset_z);
		}


		//Model 3
		m_ModeInfo[2].Model_Instance_Num = Model3_Instance;
		m_ModeInfo[2].Mat_tansform_Rot_Scal.resize(Model3_Instance);
		m_ModeInfo[2].Mat_tansform_Translation.resize(Model3_Instance);
		for (int i = 0; i < Model3_Instance; ++i)
		{
			int location_offset_x = (int)MathHelper::RandF(-1.0f, 64.0f) * 4;
			int location_offset_z = (int)MathHelper::RandF(-1.0f, 64.0f) * 4;
			m_ModeInfo[2].Mat_tansform_Rot_Scal[i] = XMMatrixRotationY(location_offset_x % 3) * XMMatrixScaling(.8f, .8f, .8f);
			m_ModeInfo[2].Mat_tansform_Translation[i] = XMMatrixTranslation(-2.0f + location_offset_x, -4.0f, 5.0f + location_offset_z);
		}


		//Model 4
		m_ModeInfo[3].Model_Instance_Num = Model4_Instance;
		m_ModeInfo[3].Mat_tansform_Rot_Scal.resize(Model4_Instance);
		m_ModeInfo[3].Mat_tansform_Translation.resize(Model4_Instance);
		for (int i = 0; i < Model4_Instance; ++i)
		{
			int location_offset_x = (int)MathHelper::RandF(-5.0f, 16.0f) * Unit_MapOffset;
			int location_offset_z = (int)MathHelper::RandF(-5.0f, 16.0f) * Unit_MapOffset;
			m_ModeInfo[3].Mat_tansform_Rot_Scal[i] = XMMatrixRotationX(MathHelper::Pi / 2) * XMMatrixRotationY(location_offset_x % 3) * XMMatrixScaling(4, 4, 4);
			m_ModeInfo[3].Mat_tansform_Translation[i] = XMMatrixTranslation(0.0f + location_offset_x, -3.0f, 0.0f + location_offset_z);
		}


		//Model 5
		m_ModeInfo[4].Model_Instance_Num = Model5_Instance;
		m_ModeInfo[4].Mat_tansform_Rot_Scal.resize(Model5_Instance);
		m_ModeInfo[4].Mat_tansform_Translation.resize(Model5_Instance);
		for (int i = 0; i < Model5_Instance; ++i)
		{
			int location_offset_x = (int)MathHelper::RandF(-50.0f, 50.0f) * 4;
			int location_offset_z = (int)MathHelper::RandF(-50.0f, 50.0f) * 4;
			m_ModeInfo[4].Mat_tansform_Rot_Scal[i] = XMMatrixRotationX(-MathHelper::Pi / 2) * XMMatrixScaling(5, 5, 5);
			m_ModeInfo[4].Mat_tansform_Translation[i] = XMMatrixTranslation(20.0f + location_offset_x, -4.0f, 100.0f + location_offset_z);
		}

		//Model 6
		m_ModeInfo[5].Model_Instance_Num = Model6_Instance;
		m_ModeInfo[5].Mat_tansform_Rot_Scal.resize(Model6_Instance);
		m_ModeInfo[5].Mat_tansform_Translation.resize(Model6_Instance);
		for (int i = 0; i < Model6_Instance; ++i)
		{
			int location_offset_x = (int)MathHelper::RandF(-3.0f, 16.0f) * Unit_MapOffset;
			int location_offset_z = (int)MathHelper::RandF(-3.0f, 16.0f) * Unit_MapOffset;
			m_ModeInfo[5].Mat_tansform_Rot_Scal[i] = XMMatrixRotationX(-MathHelper::Pi / 2) * XMMatrixRotationY(location_offset_x % 3) * XMMatrixScaling(4, 4, 4);
			m_ModeInfo[5].Mat_tansform_Translation[i] = XMMatrixTranslation(location_offset_x, -4.0f, location_offset_z + 5.0f);
		}

		//Model 7
		m_ModeInfo[6].Model_Instance_Num = Model7_Instance;
		m_ModeInfo[6].Mat_tansform_Rot_Scal.resize(Model7_Instance);
		m_ModeInfo[6].Mat_tansform_Translation.resize(Model7_Instance);
		for (int i = 0; i < Model7_Instance; ++i)
		{
			int location_offset_x = (int)MathHelper::RandF(-3.0f, 16.0f) * Unit_MapOffset;
			int location_offset_z = (int)MathHelper::RandF(-3.0f, 16.0f) * Unit_MapOffset;
			m_ModeInfo[6].Mat_tansform_Rot_Scal[i] = XMMatrixRotationX(-MathHelper::Pi / 2) * XMMatrixRotationY(location_offset_x % 3) * XMMatrixScaling(2, 2, 2);
			m_ModeInfo[6].Mat_tansform_Translation[i] = XMMatrixTranslation(location_offset_x, -4.0f, location_offset_z + 5.0f);
		}

		//Model 8
		m_ModeInfo[7].Model_Instance_Num = Model8_Instance;
		m_ModeInfo[7].Mat_tansform_Rot_Scal.resize(Model8_Instance);
		m_ModeInfo[7].Mat_tansform_Translation.resize(Model8_Instance);
		for (int i = 0; i < Model8_Instance; ++i)
		{
			m_ModeInfo[7].Mat_tansform_Rot_Scal[i] = XMMatrixRotationX(-MathHelper::Pi / 2) * XMMatrixRotationY(0) * XMMatrixScaling(1, 1, 1);
			m_ModeInfo[7].Mat_tansform_Translation[i] = XMMatrixTranslation(-65.0f, -5.0f, -60.0f);
		}

	}

	if (m_Models[0])
	{
		m_Models[0]->CreateFileFbx("model/GiantSpider.FBX");
		m_Models[0]->CreateFileKkb("model/GiantSpider.kkb");
		m_Models[0]->CreateFileKkf("model/GiantSpider@Idle.kkf",Anim_State::Idle);
		m_Models[0]->CreateImage(L"model/GiantSpider_04.dds");
		m_Models[0]->SetModelTansInfo(&m_ModeInfo[0]);
	}
	if (m_Models[1])
	{
		m_Models[1]->CreateFileFbx("model/Blue_Tree_02a.FBX");
		m_Models[1]->CreateFileKkb("model/Blue_Tree_02a.kkb");
		m_Models[1]->CreateImage(L"model/Blue_Tree2.dds");
		m_Models[1]->SetModelTansInfo(&m_ModeInfo[1]);
	}

	if (m_Models[2])
	{
		m_Models[2]->CreateFileFbx("model/modelFlower_blue.FBX");
		m_Models[2]->CreateFileKkb("model/modelFlower_blue.kkb");
		m_Models[2]->CreateImage(L"model/Tx_flower_blue.dds");
		m_Models[2]->SetModelTansInfo(&m_ModeInfo[2]);
	}
	if (m_Models[3])
	{
		m_Models[3]->CreateFileFbx("model/shan06.FBX");
		m_Models[3]->CreateFileKkb("model/shan06.kkb");
		m_Models[3]->CreateImage(L"model/zzTex3.dds");
		m_Models[3]->SetModelTansInfo(&m_ModeInfo[3]);
	}

	if (m_Models[4])
	{
		m_Models[4]->CreateFileFbx("model/a.FBX");
		m_Models[4]->CreateFileKkb("model/a.kkb");
		m_Models[4]->CreateImage(L"model/UB25201.dds");
		m_Models[4]->SetModelTansInfo(&m_ModeInfo[4]);
	}

	if (m_Models[5])
	{
		m_Models[5]->CreateFileFbx("model/aa.FBX");
		m_Models[5]->CreateFileKkb("model/aa.kkb");
		m_Models[5]->CreateImage(L"model/UC42002.dds");
		m_Models[5]->SetModelTansInfo(&m_ModeInfo[5]);
	}

	if (m_Models[6])
	{
		m_Models[6]->CreateFileFbx("model/child.FBX");
		m_Models[6]->CreateFileKkb("model/child.kkb");
		m_Models[6]->CreateImage(L"model/UC40701.dds");
		m_Models[6]->SetModelTansInfo(&m_ModeInfo[6]);
	}
	//player model
	if (m_Models[7])
	{
		m_Models[7]->CreateFileFbx("model/TraumaGuard_Run.FBX");
		m_Models[7]->CreateFileKkb("model/TraumaGuard_Run.kkb");
		m_Models[7]->CreateFileKkf("model/TraumaGuard_Run.kkf", Anim_State::Run);
		m_Models[7]->CreateFileKkf("model/TraumaGuard_ActiveIdleLoop.kkf", Anim_State::Idle);
		m_Models[7]->CreateImage(L"model/TraumaGuard_Albedo.dds");
		m_Models[7]->SetModelTansInfo(&m_ModeInfo[7]);
	}



}

void GameApp::CreateBlendState()
{
	//Create Blend State
		ID3D11Device* pD3DDevice = D3DApp::Get()->GetD3DDevice();
		HRESULT hr = S_OK;

		D3D11_BLEND_DESC kDesc;
		kDesc.AlphaToCoverageEnable = FALSE;
		kDesc.IndependentBlendEnable = FALSE;
		kDesc.RenderTarget[0].BlendEnable = TRUE;
		kDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		kDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		kDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		kDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		kDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		kDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		kDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		hr = pD3DDevice->CreateBlendState(&kDesc, &m_pBlendState);
}

void GameApp::LoadTexture()
{
	ID3D11Resource* texResource = nullptr;
	HR(DirectX::CreateDDSTextureFromFile(md3dDevice,
		L"Textures/water3.dds", &texResource, &mSRV[0]));
	ReleaseCOM(texResource); // view saves reference

	HR(DirectX::CreateDDSTextureFromFile(md3dDevice,
		L"Textures/2.dds", &texResource, &mSRV[1]));
	ReleaseCOM(texResource); // view saves reference

	HR(DirectX::CreateDDSTextureFromFile(md3dDevice,
		L"Textures/3.dds", &texResource, &mSRV[2]));
	ReleaseCOM(texResource); // view saves reference

	HR(DirectX::CreateDDSTextureFromFile(md3dDevice,
		L"Textures/4.dds", &texResource, &mSRV[3]));
	ReleaseCOM(texResource); // view saves reference

	HR(DirectX::CreateDDSTextureFromFile(md3dDevice,
		L"Textures/5.dds", &texResource, &mSRV[4]));
	ReleaseCOM(texResource); // view saves reference

	HR(DirectX::CreateDDSTextureFromFile(md3dDevice,
		L"Textures/6.dds", &texResource, &mSRV[5]));
	ReleaseCOM(texResource); // view saves reference
}



//////////main
void MenuGUI() 
{

	launcher.Run();
}

 void MainApp() {

	 GameApp theApp(ghInstance);

	 if (!theApp.Init())
		 return ;


	 theApp.Run();
 }




////////////////////////////////////////////////////////////////////////////////////////////////////
 int NsMain(_In_ HINSTANCE hInstance,
     _In_opt_ HINSTANCE hPrevInstance,
     _In_ LPSTR    lpCmdLine,
     _In_ int       nCmdShow)
 {

     ghInstance = hInstance;
     ghPrevInstance = hPrevInstance;
     glpCmdLine = lpCmdLine;
     gnCmdShow = nCmdShow;


     launcher.SetArguments(__argc, __argv);
     launcher.SetApplicationFile("App.xaml");

	 //��UI����Ϸ���߼���Ϊ�����߳�
     th1.join();
     th2.join();
     return 0;
 }
