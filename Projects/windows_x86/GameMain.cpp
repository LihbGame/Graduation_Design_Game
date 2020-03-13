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
#include "Sky.h"
#include "Terrain.h"
#include "ShadowMap.h"
#include "ParticleSystem.h"
//debug
std::string str;

////////////////////////////////////////////////////////////////////////////////////////////////////
//model num
#define Model_Num  8
#define Model1_Instance 5
#define Model2_Instance 25
#define Model3_Instance 1
#define Model4_Instance 20
#define Model5_Instance 1
#define Model6_Instance 4
#define Model7_Instance 1
#define Model8_Instance 1
//map size
#define Map_size  21
#define Unit_MapOffset 16


//player init position
#define PlayerPositionX -60.0f 
#define PlayerPositionY -60.0f

//shadow map size
#define SMapSize 2048

AppLauncher launcher;
bool menu = true;
extern HWND  hWnd;
extern HWND hWnd32;
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

	//build  shadow instance data
	//static
	void BuildStaticInstanceData();
	//dynamic
	void BuildDynamicInstanceData();

	//draw shadow map
	void BuildShadowMap();

	//Init Particle System
	void InitParticleSystem();
	//Draw Particle
	void DrawParticle();

private:
	ID3D11Buffer* mLandVB;
	ID3D11Buffer* mLandIB;

	ID3D11Buffer* mWavesVB;
	ID3D11Buffer* mWavesIB;

	//instance buffer data
	ID3D11Buffer* mFBXInstanceBuffer[Model_Num];
	//water texture
	ID3D11ShaderResourceView* mSRV;

	Waves mWaves;

	DirectionalLight mDirLights[3];
	Material mLandMat;
	Material mWavesMat;
	Material mBoxMat;

	XMFLOAT4X4 mGrassTexTransform;
	XMFLOAT4X4 mWaterTexTransform;
	XMFLOAT4X4 mLandWorld;
	XMFLOAT4X4 mWavesWorld;
	
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

	//全局直线光(目前只有一个) 在游戏初始化函数中初始化
	DirectionalLight gDirLights;


	//FBX Models
	std::vector<Model*> m_Models;
	Model_Tansform_Info m_ModeInfo[Model_Num];
	int LoadProgress;

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
	//sky box
	Sky* mSkyBox;
	//Terrain
	Terrain mTerrain;
	//ShadowMap
	ShadowMap* mShadowMap;
	//Particle System
	ID3D11ShaderResourceView* mFireTexSRV;
	ID3D11ShaderResourceView* mRainTexSRV;
	ID3D11ShaderResourceView* mRandomTexSRV;

	ParticleSystem mFire;
	ParticleSystem mRain;
};


GameApp::GameApp(HINSTANCE hInstance)
	: D3DApp(hInstance), LoadProgress(0),mLandVB(0), mLandIB(0), mWavesVB(0), mWavesIB(0),
	mWaterTexOffset(0.0f, 0.0f), mEyePosW(0.0f, 0.0f, 0.0f), mLandIndexCount(0), mRenderOptions(Render_Options::TexturesAndFog),
	mTheta(1.3f * MathHelper::Pi), mPhi(0.4f * MathHelper::Pi), mRadius(80.0f), 
	mCamera(nullptr), PlayerPositionIndex(0,0), Mapindex(0,0), First(true), Endindex(0,0), unitlen(0),isMove(false), PlayerWorldPosition(PlayerPositionX, 0.0f, PlayerPositionY)
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

	

	
	MapBox.Center=XMFLOAT3(0.0f,0.0f,0.0f);
	MapBox.Extents= XMFLOAT3(0.5f, 0.5f, 0.5f);
	

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

	//Direction light init
	gDirLights.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	gDirLights.Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	gDirLights.Specular = XMFLOAT4(0.2f, 0.2f, 0.5f, 1.0f);
	gDirLights.Direction = XMFLOAT3(0.57735f, -0.57735f, -0.57735f);


	//mat init
	mLandMat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mLandMat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mLandMat.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);

	mWavesMat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mWavesMat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.1f);
	mWavesMat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 32.0f);

	
}

