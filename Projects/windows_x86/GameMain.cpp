#include <NsApp/EntryPoint.h>
#include <thread>
#include <DirectXCollision.h>
#include <map>

#include "app.h"
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
#include "SenceManager.h"
#include "Water.h"
//debug
std::string str;

////////////////////////////////////////////////////////////////////////////////////////////////////
//model num
#define Model_Num  8

//map size
#define Map_size  10
#define Unit_MapOffset 16


//player init position
//#define PlayerPositionX -60.0f 
//#define PlayerPositionY -60.0f

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
	Idle = 1,
	Run = 2,
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
	bool FindPath();
	void RenderFbxModel();
	void UpdateFbxAnimation(float dt);
	void GetKeyState(float dt);
	//fbx model tansform information initialization
	void InitFbxModel();
	//Create Blend State
	void CreateBlendState();
	//Load Texture
	void LoadTexture();

	//Build  shadow instance data
	//static
	void BuildStaticInstanceData();
	//dynamic
	void BuildDynamicInstanceData();

	//Draw shadow map
	void BuildShadowMap();

	//Init Particle System
	void InitParticleSystem();
	//Draw Particle
	void DrawParticle();
	//Cull Sence
	void CullSence();

	///Water///
	//Draw water Refraction map
	void DrawWater();
	void DrawWaterRefractionMap();
	void DrawWaterReflectionMap();
private:
	ID3D11Buffer* mLandVB;
	ID3D11Buffer* mWaterTangetVB;
	ID3D11Buffer* mLandIB;

	ID3D11Buffer* mWavesVB;
	ID3D11Buffer* mWavesIB;

	//instance buffer data
	ID3D11Buffer* mFBXInstanceBuffer[Model_Num];
	//water texture
	ID3D11ShaderResourceView* mWaterNormalSRV;

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
	//player position
	XMINT2 PlayerPosition;

	//FBX Models
	std::vector<Model> m_Models;
	std::vector <Model_Tansform_Info*> m_PlayerInfo;
	int LoadProgress;

	//box bounding
	BoundingBox MapBox;
	//camera Frustum
	BoundingFrustum CameraFrustum;
	BoundingBox CullingMapBox;
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
	//water maps
	Water* mWater;
	XMFLOAT4 WaveParams;
	//Particle System
	ID3D11ShaderResourceView* mFireTexSRV;
	ID3D11ShaderResourceView* mRainTexSRV;
	ID3D11ShaderResourceView* mRandomTexSRV;
	ParticleSystem mFire;
	ParticleSystem mRain;
	//sence manager
	SenceManager TerrainManager;
	SenceManager FbxModelManager;
	std::vector<UnitSence> TerrainSenceData;
	std::vector<UnitSence> ModelSenceData;
	//场景裁剪后的地图数据
	std::vector<UnitSence> VisibleModelData;
	std::vector<UnitSence> VisibleTerrainData;
	std::vector<Model_Tansform_Info>  LocalDynamicModelInfo;
	std::vector<Model_Tansform_Info>  LocalStaticModelInfo;
};


