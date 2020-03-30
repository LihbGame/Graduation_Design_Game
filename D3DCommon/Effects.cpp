#include "Effects.h"

#pragma region GEffect
GEffect::GEffect(ID3D11Device* device, const std::wstring& filename)
	: mFX(0)
{
	std::ifstream fin(filename, std::ios::binary);

	fin.seekg(0, std::ios_base::end);
	int size = (int)fin.tellg();
	fin.seekg(0, std::ios_base::beg);
	std::vector<char> compiledShader(size);

	fin.read(&compiledShader[0], size);
	fin.close();
	
	HR(D3DX11CreateEffectFromMemory(&compiledShader[0], size, 
		0, device, &mFX));
}

GEffect::~GEffect()
{
	ReleaseCOM(mFX);
}
#pragma endregion

#pragma region BasicEffect
BasicEffect::BasicEffect(ID3D11Device* device, const std::wstring& filename)
	: GEffect(device, filename)
{
	Light1Tech    = mFX->GetTechniqueByName("Light1");
	Light2Tech    = mFX->GetTechniqueByName("Light2");
	Light3Tech    = mFX->GetTechniqueByName("Light3");

	Light0TexTech = mFX->GetTechniqueByName("Light0Tex");
	Light1TexTech = mFX->GetTechniqueByName("Light1Tex");
	Light2TexTech = mFX->GetTechniqueByName("Light2Tex");
	Light3TexTech = mFX->GetTechniqueByName("Light3Tex");

	Light0TexAlphaClipTech = mFX->GetTechniqueByName("Light0TexAlphaClip");
	Light1TexAlphaClipTech = mFX->GetTechniqueByName("Light1TexAlphaClip");
	Light2TexAlphaClipTech = mFX->GetTechniqueByName("Light2TexAlphaClip");
	Light3TexAlphaClipTech = mFX->GetTechniqueByName("Light3TexAlphaClip");

	Light1FogTech    = mFX->GetTechniqueByName("Light1Fog");
	Light2FogTech    = mFX->GetTechniqueByName("Light2Fog");
	Light3FogTech    = mFX->GetTechniqueByName("Light3Fog");

	Light0TexFogTech = mFX->GetTechniqueByName("Light0TexFog");
	Light1TexFogTech = mFX->GetTechniqueByName("Light1TexFog");
	Light2TexFogTech = mFX->GetTechniqueByName("Light2TexFog");
	Light3TexFogTech = mFX->GetTechniqueByName("Light3TexFog");

	Light0TexAlphaClipFogTech = mFX->GetTechniqueByName("Light0TexAlphaClipFog");
	Light1TexAlphaClipFogTech = mFX->GetTechniqueByName("Light1TexAlphaClipFog");
	Light2TexAlphaClipFogTech = mFX->GetTechniqueByName("Light2TexAlphaClipFog");
	Light3TexAlphaClipFogTech = mFX->GetTechniqueByName("Light3TexAlphaClipFog");

	WorldViewProj     = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	World             = mFX->GetVariableByName("gWorld")->AsMatrix();
	WorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	TexTransform      = mFX->GetVariableByName("gTexTransform")->AsMatrix();
	EyePosW           = mFX->GetVariableByName("gEyePosW")->AsVector();
	FogColor          = mFX->GetVariableByName("gFogColor")->AsVector();
	FogStart          = mFX->GetVariableByName("gFogStart")->AsScalar();
	FogRange          = mFX->GetVariableByName("gFogRange")->AsScalar();
	DirLights         = mFX->GetVariableByName("gDirLights");
	Mat               = mFX->GetVariableByName("gMaterial");
	DiffuseMap        = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	Wave			  = mFX->GetVariableByName("gWave")->AsScalar();;
}

BasicEffect::~BasicEffect()
{
}
#pragma endregion


#pragma region SkyEffect
SkyEffect::SkyEffect(ID3D11Device* device, const std::wstring& filename)
	: GEffect(device, filename)
{
	SkyTech = mFX->GetTechniqueByName("SkyTech");
	WorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	CubeMap = mFX->GetVariableByName("gCubeMap")->AsShaderResource();
}

SkyEffect::~SkyEffect()
{
}
#pragma endregion