GameApp::~GameApp()
{
	md3dImmediateContext->ClearState();
	ReleaseCOM(mLandVB);
	ReleaseCOM(mLandIB);
	ReleaseCOM(mWavesVB);
	ReleaseCOM(mWavesIB);
	ReleaseCOM(mRandomTexSRV);
	ReleaseCOM(mFireTexSRV);
	ReleaseCOM(mRainTexSRV);
	ReleaseCOM(mSRV);


	Effects::DestroyAll();
	InputLayouts::DestroyAll();
	RenderStates::DestroyAll();


	//camera release
	SafeDelete(mCamera);


	//gui release
	SafeDelete(m_pGameGUI);


	//fbx model release
	for (auto& e : m_Models)
	{
	 SafeDelete(e);
	}

	//sky box release
	SafeDelete(mSkyBox);
	//shadow map release
	SafeDelete(mShadowMap);
}

bool GameApp::Init()
{
	//初始化父类
	if (!D3DApp::Init())
		return false;

	//Terrain Init
	mTerrain.Init(md3dDevice,Map_size,Unit_MapOffset);

	//转换地图
	m_GameMap.ConvertMap();

	// Must init Effects first since InputLayouts depend on shader signatures.
	Effects::InitAll(md3dDevice);
	InputLayouts::InitAll(md3dDevice);
	RenderStates::InitAll(md3dDevice);

	//sky box init
	mSkyBox = new Sky(md3dDevice,L"Textures/space3.dds",1000.0f);
	//shadow map init
	mShadowMap = new ShadowMap(md3dDevice, SMapSize, SMapSize);
	if(mShadowMap)
	{
		mShadowMap->BuildShadowTransform(gDirLights);
	}

	//load textures
	LoadTexture();
	//Build Geometry Buffers
	BuildLandGeometryBuffers();
	//FBX Create
	if (!SoD3DLogicFlowHelp_Create())
		return false;
	//FBX Init and Load
	InitFbxModel();
	//std::thread th3(&GameApp::InitFbxModel, this);
	//th3.join();
	
	//初始化像机
	mCamera->SetPosition(XMFLOAT3(PlayerWorldPosition.x + 60, 100, PlayerWorldPosition.z - 60));
	mCamera->LookAt(XMFLOAT3(PlayerWorldPosition.x + 60, 100, PlayerWorldPosition.z -60), PlayerWorldPosition,XMFLOAT3(0.0f,1.0f,0.0f));
	//Create Blend State
	CreateBlendState();
	
	//Init Particle System
	InitParticleSystem();

	return true;
}

void GameApp::OnResize()
{
	D3DApp::OnResize();

	mCamera->SetLens(0.25f * MathHelper::Pi, AspectRatio(), 1.0f, 1500.0f);
	
}

void GameApp::UpdateScene(float dt)
{
	//判断动画状态
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

	
	//每帧寻路
	FindPath();

	//更新动态模型实例化buffer
	BuildDynamicInstanceData();

	//mCamera->SetPosition(PlayerWorldPosition.x + 60, 100, PlayerWorldPosition.z - 60);

	mEyePosW = mCamera->GetPosition();

	// Tile water texture.
	XMMATRIX wavesScale = XMMatrixScaling(10.0f, 10.0f, 0.0f);

	// Translate texture over time.
	mWaterTexOffset.y += 0.05f * dt;
	mWaterTexOffset.x += 0.05f * dt;
	XMMATRIX wavesOffset = XMMatrixTranslation(mWaterTexOffset.x, mWaterTexOffset.y, 0.0f);

	// Combine scale and translation.
	XMStoreFloat4x4(&mWaterTexTransform, wavesScale * wavesOffset);

	//获取按键消息
	GetKeyState(dt);

	//Update Fbx Animation
	UpdateFbxAnimation(dt);
	
	//gui update
	m_pGameGUI->Update(dt);
	//Fire Particle Update
	mFire.Update(dt, mTimer.TotalTime());
	//Rain Particle Update
	//mRain.Update(dt, mTimer.TotalTime());
}