GameApp::GameApp(HINSTANCE hInstance)
	: D3DApp(hInstance), LoadProgress(0),mLandVB(0), mLandIB(0), mWavesVB(0), mWavesIB(0),
	mWaterTexOffset(0.0f, 0.0f), mEyePosW(0.0f, 0.0f, 0.0f), mLandIndexCount(0), mRenderOptions(Render_Options::TexturesAndFog),
	mTheta(1.3f * MathHelper::Pi), mPhi(0.4f * MathHelper::Pi), mRadius(80.0f), 
	mCamera(nullptr), PlayerPositionIndex(0,0), Mapindex(0,0), First(true), Endindex(0,0), unitlen(0),isMove(false)
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
	
	WaveParams = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);

	
	//camera init
	if (mCamera == NULL)
	{
		mCamera = Camera::Get();
	}


	MapBox.Center=XMFLOAT3(0.0f,0.0f,0.0f);
	MapBox.Extents= XMFLOAT3(0.5f, 0.5f, 0.5f);
	
	CullingMapBox.Center= XMFLOAT3(0.0f, 0.0f, 0.0f);
	CullingMapBox.Extents= XMFLOAT3(80.0f,7.5f, 80.0f);//16x10/2=80

	XMMATRIX grassTexScale = XMMatrixScaling(100.0f, 100.0f, 0.0f);
	XMStoreFloat4x4(&mGrassTexTransform, grassTexScale);

	//GUI init
	m_pGameGUI = new GameGUI;

	//Direction light init
	gDirLights.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	gDirLights.Diffuse = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	gDirLights.Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	gDirLights.Direction = XMFLOAT3(0.57735f, -0.57735f, -0.57735f);


	//mat init
	mLandMat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mLandMat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mLandMat.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);

	mWavesMat.Ambient = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
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
	ReleaseCOM(mWaterNormalSRV);


	Effects::DestroyAll();
	InputLayouts::DestroyAll();
	RenderStates::DestroyAll();


	//camera release
	SafeDelete(mCamera);


	//gui release
	SafeDelete(m_pGameGUI);


	//sky box release
	SafeDelete(mSkyBox);
	//shadow map release
	SafeDelete(mShadowMap);
	//water release
	SafeDelete(mWater);
}

bool GameApp::Init()
{
	//初始化父类
	if (!D3DApp::Init())
		return false;

	//Terrain Sence Manager

	TerrainManager.ReadMapDataFormFile("Sence/Terrain.txt");
	TerrainManager.DivisionMap(Unit_MapOffset);
	TerrainSenceData = TerrainManager.GetSenceData();
	
	//Model Sence Manager
	std::vector<std::vector<char>> AIMapData;
	FbxModelManager.ReadMapDataFormFile("Sence/Model.txt");
	FbxModelManager.DivisionMap(Unit_MapOffset,&AIMapData);
	ModelSenceData = FbxModelManager.GetSenceData();
	
	//Terrain Init
	mTerrain.InitTerrain(md3dDevice,Map_size, Unit_MapOffset, TerrainSenceData);
	mTerrain.InitModel(md3dDevice, Map_size, Unit_MapOffset, ModelSenceData,m_PlayerInfo);
	
	//转换地图
	m_GameMap.ConvertMap(AIMapData);
	
	// Must init Effects first since InputLayouts depend on shader signatures.
	Effects::InitAll(md3dDevice);
	InputLayouts::InitAll(md3dDevice);
	RenderStates::InitAll(md3dDevice);
	
	//sky box init
	mSkyBox = new Sky(md3dDevice,L"Textures/sunsetcube1024.dds",1000.0f);
	
	//shadow map init
	mShadowMap = new ShadowMap(md3dDevice, SMapSize, SMapSize);
	if(mShadowMap)
	{
		mShadowMap->BuildShadowTransform(gDirLights);
	}
	
	//water maps init
	mWater = new Water(md3dDevice, mClientWidth, mClientHeight);

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
	//mCamera->LookAt(XMFLOAT3(PlayerWorldPosition.x + 60, 100, PlayerWorldPosition.z -60), PlayerWorldPosition,XMFLOAT3(0.0f,1.0f,0.0f));
	
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
	
	//create Camera Frustum
	BoundingFrustum::CreateFromMatrix(CameraFrustum, mCamera->Proj());
}