#pragma region InstancedBasicEffect
InstancedBasicEffect::InstancedBasicEffect(ID3D11Device* device, const std::wstring& filename)
	:GEffect(device, filename)
{
	Light1Tech = mFX->GetTechniqueByName("Light1");
	Light2Tech = mFX->GetTechniqueByName("Light2");
	Light3Tech = mFX->GetTechniqueByName("Light3");

	Light0TexTech = mFX->GetTechniqueByName("Light0Tex");
	Light1TexTech = mFX->GetTechniqueByName("Light1Tex");
	Light1TexClipTech= mFX->GetTechniqueByName("Light1TexClip");
	Light2TexTech = mFX->GetTechniqueByName("Light2Tex");
	Light3TexTech = mFX->GetTechniqueByName("Light3Tex");

	Light0TexAlphaClipTech = mFX->GetTechniqueByName("Light0TexAlphaClip");
	Light1TexAlphaClipTech = mFX->GetTechniqueByName("Light1TexAlphaClip");
	Light2TexAlphaClipTech = mFX->GetTechniqueByName("Light2TexAlphaClip");
	Light3TexAlphaClipTech = mFX->GetTechniqueByName("Light3TexAlphaClip");

	Light1FogTech = mFX->GetTechniqueByName("Light1Fog");
	Light2FogTech = mFX->GetTechniqueByName("Light2Fog");
	Light3FogTech = mFX->GetTechniqueByName("Light3Fog");

	Light0TexFogTech = mFX->GetTechniqueByName("Light0TexFog");
	Light1TexFogTech = mFX->GetTechniqueByName("Light1TexFog");
	Light2TexFogTech = mFX->GetTechniqueByName("Light2TexFog");
	Light3TexFogTech = mFX->GetTechniqueByName("Light3TexFog");

	Light0TexAlphaClipFogTech = mFX->GetTechniqueByName("Light0TexAlphaClipFog");
	Light1TexAlphaClipFogTech = mFX->GetTechniqueByName("Light1TexAlphaClipFog");
	Light2TexAlphaClipFogTech = mFX->GetTechniqueByName("Light2TexAlphaClipFog");
	Light3TexAlphaClipFogTech = mFX->GetTechniqueByName("Light3TexAlphaClipFog");

	ViewProj = mFX->GetVariableByName("gViewProj")->AsMatrix();
	World = mFX->GetVariableByName("gWorld")->AsMatrix();
	WorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	TexTransform = mFX->GetVariableByName("gTexTransform")->AsMatrix();
	ShadowTransform = mFX->GetVariableByName("gShadowTransform")->AsMatrix();
	EyePosW = mFX->GetVariableByName("gEyePosW")->AsVector();
	FogColor = mFX->GetVariableByName("gFogColor")->AsVector();
	FogStart = mFX->GetVariableByName("gFogStart")->AsScalar();
	FogRange = mFX->GetVariableByName("gFogRange")->AsScalar();
	DirLights = mFX->GetVariableByName("gDirLights");
	Mat = mFX->GetVariableByName("gMaterial");
	ShadowMap = mFX->GetVariableByName("gShadowMap")->AsShaderResource();

	//diffuse maps
	ID3DX11EffectVariable* DiffuseMaps = mFX->GetVariableByName("gDiffuseMap");
	for (int i = 0; i < 5; ++i)
	{
		DiffuseMap[i] =DiffuseMaps->GetElement(i)->AsShaderResource();
	}
	//normal maps
	ID3DX11EffectVariable* NormalMaps = mFX->GetVariableByName("gNormalMap");
	for (int i = 0; i < 5; ++i)
	{
		NormalMap[i] = NormalMaps->GetElement(i)->AsShaderResource();
	}

}

InstancedBasicEffect::~InstancedBasicEffect()
{
}
#pragma endregion


#pragma region ParticleEffect
ParticleEffect::ParticleEffect(ID3D11Device* device, const std::wstring& filename)
	: GEffect(device, filename)
{
	StreamOutTech = mFX->GetTechniqueByName("StreamOutTech");
	DrawTech = mFX->GetTechniqueByName("DrawTech");

	ViewProj = mFX->GetVariableByName("gViewProj")->AsMatrix();
	GameTime = mFX->GetVariableByName("gGameTime")->AsScalar();
	TimeStep = mFX->GetVariableByName("gTimeStep")->AsScalar();
	EyePosW = mFX->GetVariableByName("gEyePosW")->AsVector();
	EmitPosW = mFX->GetVariableByName("gEmitPosW")->AsVector();
	EmitDirW = mFX->GetVariableByName("gEmitDirW")->AsVector();
	TexArray = mFX->GetVariableByName("gTexArray")->AsShaderResource();
	RandomTex = mFX->GetVariableByName("gRandomTex")->AsShaderResource();
}

ParticleEffect::~ParticleEffect()
{
}
#pragma endregion


#pragma region WaterRefractionMaskEffect

WaterRefractionMaskEffect::WaterRefractionMaskEffect(ID3D11Device* device, const std::wstring& filename)
	: GEffect(device, filename)
{
	WaterTech=mFX->GetTechniqueByName("WaterTech");
	WorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
}

WaterRefractionMaskEffect::~WaterRefractionMaskEffect()
{
}
#pragma endregion

#pragma region WaterEffect