void GameApp::DrawScene()
{
	//在非菜单状态下渲染游戏场景
	if (!menu&&LoadProgress==100)
	{
		
		//GUI pre render(if need Render Offscreen)
		m_pGameGUI->PreRender();

		//shadow map
		BuildShadowMap();
		

		md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&GColors::Silver));
		md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		md3dImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);
		md3dImmediateContext->RSSetViewports(1, &mScreenViewport);
		float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		md3dImmediateContext->OMSetBlendState(m_pBlendState, blendFactor, 0xFFFFFFFF);
		//sky box render
		mSkyBox->Draw(md3dImmediateContext, *mCamera);
		//restore default states, as the SkyFX changes them in the effect file.
		md3dImmediateContext->RSSetState(0);
		md3dImmediateContext->OMSetDepthStencilState(0, 0);

		md3dImmediateContext->IASetInputLayout(InputLayouts::Basic32);
		md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		

		UINT stride = sizeof(Vertex::Basic32);
		UINT offset = 0;

		XMMATRIX view = mCamera->View();
		XMMATRIX proj = mCamera->Proj();
		XMMATRIX viewProj = view * proj;

		// Set per frame constants.
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

		mTerrain.Render(md3dImmediateContext,gDirLights,mShadowMap);
		md3dImmediateContext->OMSetDepthStencilState(0, 0);

		//	// Restore default render state.
			//md3dImmediateContext->RSSetState(RenderStates::WireframeRS);
		//}

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
			Effects::BasicFX->SetDiffuseMap(mSRV);
			Effects::BasicFX->SetFogRange(175.0f);
			landAndWavesTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
			md3dImmediateContext->DrawIndexed(mLandIndexCount, 0, 0);

		}
		//fbx model
		md3dImmediateContext->RSSetState(RenderStates::CullBackRS);
		RenderFbxModel();
		//render gui
		m_pGameGUI->Render();
		//draw particle
		md3dImmediateContext->OMSetBlendState(0, blendFactor, 0xffffffff); // restore default
		DrawParticle();

		// restore default states.
		md3dImmediateContext->RSSetState(0);
		md3dImmediateContext->OMSetDepthStencilState(0, 0);
		md3dImmediateContext->OMSetBlendState(0, blendFactor, 0xffffffff);
		HR(mSwapChain->Present(0, 0));
	}
	
}

void GameApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	if (!menu)
	{
		//记录鼠标选中的地图索引(以便寻路算法使用)
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

	geoGen.CreateGrid(2000.0f, 2000.0f, 200, 200, grid);

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
	//在单位地图范围内检测射线与地图的碰撞
	for (int i = 0; i < Map_size; ++i)
	{
		for (int j = 0; j < Map_size; ++j)
		{
			// Ray definition in view space.
			 rayOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
			 rayDir = XMVectorSet(vx, vy, 1.0f, 0.0f);


			XMMATRIX W = XMLoadFloat4x4(&mTerrain.mInstance[i*Map_size+j].World);
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

	//返回相交地图的序号;
	return XMINT2(Map_x, Map_y);
}

void GameApp::FindPath()
{
	if (!menu)
	{
		//重置寻路
		if (unitlen == 0 || unitlen == Unit_MapOffset)
		{
			if (Mapindex.x >= 0)
			{
				Endindex = Mapindex;
				First = true;
			}
		}


		static float x = PlayerPositionX;
		static float z = PlayerPositionY;
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
				m_ModeInfo[7].Mat_tansform_Translation[0] = XMMatrixTranslation(x, 00.0f, z);
				//更新玩家位置信息
				PlayerWorldPosition = XMFLOAT3(x, 0.0f, z);
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
			if (!m_Models[2]->BeginRender(i, Anim_State::Idle))
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

	if (m_Models[2])
		m_Models[2]->Update(dt,Anim_State::Idle);
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
	if (GetAsyncKeyState('W') & 0x8000)
		mCamera->Walk(2.0f);

	if (GetAsyncKeyState('S') & 0x8000)
		mCamera->Walk(-2.0f);

	if (GetAsyncKeyState('A') & 0x8000)
		mCamera->Strafe(-2.0f);

	if (GetAsyncKeyState('D') & 0x8000)
		mCamera->Strafe(2.0f);

	//更新像机视图矩阵
	mCamera->UpdateViewMatrix();
}

void GameApp::InitFbxModel()
{

	///fbx model tansform information initialization(与模型下标统一)
	{
		//Model 1  (蜘蛛怪)
		m_ModeInfo[0].Model_Instance_Num = Model1_Instance;
		m_ModeInfo[0].Mat_tansform_Rot_Scal.resize(Model1_Instance);
		m_ModeInfo[0].Mat_tansform_Translation.resize(Model1_Instance);
		for (int i = 0; i < Model1_Instance; ++i)
		{
			int location_offset_x = (int)MathHelper::RandF(-2.0f, 19.0f) * Unit_MapOffset;
			int location_offset_z = (int)MathHelper::RandF(-2.0f, 19.0f) * Unit_MapOffset;
			m_ModeInfo[0].Mat_tansform_Rot_Scal[i] = XMMatrixRotationX(-MathHelper::Pi / 2) * XMMatrixScaling(0.055f, 0.055f, 0.055f);
			m_ModeInfo[0].Mat_tansform_Translation[i] = XMMatrixTranslation(location_offset_x+4.5f, 0.0f, 3.0f + location_offset_z);
		}

		//Model 2 (树)
		m_ModeInfo[1].Model_Instance_Num = Model2_Instance;
		m_ModeInfo[1].Mat_World.resize(Model2_Instance);
		for (int i = 0; i < Model2_Instance; ++i)
		{
			int location_offset_x = (int)MathHelper::RandF(-5.0f, 16.0f) * Unit_MapOffset;
			int location_offset_z = (int)MathHelper::RandF(-5.0f, 16.0f) * Unit_MapOffset;
			m_ModeInfo[1].Mat_World[i] = XMMatrixRotationY(location_offset_x % 3) 
				* XMMatrixScaling(0.15f, 0.15f, 0.15f)
				*XMMatrixTranslation(-13.0f + location_offset_x, 0.0f, 5.0f + location_offset_z);
		}


		//Model 3 (白鹤)
		m_ModeInfo[2].Model_Instance_Num = Model3_Instance;
		m_ModeInfo[2].Mat_World.resize(Model3_Instance);
		for (int i = 0; i < Model3_Instance; ++i)
		{
			int location_offset_x = (int)MathHelper::RandF(-1.0f, 64.0f) * 4;
			int location_offset_z = (int)MathHelper::RandF(-1.0f, 64.0f) * 4;
			m_ModeInfo[2].Mat_World[i] = XMMatrixRotationX(-MathHelper::Pi / 2) 
				* XMMatrixScaling(5.2f, 5.2f,5.2f)
				* XMMatrixTranslation(20.0f + location_offset_x, 70.0f, 5.0f + location_offset_z);
		}


		//Model 4 (石山)
		m_ModeInfo[3].Model_Instance_Num = Model4_Instance;
		m_ModeInfo[3].Mat_World.resize(Model4_Instance);
		for (int i = 0; i < Model4_Instance; ++i)
		{
			int location_offset_x = (int)MathHelper::RandF(-5.0f, 16.0f) * Unit_MapOffset;
			int location_offset_z = (int)MathHelper::RandF(-5.0f, 16.0f) * Unit_MapOffset;
			m_ModeInfo[3].Mat_World[i] = XMMatrixRotationX(MathHelper::Pi / 2) 
				* XMMatrixRotationY(location_offset_x % 3) 
				* XMMatrixScaling(0.007f, 0.007f, 0.007f)
				* XMMatrixTranslation(5.0f + location_offset_x, -4.0f, -9.0f + location_offset_z);
		}


		//Model 5  (房屋1)
		m_ModeInfo[4].Model_Instance_Num = Model5_Instance;
		m_ModeInfo[4].Mat_World.resize(Model5_Instance);
		for (int i = 0; i < Model5_Instance; ++i)
		{
			int location_offset_x = (int)MathHelper::RandF(-50.0f, 50.0f) * 4;
			int location_offset_z = (int)MathHelper::RandF(-50.0f, 50.0f) * 4;
			m_ModeInfo[4].Mat_World[i] = XMMatrixRotationX(-MathHelper::Pi / 2) 
				* XMMatrixScaling(2.0f, 2.0f, 2.0f)
				* XMMatrixTranslation(20.0f + location_offset_x, 0.0f, 100.0f + location_offset_z);
		}

		//Model 6 (神像)
		m_ModeInfo[5].Model_Instance_Num = Model6_Instance;
		m_ModeInfo[5].Mat_World.resize(Model6_Instance);
		for (int i = 0; i < Model6_Instance; ++i)
		{
			int location_offset_x = (int)MathHelper::RandF(-3.0f, 16.0f) * Unit_MapOffset;
			int location_offset_z = (int)MathHelper::RandF(-3.0f, 16.0f) * Unit_MapOffset;
			m_ModeInfo[5].Mat_World[i] = XMMatrixRotationX(-MathHelper::Pi / 2) 
				* XMMatrixRotationY(location_offset_x % 3) 
				* XMMatrixScaling(5.0f, 5.0f, 5.0f)
				* XMMatrixTranslation(location_offset_x+5.0f, 0.0f, location_offset_z + 5.0f);
		}

		//Model 7 (小孩石像)
		m_ModeInfo[6].Model_Instance_Num = Model7_Instance;
		m_ModeInfo[6].Mat_World.resize(Model7_Instance);
		for (int i = 0; i < Model7_Instance; ++i)
		{
			int location_offset_x = (int)MathHelper::RandF(-3.0f, 16.0f) * Unit_MapOffset;
			int location_offset_z = (int)MathHelper::RandF(-3.0f, 16.0f) * Unit_MapOffset;
			m_ModeInfo[6].Mat_World[i] = XMMatrixRotationX(-MathHelper::Pi / 2) 
				* XMMatrixRotationY(location_offset_x % 3) 
				* XMMatrixScaling(6.0f, 6.0f, 6.0f)
				* XMMatrixTranslation(location_offset_x+5.0f, 0.0f, location_offset_z + 5.0f);
		}

		//Model 8 PLAYER MODEL
		m_ModeInfo[7].Model_Instance_Num = Model8_Instance;
		m_ModeInfo[7].Mat_tansform_Rot_Scal.resize(Model8_Instance);
		m_ModeInfo[7].Mat_tansform_Translation.resize(Model8_Instance);
		for (int i = 0; i < Model8_Instance; ++i)
		{
			m_ModeInfo[7].Mat_tansform_Rot_Scal[i] = XMMatrixRotationX(-MathHelper::Pi / 2) * XMMatrixRotationY(0) * XMMatrixScaling(0.002f ,0.002f, 0.002f);
			m_ModeInfo[7].Mat_tansform_Translation[i] = XMMatrixTranslation(PlayerPositionX, 0.0f, PlayerPositionY);
		}

	}

	if (m_Models[0])
	{
		m_Models[0]->CreateFileFbx("model/GiantSpider.FBX");
		m_Models[0]->CreateFileKkb("model/GiantSpider.kkb");
		m_Models[0]->CreateFileKkf("model/GiantSpider@Idle.kkf",Anim_State::Idle);
		m_Models[0]->CreateImage(L"model/GiantSpider_04.dds");
		m_Models[0]->CreateNormalTexture(L"model/GiantSpider_04_NRM.dds");
		m_Models[0]->SetModelTansInfo(&m_ModeInfo[0]);
	}
	if (m_Models[1])
	{
		m_Models[1]->CreateFileFbx("model/Blue_Tree_02a.FBX");
		m_Models[1]->CreateFileKkb("model/Blue_Tree_02a.kkb");
		m_Models[1]->CreateImage(L"model/Blue_Tree2.dds");
		m_Models[1]->CreateNormalTexture(L"model/Blue_Tree2_NRM.dds");
		m_Models[1]->SetModelTansInfo(&m_ModeInfo[1]);
	}

	if (m_Models[2])
	{
		m_Models[2]->CreateFileFbx("model/BaiLu_Fei_Loop.FBX");
		m_Models[2]->CreateFileKkb("model/BaiLu_Fei_Loop.kkb");
		m_Models[2]->CreateFileKkf("model/BaiLu_Fei_Loop.kkf",Anim_State::Idle);
		m_Models[2]->CreateImage(L"model/bailu.dds");
		m_Models[2]->CreateNormalTexture(L"model/bailu_NRM.dds");
		m_Models[2]->SetModelTansInfo(&m_ModeInfo[2]);
	}
	if (m_Models[3])
	{
		m_Models[3]->CreateFileFbx("model/shan06.FBX");
		m_Models[3]->CreateFileKkb("model/shan06.kkb");
		m_Models[3]->CreateImage(L"model/zzTex3.dds");
		m_Models[3]->CreateNormalTexture(L"model/zzTex3_NRM.dds");
		m_Models[3]->SetModelTansInfo(&m_ModeInfo[3]);
	}

	if (m_Models[4])
	{
		m_Models[4]->CreateFileFbx("model/a.FBX");
		m_Models[4]->CreateFileKkb("model/a.kkb");
		m_Models[4]->CreateImage(L"model/UB25201.dds");
		m_Models[4]->CreateNormalTexture(L"model/UB25201_NRM.dds");
		m_Models[4]->SetModelTansInfo(&m_ModeInfo[4]);
	}

	if (m_Models[5])
	{
		m_Models[5]->CreateFileFbx("model/aa.FBX");
		m_Models[5]->CreateFileKkb("model/aa.kkb");
		m_Models[5]->CreateImage(L"model/UC42002.dds");
		m_Models[5]->CreateNormalTexture(L"model/UC42002_NRM.dds");
		m_Models[5]->SetModelTansInfo(&m_ModeInfo[5]);
	}

	if (m_Models[6])
	{
		m_Models[6]->CreateFileFbx("model/child.FBX");
		m_Models[6]->CreateFileKkb("model/child.kkb");
		m_Models[6]->CreateImage(L"model/UC40701.dds");
		m_Models[6]->CreateNormalTexture(L"model/UC40701_NRM.dds");
		m_Models[6]->SetModelTansInfo(&m_ModeInfo[6]);
	}
	//player model
	if (m_Models[7])
	{
		m_Models[7]->CreateFileFbx("model/MercFemale.FBX");
		m_Models[7]->CreateFileKkb("model/MercFemale.kkb");
		m_Models[7]->CreateFileKkf("model/MercFemale_Run.kkf", Anim_State::Run);
		m_Models[7]->CreateFileKkf("model/MercFemale_IdleLoop.kkf", Anim_State::Idle);
		m_Models[7]->CreateImage(L"model/MercFemale_Albedo.dds");
		m_Models[7]->CreateNormalTexture(L"model/MercFemale_Albedo_NRM.dds");
		m_Models[7]->SetModelTansInfo(&m_ModeInfo[7]);
	}

	// 在InitFbxModel()之后调用
	BuildStaticInstanceData();
	BuildDynamicInstanceData();

	LoadProgress = 100;
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
		L"Textures/water3.dds", &texResource, &mSRV));
	ReleaseCOM(texResource); // view saves reference

}

//初始化调用
void GameApp::BuildStaticInstanceData()
{
	for (int i = 0; i < Model_Num; ++i)
	{
		//判断是否为静态模型
		if(m_ModeInfo[i].Mat_tansform_Rot_Scal.size()==0)
		{

			D3D11_BUFFER_DESC vbd;
			vbd.Usage = D3D11_USAGE_DYNAMIC;
			vbd.ByteWidth = sizeof(XMMATRIX) * m_ModeInfo[i].Mat_World.size();
			vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			vbd.MiscFlags = 0;
			vbd.StructureByteStride = 0;
			D3D11_SUBRESOURCE_DATA vinitData;
			vinitData.pSysMem = &m_ModeInfo[i].Mat_World[0];
			HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mFBXInstanceBuffer[i]));
		}
	}



}