void GameApp::UpdateScene(float dt)
{

	if (!menu)
	{
		//每帧寻路
		//判断动画状态
		if (FindPath())
		{//寻路成功
			if ((PlayerPositionIndex.x == Mapindex.x) && (PlayerPositionIndex.y == Mapindex.y) && !isMove)
			{
				mAnim = Anim_State::Idle;
			}
			else
			{
				mAnim = Anim_State::Run;
			}
		}
		else//寻路失败
		{
			mAnim = Anim_State::Idle;
		}
		if (Mapindex.x < 0 && unitlen == Unit_MapOffset)
		{
			mAnim = Anim_State::Idle;
		}

		//更新模型实例化buffer
		BuildStaticInstanceData();
		BuildDynamicInstanceData();

		//mCamera->SetPosition(PlayerWorldPosition.x + 60, 100, PlayerWorldPosition.z - 60);
		//mFire.SetEmitPos(XMFLOAT3(PlayerWorldPosition.x-10, PlayerWorldPosition.y +5 , PlayerWorldPosition.z +10 ));

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

		//culling sence
		CullSence();

		//Update Fbx Animation
		UpdateFbxAnimation(dt);

		//gui update
		m_pGameGUI->Update(dt);
		//Fire Particle Update
		//mFire.Update(dt, mTimer.TotalTime());
		//Rain Particle Update
		mRain.Update(dt, mTimer.TotalTime());

		//update water
		WaveParams.w = mTimer.TotalTime();
	}
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

		


		md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&GColors::White));
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
		//
		// Draw the terrain with alpha clipping.
		// 

		mTerrain.Render(md3dImmediateContext,gDirLights,mShadowMap,false);
		md3dImmediateContext->OMSetDepthStencilState(0, 0);

		//	// Restore default render state.
			//md3dImmediateContext->RSSetState(RenderStates::WireframeRS);
		//}

	

		//fbx model
		md3dImmediateContext->RSSetState(RenderStates::CullBackRS);
		RenderFbxModel();

		

		//draw particle
		md3dImmediateContext->OMSetBlendState(m_pBlendState, blendFactor, 0xFFFFFFFF);
		DrawParticle();

		//water
		DrawWater();



		//render gui
		m_pGameGUI->Render();

	
		// restore default states.
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

	geoGen.CreateGrid(1000.0f, 1000.0f, 10, 10, grid);

	mLandIndexCount = grid.Indices.size();

	//
	// Extract the vertex elements we are interested and apply the height function to
	// each vertex.  
	//

	std::vector<Vertex::Basic32> vertices(grid.Vertices.size());
	std::vector<Vertex::PosNormalTexTan> Watervertices(grid.Vertices.size());
	for (UINT i = 0; i < grid.Vertices.size(); ++i)
	{
		XMFLOAT3 p = grid.Vertices[i].Position;

		//p.y = GetHillHeight(p.x, p.z);

		vertices[i].Pos = p;
		vertices[i].Normal = XMFLOAT3(0.0f,1.0f,0.0f);
		vertices[i].Tex = grid.Vertices[i].TexC;


		Watervertices[i].Pos = p;
		Watervertices[i].Normal = grid.Vertices[i].Normal;
		Watervertices[i].Tex = grid.Vertices[i].TexC;
		Watervertices[i].TangentU = grid.Vertices[i].TangentU;
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
	//water tanget pos
	vbd.ByteWidth = sizeof(Vertex::PosNormalTexTan) * grid.Vertices.size();
	vinitData.pSysMem = &Watervertices[0];
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mWaterTangetVB));

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
	for (int i = 0; i < TerrainSenceData.size(); ++i) 
	{
		for (int j = 0; j < TerrainSenceData[i].TansformData.size(); ++j)
		{
			// Ray definition in view space.
			 rayOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
			 rayDir = XMVectorSet(vx, vy, 1.0f, 0.0f);


			XMMATRIX W = XMLoadFloat4x4(&TerrainSenceData[i].TansformData[j]);
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
					Map_x = TerrainSenceData[i].IndexData[j].x;
					Map_y = TerrainSenceData[i].IndexData[j].y;
					lmin = tmin;
				}
			}
		}
	}

	//返回相交地图的序号;
	return XMINT2(Map_x, Map_y);
}

bool GameApp::FindPath()
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


		static float x = PlayerWorldPosition.x;
		static float z = PlayerWorldPosition.z;
		static bool findpath = false;
		
		{
			if (First || unitlen >= Unit_MapOffset)
			{
				if (Mapindex.x < 0)
				{
					return false;
				}

				findpath = m_GameMap.FindNextDirection(&m_Dir, PlayerPositionIndex.x, PlayerPositionIndex.y, Endindex.x, Endindex.y, ROT);
				//寻路失败
				if (!findpath)
					return false;
				
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
						m_PlayerInfo[0]->Mat_tansform_Rot_Scal[0] *= XMMatrixRotationY(MathHelper::Pi / 2);

					}
					else if (ROT == TurnLeft)
					{
						m_PlayerInfo[0]->Mat_tansform_Rot_Scal[0] *= XMMatrixRotationY(-MathHelper::Pi / 2);
					}
					else if (ROT == BackWard)
					{
						m_PlayerInfo[0]->Mat_tansform_Rot_Scal[0] *= XMMatrixRotationY(MathHelper::Pi);
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
			
				m_PlayerInfo[0]->Mat_tansform_Translation[0] = XMMatrixTranslation(x, 7.5f, z);
				//更新玩家位置信息
				PlayerWorldPosition = XMFLOAT3(x, 7.5f, z);
			}
		}
	}
	return true;
}