WaterEffect::WaterEffect(ID3D11Device* device, const std::wstring& filename)
	: GEffect(device, filename)
{
	WaterTech = mFX->GetTechniqueByName("Water");
	WorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	World = mFX->GetVariableByName("gWorld")->AsMatrix();
	EyePosW= mFX->GetVariableByName("gEyePosW")->AsVector();
	WaveParams = mFX->GetVariableByName("gWaveParams")->AsVector();
	NormalMap = mFX->GetVariableByName("gNormalMap")->AsShaderResource();
	ReflectionMap = mFX->GetVariableByName("gReflectionMap")->AsShaderResource();
	RefractionMap = mFX->GetVariableByName("gRefractionMap")->AsShaderResource();
}

WaterEffect::~WaterEffect()
{
}


#pragma endregion



#pragma region TerrainEffect
TerrainEffect::TerrainEffect(ID3D11Device* device, const std::wstring& filename)
	: GEffect(device, filename)
{
	Light1Tech = mFX->GetTechniqueByName("Light1");
	Light2Tech = mFX->GetTechniqueByName("Light2");
	Light3Tech = mFX->GetTechniqueByName("Light3");
	Light1FogTech = mFX->GetTechniqueByName("Light1Fog");
	Light2FogTech = mFX->GetTechniqueByName("Light2Fog");
	Light3FogTech = mFX->GetTechniqueByName("Light3Fog");

	ViewProj = mFX->GetVariableByName("gViewProj")->AsMatrix();
	EyePosW = mFX->GetVariableByName("gEyePosW")->AsVector();
	FogColor = mFX->GetVariableByName("gFogColor")->AsVector();
	FogStart = mFX->GetVariableByName("gFogStart")->AsScalar();
	FogRange = mFX->GetVariableByName("gFogRange")->AsScalar();
	DirLights = mFX->GetVariableByName("gDirLights");
	Mat = mFX->GetVariableByName("gMaterial");

	MinDist = mFX->GetVariableByName("gMinDist")->AsScalar();
	MaxDist = mFX->GetVariableByName("gMaxDist")->AsScalar();
	MinTess = mFX->GetVariableByName("gMinTess")->AsScalar();
	MaxTess = mFX->GetVariableByName("gMaxTess")->AsScalar();
	TexelCellSpaceU = mFX->GetVariableByName("gTexelCellSpaceU")->AsScalar();
	TexelCellSpaceV = mFX->GetVariableByName("gTexelCellSpaceV")->AsScalar();
	WorldCellSpace = mFX->GetVariableByName("gWorldCellSpace")->AsScalar();
	WorldFrustumPlanes = mFX->GetVariableByName("gWorldFrustumPlanes")->AsVector();

	LayerMapArray = mFX->GetVariableByName("gLayerMapArray")->AsShaderResource();
	BlendMap = mFX->GetVariableByName("gBlendMap")->AsShaderResource();
	HeightMap = mFX->GetVariableByName("gHeightMap")->AsShaderResource();
	GrassMap = mFX->GetVariableByName("gGrassMap")->AsShaderResource();
}

TerrainEffect::~TerrainEffect()
{
}
#pragma endregion



#pragma region Effects

BasicEffect* Effects::BasicFX = 0;
SkyEffect* Effects::SkyFX = 0;
InstancedBasicEffect* Effects::InstancedBasicFX = 0;
ParticleEffect* Effects::FireFX = 0;
ParticleEffect* Effects::RainFX = 0;
WaterRefractionMaskEffect* Effects::WaterRefractionMaskFX=0;
WaterEffect* Effects::WaterFX=0;
TerrainEffect* Effects::TerrainFX = 0;

void Effects::InitAll(ID3D11Device* device)
{
	BasicFX = new BasicEffect(device, L"shader/Basic.fxo");
	SkyFX = new SkyEffect(device, L"shader/Sky.fxo");
	InstancedBasicFX = new InstancedBasicEffect(device, L"shader/InstancedBasic.fxo");
	FireFX = new ParticleEffect(device, L"shader/Fire.fxo");
	RainFX = new ParticleEffect(device, L"shader/Rain.fxo");
	WaterRefractionMaskFX= new WaterRefractionMaskEffect(device, L"shader/WaterRefractionMask.fxo");
	WaterFX = new WaterEffect(device, L"shader/Water.fxo");
	TerrainFX = new TerrainEffect(device, L"shader/Terrain.fxo");
}

void Effects::DestroyAll()
{
	SafeDelete(BasicFX);
	SafeDelete(SkyFX);
	SafeDelete(InstancedBasicFX);
	SafeDelete(FireFX);
	SafeDelete(RainFX);
	SafeDelete(WaterRefractionMaskFX);
	SafeDelete(WaterFX);
	SafeDelete(TerrainFX);
}
#pragma endregion