//每帧调用
void GameApp::BuildDynamicInstanceData()
{
	for (int i = 0; i < Model_Num; ++i)
	{
		//判断是否为动态模型
		if (m_ModeInfo[i].Mat_tansform_Rot_Scal.size ()!= 0)
		{
			//更新world矩阵
			int size = m_ModeInfo[i].Mat_tansform_Rot_Scal.size();
			m_ModeInfo[i].Mat_World.resize(size);
			for (int j=0;j< size;++j)
			{
				m_ModeInfo[i].Mat_World[j] = m_ModeInfo[i].Mat_tansform_Rot_Scal[j]
					* m_ModeInfo[i].Mat_tansform_Translation[j];
			}

			m_Models[i]->SetModelTansInfo(&m_ModeInfo[i]);

			////创建buffer
			//D3D11_BUFFER_DESC vbd;
			//vbd.Usage = D3D11_USAGE_DYNAMIC;
			//vbd.ByteWidth = sizeof(XMMATRIX) * m_ModeInfo[i].Mat_World.size();
			//vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			//vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			//vbd.MiscFlags = 0;
			//vbd.StructureByteStride = 0;
			//D3D11_SUBRESOURCE_DATA vinitData;
			//vinitData.pSysMem = &m_ModeInfo[i].Mat_World[0];
			//HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mFBXInstanceBuffer[i]));
		}
	}


}