void GameApp::RenderFbxModel()
{
	//render fbx model

	for (int index = 0; index < Model_Num; ++index)
	{
		if (LocalStaticModelInfo[index].Mat_World.size() != 0)//静态模型
		{

			for (int j = 0; j < LocalStaticModelInfo[index].Mat_World.size(); ++j)
			{
				if (!m_Models[index].BeginRender(j))
					return;
			}

		}
		else if (LocalDynamicModelInfo[index].Mat_World.size() != 0)//动态模型
		{
			if (index == 7)
			{
				if (mAnim == Anim_State::Idle)
				{
					if (!m_Models[index].BeginRender(0, Anim_State::Idle))
						return;
				}
				else if (mAnim == Anim_State::Run)
				{
					if (!m_Models[index].BeginRender(0, Anim_State::Run))
						return;
				}
			}
			else
			{
				for (int j = 0; j < LocalDynamicModelInfo[index].Mat_World.size(); ++j)
				{
					if (!m_Models[index].BeginRender(j, Anim_State::Idle))
						return;
				}
			}
		}
	}
	//reset input layout
	md3dImmediateContext->IASetInputLayout(InputLayouts::Basic32);

}

void GameApp::UpdateFbxAnimation(float dt)
{
	//fbx update
	m_Models[0].Update(dt, Anim_State::Idle);
	m_Models[2].Update(dt, Anim_State::Idle);

	//player model update
	if (mAnim == Anim_State::Idle)
	{
		m_Models[7].Update(dt, Anim_State::Idle);
	}
	else if (mAnim == Anim_State::Run)
	{
		m_Models[7].Update(dt * 1.2f, Anim_State::Run);
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
	//player position init
	XMFLOAT4X4 PlayerTranslation;
	XMStoreFloat4x4(&PlayerTranslation, m_PlayerInfo[0]->Mat_tansform_Translation[0]);
	PlayerWorldPosition = XMFLOAT3(PlayerTranslation._41, 0.0f, PlayerTranslation._43);
	

	m_Models.resize(Model_Num);
	//Model 1  (蜘蛛怪)
	{
		m_Models[0].CreateFileFbx("model/GiantSpider.FBX");
		m_Models[0].CreateFileKkb("model/GiantSpider.kkb");
		m_Models[0].CreateFileKkf("model/GiantSpider@Idle.kkf",Anim_State::Idle);
		m_Models[0].CreateImage(L"model/GiantSpider_04.dds");
		m_Models[0].CreateNormalTexture(L"model/GiantSpider_04_NRM.dds");
	}
	//Model 2 (树)
	{
		m_Models[1].CreateFileFbx("model/Blue_Tree_02a.FBX");
		m_Models[1].CreateFileKkb("model/Blue_Tree_02a.kkb");
		m_Models[1].CreateImage(L"model/Blue_Tree2.dds");
		m_Models[1].CreateNormalTexture(L"model/Blue_Tree2_NRM.dds");
	}
	//Model 3 (白鹤)
	{
		m_Models[2].CreateFileFbx("model/BaiLu_Fei_Loop.FBX");
		m_Models[2].CreateFileKkb("model/BaiLu_Fei_Loop.kkb");
		m_Models[2].CreateFileKkf("model/BaiLu_Fei_Loop.kkf",Anim_State::Idle);
		m_Models[2].CreateImage(L"model/bailu.dds");
		m_Models[2].CreateNormalTexture(L"model/bailu_NRM.dds");
	}
	//Model 4 (石山)
	{
		m_Models[3].CreateFileFbx("model/shan06.FBX");
		m_Models[3].CreateFileKkb("model/shan06.kkb");
		m_Models[3].CreateImage(L"model/zzTex3.dds");
		m_Models[3].CreateNormalTexture(L"model/zzTex3_NRM.dds");
	}
	//Model 5  (房屋1)
	{
		m_Models[4].CreateFileFbx("model/a.FBX");
		m_Models[4].CreateFileKkb("model/a.kkb");
		m_Models[4].CreateImage(L"model/UB25201.dds");
		m_Models[4].CreateNormalTexture(L"model/UB25201_NRM.dds");
	}
	//Model 6 (神像)
	{
		m_Models[5].CreateFileFbx("model/aa.FBX");
		m_Models[5].CreateFileKkb("model/aa.kkb");
		m_Models[5].CreateImage(L"model/UC42002.dds");
		m_Models[5].CreateNormalTexture(L"model/UC42002_NRM.dds");
	}
	//Model 7 (小孩石像)
	{
		m_Models[6].CreateFileFbx("model/child.FBX");
		m_Models[6].CreateFileKkb("model/child.kkb");
		m_Models[6].CreateImage(L"model/UC40701.dds");
		m_Models[6].CreateNormalTexture(L"model/UC40701_NRM.dds");
	}
	//Model 8 PLAYER MODEL
	{

		m_Models[7].CreateFileFbx("model/TraumaGuard.FBX");
		m_Models[7].CreateFileKkb("model/TraumaGuard.kkb");
		m_Models[7].CreateFileKkf("model/TraumaGuard_Run.kkf", Anim_State::Run);
		m_Models[7].CreateFileKkf("model/TraumaGuard_ActiveIdleLoop.kkf", Anim_State::Idle);
		m_Models[7].CreateImage(L"model/TraumaGuard_Albedo.dds");
		m_Models[7].CreateNormalTexture(L"model/TraumaGuard_Normal.dds");
		m_Models[7].SetModelTansInfo(m_PlayerInfo[0]);
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
		L"Textures/wavesbump.dds", &texResource, &mWaterNormalSRV));
	ReleaseCOM(texResource); // view saves reference

}

//初始化调用
void GameApp::BuildStaticInstanceData()
{
	int Modelindex = 0;
	LocalStaticModelInfo.clear();
	LocalStaticModelInfo.resize(Model_Num);
	for (auto& VisitbleSence : VisibleModelData)
	{
		for (auto& ModelInfo : VisitbleSence.ModelInfo)
		{
			//判断是否为静态模型
			if (ModelInfo.second.Mat_tansform_Rot_Scal.size() == 0)
			{
				Modelindex = atoi(&ModelInfo.first) - 1;
				for (int j = 0; j < ModelInfo.second.Mat_World.size(); ++j)
				{
					LocalStaticModelInfo[Modelindex].Mat_World.push_back(ModelInfo.second.Mat_World[j]);
				}
			}
		}
	}


	//更新裁剪后的静态场景
	for (int i = 0; i < LocalStaticModelInfo.size(); ++i)
	{
		if (LocalStaticModelInfo[i].Mat_World.size() != 0)
		{
			D3D11_BUFFER_DESC vbd;
			vbd.Usage = D3D11_USAGE_DYNAMIC;
			vbd.ByteWidth = sizeof(XMMATRIX) * LocalStaticModelInfo[i].Mat_World.size();
			vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			vbd.MiscFlags = 0;
			vbd.StructureByteStride = 0;
			D3D11_SUBRESOURCE_DATA vinitData;
			vinitData.pSysMem = &LocalStaticModelInfo[i].Mat_World[0];
			HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mFBXInstanceBuffer[i]));

			//绑带静态模型数据
			m_Models[i].SetModelTansInfo(&LocalStaticModelInfo[i]);
		}
	}
}