void GameApp::BuildShadowMap()
{
	mShadowMap->BindDsvAndSetNullRenderTarget(md3dImmediateContext);
	md3dImmediateContext->OMSetDepthStencilState(0, 0);
	
	
	// move model
	if (m_Models[0])
	{
		for (int i = 0; i < Model1_Instance; ++i)
		{
			if (!m_Models[0]->ShadowRender(i, Anim_State::Idle,mShadowMap))
				return;
		}
	}

	if (m_Models[7])
	{
		for (int i = 0; i < Model8_Instance; ++i)
		{
			if (mAnim == Anim_State::Idle)
			{
				if (!m_Models[7]->ShadowRender(i, Anim_State::Idle, mShadowMap))
					return;
			}
			else if (mAnim == Anim_State::Run)
			{
				if (!m_Models[7]->ShadowRender(i, Anim_State::Run, mShadowMap))
					return;
			}
		}
	}

	if (m_Models[2])
	{
		for (int i = 0; i < Model3_Instance; ++i)
		{
			if (!m_Models[2]->ShadowRender(i, Anim_State::Idle, mShadowMap))
				return;
		}
	}



	//not move model 
	if (m_Models[3])
	{
			if (!m_Models[3]->ShadowRender(Model4_Instance, mShadowMap, mFBXInstanceBuffer[3]))
				return;
	}

	if (m_Models[4])
	{
			if (!m_Models[4]->ShadowRender(Model5_Instance, mShadowMap, mFBXInstanceBuffer[4]))
				return;
	}

	if (m_Models[5])
	{
			if (!m_Models[5]->ShadowRender(Model6_Instance, mShadowMap, mFBXInstanceBuffer[5]))
				return;
	}

	if (m_Models[6])
	{
			if (!m_Models[6]->ShadowRender(Model7_Instance, mShadowMap, mFBXInstanceBuffer[6]))
				return;
	}

	if (m_Models[1])
	{
			if (!m_Models[1]->ShadowRender(Model2_Instance, mShadowMap, mFBXInstanceBuffer[1]))
				return;
	}

}