//每帧调用
void GameApp::BuildDynamicInstanceData()
{
	int Modelindex = 0;
	LocalDynamicModelInfo.clear();
	LocalDynamicModelInfo.resize(Model_Num);
	for (auto& VisitbleSence : VisibleModelData)
	{
		for (auto& ModelInfo : VisitbleSence.ModelInfo)
		{
			//判断是否为动态模型
			if (ModelInfo.second.Mat_tansform_Rot_Scal.size() != 0)
			{
				//更新world矩阵
				int size = ModelInfo.second.Mat_tansform_Rot_Scal.size();
				ModelInfo.second.Mat_World.resize(size);
				Modelindex = atoi(&ModelInfo.first) - 1;
				for (int j = 0; j < size; ++j)
				{
					ModelInfo.second.Mat_World[j] = ModelInfo.second.Mat_tansform_Rot_Scal[j]
						* ModelInfo.second.Mat_tansform_Translation[j];
					LocalDynamicModelInfo[Modelindex].Mat_World.push_back(ModelInfo.second.Mat_World[j]);
				}
			}
		}
	}

	//更新裁剪后的动态场景
	for (int i = 0; i < LocalDynamicModelInfo.size(); ++i)
	{
		if (LocalDynamicModelInfo[i].Mat_World.size() != 0)
		{
			//绑带动态模型数据
			m_Models[i].SetModelTansInfo(&LocalDynamicModelInfo[i]);
		}
	}
	//单独处理玩家数据
	{
		LocalDynamicModelInfo[7].Mat_World.resize(1);//玩家数量始终唯为一
		m_PlayerInfo[0]->Mat_World.clear();
		m_PlayerInfo[0]->Mat_World.push_back(m_PlayerInfo[0]->Mat_tansform_Rot_Scal[0]
			* m_PlayerInfo[0]->Mat_tansform_Translation[0]);
		m_Models[7].SetModelTansInfo(m_PlayerInfo[0]);
	}
}

void GameApp::BuildShadowMap()
{
	mShadowMap->BindDsvAndSetNullRenderTarget(md3dImmediateContext);
	md3dImmediateContext->OMSetDepthStencilState(0, 0);



	for (int index = 0; index < Model_Num; ++index)
	{
		if (LocalStaticModelInfo[index].Mat_World.size() != 0)//静态模型
		{
			if (index == 2)
			{
				for (int j = 0; j < LocalStaticModelInfo[index].Mat_World.size(); ++j)
				{
					if (!m_Models[index].ShadowRender(j, Anim_State::Idle, mShadowMap))
						return;
				}
			}
			else
			{
				if (!m_Models[index].ShadowRender(LocalStaticModelInfo[index].Mat_World.size(), mShadowMap, mFBXInstanceBuffer[index]))
					return;
			}
		}
		else if (LocalDynamicModelInfo[index].Mat_World.size() != 0)//动态模型
		{
			if (index == 7)
			{
				if (mAnim == Anim_State::Idle)
				{
					if (!m_Models[index].ShadowRender(0, Anim_State::Idle, mShadowMap))
						return;
				}
				else if (mAnim == Anim_State::Run)
				{
					if (!m_Models[index].ShadowRender(0, Anim_State::Run, mShadowMap))
						return;
				}
			}
			else
			{
				for (int j = 0; j < LocalDynamicModelInfo[index].Mat_World.size(); ++j)
				{
					if (!m_Models[index].ShadowRender(j, Anim_State::Idle, mShadowMap))
						return;
				}
			}
		}
	}
}

void GameApp::InitParticleSystem()
{
	mRandomTexSRV = d3dHelper::CreateRandomTexture1DSRV(md3dDevice);

	std::vector<std::wstring> flares;
	flares.push_back(L"Textures\\flare0.dds");
	mFireTexSRV = d3dHelper::CreateTexture2DArraySRV(md3dDevice, md3dImmediateContext, flares);
	//Fire Particle
	mFire.Init(md3dDevice, Effects::FireFX, mFireTexSRV, mRandomTexSRV, 50000);
	mFire.SetEmitPos(XMFLOAT3(0.0f, 50.0f, 100.0f));
	//Rain Particle
	std::vector<std::wstring> raindrops;
	raindrops.push_back(L"Textures\\raindrop.dds");
	mRainTexSRV = d3dHelper::CreateTexture2DArraySRV(md3dDevice, md3dImmediateContext, raindrops);

	mRain.Init(md3dDevice, Effects::RainFX, mRainTexSRV, mRandomTexSRV, 10000);
}

void GameApp::DrawParticle()
{
	// Draw particle systems last so it is blended with scene.
	//mFire.SetEyePos(mCamera->GetPosition());
	//mFire.Draw(md3dImmediateContext);

	mRain.SetEyePos(mCamera->GetPosition());
	mRain.SetEmitPos(mCamera->GetPosition());
	mRain.Draw(md3dImmediateContext);

}

void GameApp::CullSence()
{
	//视图矩阵的逆矩阵
	XMVECTOR detView = XMMatrixDeterminant(mCamera->View());
	XMMATRIX invView = XMMatrixInverse(&detView, mCamera->View());
	VisibleModelData.clear();
	VisibleTerrainData.clear();
	float OriginoffsetX = 80.0f;
	float OriginoffsetY = 80.0f;
	//进行地图块可视化检测
	for (UINT i = 0; i <TerrainSenceData.size(); ++i)
	{
		float offsetX = OriginoffsetX + TerrainSenceData[i].SenceOffset.x;
		float offsetY = OriginoffsetY + TerrainSenceData[i].SenceOffset.z;
		//世界矩阵的逆矩阵
		XMMATRIX W = XMMatrixTranslation(offsetX, 0.0f, offsetY);
		XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(W), W);
		// View space to the object's local space.
		XMMATRIX toLocal = XMMatrixMultiply(invView, invWorld);

		// Transform the camera frustum from view space to the object's local space.
		BoundingFrustum LocalFrustum;
		CameraFrustum.Transform(LocalFrustum, toLocal);

		// Perform the box/frustum intersection test in local space.
		if (LocalFrustum.Contains(CullingMapBox) != DirectX::DISJOINT)
		{
			// storage visible objects.
			
			VisibleModelData.push_back(ModelSenceData[i]);
			VisibleTerrainData.push_back(TerrainSenceData[i]);
		}
	}
}

void GameApp::DrawWater()
{
	//Refraction Map
	DrawWaterRefractionMap();

	//Reflection Map
	DrawWaterReflectionMap();

	//Draw water
	md3dImmediateContext->IASetInputLayout(InputLayouts::Water);
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	D3DX11_TECHNIQUE_DESC techDesc;
	Effects::WaterFX->WaterTech->GetDesc(&techDesc);
	UINT stride = sizeof(Vertex::PosNormalTexTan);
	UINT offset = 0;

	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		md3dImmediateContext->IASetVertexBuffers(0, 1, &mWaterTangetVB, &stride, &offset);
		md3dImmediateContext->IASetIndexBuffer(mLandIB, DXGI_FORMAT_R32_UINT, 0);
		// Set per object constants.
		XMMATRIX world = XMMatrixTranslation(0.0f, 0.0f, 0.0f) * XMLoadFloat4x4(&mLandWorld);
		XMMATRIX worldViewProj = world * mCamera->View() * mCamera->Proj();
		Effects::WaterFX->SetWorld(world);
		Effects::WaterFX->SetWorldViewProj(worldViewProj);
		Effects::WaterFX->SetEyePosW(mCamera->GetPosition());
		Effects::WaterFX->SetWaveParams(WaveParams);
		//change to single sample 
		ID3D11Resource* Dst = 0;
		ID3D11Resource* Src = 0;
		mWater->SingleReflectionSRV()->GetResource(&Dst);
		mWater->ReflectionSRV()->GetResource(&Src);
		md3dImmediateContext->ResolveSubresource(Dst,0, Src, 0,DXGI_FORMAT_R8G8B8A8_UNORM);
		ReleaseCOM(Dst);
		ReleaseCOM(Src);

		Effects::WaterFX->SetReflectionMap(mWater->SingleReflectionSRV());
		Effects::WaterFX->SetRefractionMap(mWater->SingleRefractionSRV());
		Effects::WaterFX->SetNormalMap(mWaterNormalSRV);
		Effects::WaterFX->WaterTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(mLandIndexCount, 0, 0);
	}

}