void GameApp::InitParticleSystem()
{
	mRandomTexSRV = d3dHelper::CreateRandomTexture1DSRV(md3dDevice);

	std::vector<std::wstring> flares;
	flares.push_back(L"Textures\\th.dds");
	mFireTexSRV = d3dHelper::CreateTexture2DArraySRV(md3dDevice, md3dImmediateContext, flares);
	//Fire Particle
	mFire.Init(md3dDevice, Effects::FireFX, mFireTexSRV, mRandomTexSRV, 500);
	mFire.SetEmitPos(XMFLOAT3(0.0f, 5.0f, 100.0f));
	//Rain Particle
	std::vector<std::wstring> raindrops;
	raindrops.push_back(L"Textures\\raindrop.dds");
	mRainTexSRV = d3dHelper::CreateTexture2DArraySRV(md3dDevice, md3dImmediateContext, raindrops);

	mRain.Init(md3dDevice, Effects::RainFX, mRainTexSRV, mRandomTexSRV, 10000);
}

void GameApp::DrawParticle()
{
	// Draw particle systems last so it is blended with scene.
	mFire.SetEyePos(mCamera->GetPosition());
	mFire.Draw(md3dImmediateContext);

	/*mRain.SetEyePos(mCamera->GetPosition());
	mRain.SetEmitPos(mCamera->GetPosition());
	mRain.Draw(md3dImmediateContext);*/

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

	 //把UI和游戏主逻辑分为两个线程
     th1.join();
     th2.join();
     return 0;
 }