void GameApp::DrawWaterRefractionMap()
{
	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	md3dImmediateContext->RSSetState(0);
	md3dImmediateContext->OMSetDepthStencilState(0, 0);
	// Render refractive mesh (water) into alpha channel, as white to create the Refraction Mask
	md3dImmediateContext->OMSetBlendState(RenderStates::WriteAlphaBS, blendFactor, 0xffffffff);
	md3dImmediateContext->IASetInputLayout(InputLayouts::WaterMaskPos);
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	D3DX11_TECHNIQUE_DESC techDesc;
	Effects::WaterRefractionMaskFX->WaterTech->GetDesc(&techDesc);
	UINT stride = sizeof(Vertex::Basic32);
	UINT offset = 0;
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		md3dImmediateContext->IASetVertexBuffers(0, 1, &mLandVB, &stride, &offset);
		md3dImmediateContext->IASetIndexBuffer(mLandIB, DXGI_FORMAT_R32_UINT, 0);
		// Set per object constants.
		XMMATRIX world = XMMatrixTranslation(0.0f, 0.0f, 0.0f) * XMLoadFloat4x4(&mLandWorld);
		XMMATRIX worldViewProj = world * mCamera->View() * mCamera->Proj();
		Effects::WaterRefractionMaskFX->SetWorldViewProj(worldViewProj);
		Effects::WaterRefractionMaskFX->WaterTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(mLandIndexCount, 0, 0);
	}

	md3dImmediateContext->OMSetBlendState(0, blendFactor, 0xffffffff);
	// Copy backbuffer into refraction map texture
	ID3D11Resource* Dst = 0;
	ID3D11Resource* Src = 0;
	mWater->SingleRefractionSRV()->GetResource(&Dst);
	mRenderTargetView->GetResource(&Src);
	//HR(mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&Src)));
	md3dImmediateContext->ResolveSubresource(Dst, 0, Src, 0, DXGI_FORMAT_R8G8B8A8_UNORM);
	ReleaseCOM(Dst);
	ReleaseCOM(Src);
	//reset
	md3dImmediateContext->OMSetBlendState(0, blendFactor, 0xffffffff);
}

void GameApp::DrawWaterReflectionMap()
{
	ID3D11RenderTargetView* RenderTargetView[1] = { mWater->ReflectionRTV() };
	ID3D11DepthStencilView* DepthStencilView[1] = { mWater->ReflectionDepthMapDSV() };
	md3dImmediateContext->ClearRenderTargetView(*RenderTargetView, reinterpret_cast<const float*>(&GColors::White));
	md3dImmediateContext->ClearDepthStencilView(*DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	md3dImmediateContext->OMSetRenderTargets(1, RenderTargetView, *DepthStencilView);
	//md3dImmediateContext->RSSetViewports(1, &mScreenViewport);
	

	// Apply reflection on WorldViewProj matrix
	XMMATRIX View= mCamera->View();
	XMMATRIX Proj = mCamera->Proj();
	mCamera->SetView(XMMatrixScaling(1.0f, -1.0f, 1.0f) * View);

	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	//render sence
	{
		
		md3dImmediateContext->OMSetBlendState(m_pBlendState, blendFactor, 0xFFFFFFFF);
		//sky box render
		mSkyBox->Draw(md3dImmediateContext, *mCamera);
		//restore default states, as the SkyFX changes them in the effect file.
		md3dImmediateContext->RSSetState(0);
		md3dImmediateContext->OMSetDepthStencilState(0, 0);

		// Draw the terrain 
		md3dImmediateContext->RSSetState(RenderStates::CullBackRS);
		mTerrain.Render(md3dImmediateContext, gDirLights, mShadowMap,false);
		md3dImmediateContext->OMSetDepthStencilState(0, 0);

		//fbx model
		RenderFbxModel();

		//draw particle
		md3dImmediateContext->OMSetBlendState(m_pBlendState, blendFactor, 0xFFFFFFFF);
		DrawParticle();
	}

	//RESET States
	mCamera->SetView(View);
	md3dImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);
	md3dImmediateContext->RSSetState(0);
	md3dImmediateContext->OMSetDepthStencilState(RenderStates::LessEqualsDSS, 0);
	md3dImmediateContext->OMSetBlendState(0, blendFactor, 0xFFFFFFFF);
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
