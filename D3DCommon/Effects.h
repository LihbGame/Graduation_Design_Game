
#ifndef EFFECTS_H
#define EFFECTS_H

#include "d3dUtil.h"

#pragma region GEffect
class GEffect
{
public:
	GEffect(ID3D11Device* device, const std::wstring& filename);
	virtual ~GEffect();

private:
	GEffect(const GEffect& rhs);
	GEffect& operator=(const GEffect& rhs);

protected:
	ID3DX11Effect* mFX;
};
#pragma endregion

#pragma region WaterEffect

class WaterEffect :public GEffect
{
public:
	WaterEffect(ID3D11Device* device, const std::wstring& filename);
	~WaterEffect();

	void SetWorldViewProj(CXMMATRIX M) { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorld(CXMMATRIX M) { World->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetNormalMap(ID3D11ShaderResourceView* tex) { NormalMap->SetResource(tex); }
	void SetReflectionMap(ID3D11ShaderResourceView* tex) { ReflectionMap->SetResource(tex); }
	void SetRefractionMap(ID3D11ShaderResourceView* tex) { RefractionMap->SetResource(tex); }
	void SetEyePosW(const XMFLOAT3& v) { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
	void SetWaveParams(const XMFLOAT4& v) { WaveParams->SetRawValue(&v, 0, sizeof(XMFLOAT4)); }




	ID3DX11EffectTechnique* WaterTech;
	ID3DX11EffectMatrixVariable* WorldViewProj;
	ID3DX11EffectMatrixVariable* World;
	ID3DX11EffectVectorVariable* EyePosW;
	ID3DX11EffectVectorVariable* WaveParams;
	ID3DX11EffectShaderResourceVariable* NormalMap;
	ID3DX11EffectShaderResourceVariable* ReflectionMap;
	ID3DX11EffectShaderResourceVariable* RefractionMap;
};


#pragma endregion


#pragma region WaterRefractionMaskEffect
class WaterRefractionMaskEffect:public GEffect
{
public:
	WaterRefractionMaskEffect(ID3D11Device* device, const std::wstring& filename);
	~WaterRefractionMaskEffect();
	void SetWorldViewProj(CXMMATRIX M) { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }

	ID3DX11EffectTechnique* WaterTech;
	ID3DX11EffectMatrixVariable* WorldViewProj;
};

#pragma endregion

#pragma region BasicEffect
class BasicEffect : public GEffect
{
public:
	BasicEffect(ID3D11Device* device, const std::wstring& filename);
	~BasicEffect();

	void SetWorldViewProj(CXMMATRIX M)                  { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorld(CXMMATRIX M)                          { World->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldInvTranspose(CXMMATRIX M)              { WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetTexTransform(CXMMATRIX M)                   { TexTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetEyePosW(const XMFLOAT3& v)                  { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
	void SetFogColor(const FXMVECTOR v)                 { FogColor->SetFloatVector(reinterpret_cast<const float*>(&v)); }
	void SetFogStart(float f)                           { FogStart->SetFloat(f); }
	void SetFogRange(float f)                           { FogRange->SetFloat(f); }
	void SetDirLights(const DirectionalLight* lights)   { DirLights->SetRawValue(lights, 0, 3*sizeof(DirectionalLight)); }
	void SetMaterial(const Material& mat)               { Mat->SetRawValue(&mat, 0, sizeof(Material)); }
	void SetDiffuseMap(ID3D11ShaderResourceView* tex)   { DiffuseMap->SetResource(tex); }
	void SetWave(bool isWave)							{ Wave->SetBool(isWave); }

	ID3DX11EffectTechnique* Light1Tech;
	ID3DX11EffectTechnique* Light2Tech;
	ID3DX11EffectTechnique* Light3Tech;

	ID3DX11EffectTechnique* Light0TexTech;
	ID3DX11EffectTechnique* Light1TexTech;
	ID3DX11EffectTechnique* Light2TexTech;
	ID3DX11EffectTechnique* Light3TexTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipTech;

	ID3DX11EffectTechnique* Light1FogTech;
	ID3DX11EffectTechnique* Light2FogTech;
	ID3DX11EffectTechnique* Light3FogTech;

	ID3DX11EffectTechnique* Light0TexFogTech;
	ID3DX11EffectTechnique* Light1TexFogTech;
	ID3DX11EffectTechnique* Light2TexFogTech;
	ID3DX11EffectTechnique* Light3TexFogTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipFogTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipFogTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipFogTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipFogTech;

	ID3DX11EffectMatrixVariable* WorldViewProj;
	ID3DX11EffectMatrixVariable* World;
	ID3DX11EffectMatrixVariable* WorldInvTranspose;
	ID3DX11EffectMatrixVariable* TexTransform;
	ID3DX11EffectVectorVariable* EyePosW;
	ID3DX11EffectVectorVariable* FogColor;
	ID3DX11EffectScalarVariable* FogStart;
	ID3DX11EffectScalarVariable* FogRange;
	ID3DX11EffectVariable* DirLights;
	ID3DX11EffectVariable* Mat;
	ID3DX11EffectScalarVariable* Wave;

	ID3DX11EffectShaderResourceVariable* DiffuseMap;
};
#pragma endregion

//SkyEffect
#pragma region SkyEffect
class SkyEffect : public GEffect
{
public:
	SkyEffect(ID3D11Device* device, const std::wstring& filename);
	~SkyEffect();

	void SetWorldViewProj(CXMMATRIX M) { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetCubeMap(ID3D11ShaderResourceView* cubemap) { CubeMap->SetResource(cubemap); }

	ID3DX11EffectTechnique* SkyTech;

	ID3DX11EffectMatrixVariable* WorldViewProj;

	ID3DX11EffectShaderResourceVariable* CubeMap;
};
#pragma endregion



#pragma region InstancedBasicEffect
class InstancedBasicEffect : public GEffect
{
public:
	InstancedBasicEffect(ID3D11Device* device, const std::wstring& filename);
	~InstancedBasicEffect();

	void SetViewProj(CXMMATRIX M) { ViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorld(CXMMATRIX M) { World->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldInvTranspose(CXMMATRIX M) { WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetTexTransform(CXMMATRIX M) { TexTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetShadowTransform(CXMMATRIX M) { ShadowTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetEyePosW(const XMFLOAT3& v) { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
	void SetFogColor(const FXMVECTOR v) { FogColor->SetFloatVector(reinterpret_cast<const float*>(&v)); }
	void SetFogStart(float f) { FogStart->SetFloat(f); }
	void SetFogRange(float f) { FogRange->SetFloat(f); }
	void SetDirLights(const DirectionalLight& lights) { DirLights->SetRawValue(&lights, 0, sizeof(DirectionalLight)); }
	void SetMaterial(const Material& mat) { Mat->SetRawValue(&mat, 0, sizeof(Material)); }
	void SetShadowMap(ID3D11ShaderResourceView* tex) { ShadowMap->SetResource(tex); }
	//texture maps 5
	void SetDiffuseMap(ID3D11ShaderResourceView** tex) 
	{ 
		for (int i = 0; i < 5; ++i)
		{
			DiffuseMap[i]->SetResource(tex[i]);
		}
	}


	//normal maps 5
	void SetNormalMap(ID3D11ShaderResourceView** tex)
	{
		for (int i = 0; i < 5; ++i)
		{
			NormalMap[i]->SetResource(tex[i]);
		}
	}

	ID3DX11EffectTechnique* Light1Tech;
	ID3DX11EffectTechnique* Light2Tech;
	ID3DX11EffectTechnique* Light3Tech;

	ID3DX11EffectTechnique* Light0TexTech;
	ID3DX11EffectTechnique* Light1TexTech;
	ID3DX11EffectTechnique* Light1TexClipTech;
	ID3DX11EffectTechnique* Light2TexTech;
	ID3DX11EffectTechnique* Light3TexTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipTech;

	ID3DX11EffectTechnique* Light1FogTech;
	ID3DX11EffectTechnique* Light2FogTech;
	ID3DX11EffectTechnique* Light3FogTech;

	ID3DX11EffectTechnique* Light0TexFogTech;
	ID3DX11EffectTechnique* Light1TexFogTech;
	ID3DX11EffectTechnique* Light2TexFogTech;
	ID3DX11EffectTechnique* Light3TexFogTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipFogTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipFogTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipFogTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipFogTech;

	ID3DX11EffectMatrixVariable* ViewProj;
	ID3DX11EffectMatrixVariable* World;
	ID3DX11EffectMatrixVariable* WorldInvTranspose;
	ID3DX11EffectMatrixVariable* TexTransform;
	ID3DX11EffectMatrixVariable* ShadowTransform;
	ID3DX11EffectVectorVariable* EyePosW;
	ID3DX11EffectVectorVariable* FogColor;
	ID3DX11EffectScalarVariable* FogStart;
	ID3DX11EffectScalarVariable* FogRange;
	ID3DX11EffectVariable* DirLights;
	ID3DX11EffectVariable* Mat;

	ID3DX11EffectShaderResourceVariable* DiffuseMap[5];
	ID3DX11EffectShaderResourceVariable* NormalMap[5];
	ID3DX11EffectShaderResourceVariable* ShadowMap;
};
#pragma endregion



#pragma region ParticleEffect
class ParticleEffect : public GEffect
{
public:
	ParticleEffect(ID3D11Device* device, const std::wstring& filename);
	~ParticleEffect();

	void SetViewProj(CXMMATRIX M) { ViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }

	void SetGameTime(float f) { GameTime->SetFloat(f); }
	void SetTimeStep(float f) { TimeStep->SetFloat(f); }

	void SetEyePosW(const XMFLOAT3& v) { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
	void SetEmitPosW(const XMFLOAT3& v) { EmitPosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
	void SetEmitDirW(const XMFLOAT3& v) { EmitDirW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }

	void SetTexArray(ID3D11ShaderResourceView* tex) { TexArray->SetResource(tex); }
	void SetRandomTex(ID3D11ShaderResourceView* tex) { RandomTex->SetResource(tex); }

	ID3DX11EffectTechnique* StreamOutTech;
	ID3DX11EffectTechnique* DrawTech;

	ID3DX11EffectMatrixVariable* ViewProj;
	ID3DX11EffectScalarVariable* GameTime;
	ID3DX11EffectScalarVariable* TimeStep;
	ID3DX11EffectVectorVariable* EyePosW;
	ID3DX11EffectVectorVariable* EmitPosW;
	ID3DX11EffectVectorVariable* EmitDirW;
	ID3DX11EffectShaderResourceVariable* TexArray;
	ID3DX11EffectShaderResourceVariable* RandomTex;
};
#pragma endregion


#pragma region TerrainEffect
class TerrainEffect : public GEffect
{
public:
	TerrainEffect(ID3D11Device* device, const std::wstring& filename);
	~TerrainEffect();

	void SetViewProj(CXMMATRIX M) { ViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetEyePosW(const XMFLOAT3& v) { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
	void SetFogColor(const FXMVECTOR v) { FogColor->SetFloatVector(reinterpret_cast<const float*>(&v)); }
	void SetFogStart(float f) { FogStart->SetFloat(f); }
	void SetFogRange(float f) { FogRange->SetFloat(f); }
	void SetDirLights(const DirectionalLight* lights) { DirLights->SetRawValue(lights, 0, 3 * sizeof(DirectionalLight)); }
	void SetMaterial(const Material& mat) { Mat->SetRawValue(&mat, 0, sizeof(Material)); }

	void SetMinDist(float f) { MinDist->SetFloat(f); }
	void SetMaxDist(float f) { MaxDist->SetFloat(f); }
	void SetMinTess(float f) { MinTess->SetFloat(f); }
	void SetMaxTess(float f) { MaxTess->SetFloat(f); }
	void SetTexelCellSpaceU(float f) { TexelCellSpaceU->SetFloat(f); }
	void SetTexelCellSpaceV(float f) { TexelCellSpaceV->SetFloat(f); }
	void SetWorldCellSpace(float f) { WorldCellSpace->SetFloat(f); }
	void SetWorldFrustumPlanes(XMFLOAT4 planes[6]) { WorldFrustumPlanes->SetFloatVectorArray(reinterpret_cast<float*>(planes), 0, 6); }

	void SetLayerMapArray(ID3D11ShaderResourceView* tex) { LayerMapArray->SetResource(tex); }
	void SetBlendMap(ID3D11ShaderResourceView* tex) { BlendMap->SetResource(tex); }
	void SetHeightMap(ID3D11ShaderResourceView* tex) { HeightMap->SetResource(tex); }
	void SetGrassMap(ID3D11ShaderResourceView* tex) { GrassMap->SetResource(tex); }

	ID3DX11EffectTechnique* Light1Tech;
	ID3DX11EffectTechnique* Light2Tech;
	ID3DX11EffectTechnique* Light3Tech;
	ID3DX11EffectTechnique* Light1FogTech;
	ID3DX11EffectTechnique* Light2FogTech;
	ID3DX11EffectTechnique* Light3FogTech;

	ID3DX11EffectMatrixVariable* ViewProj;
	ID3DX11EffectMatrixVariable* World;
	ID3DX11EffectMatrixVariable* WorldInvTranspose;
	ID3DX11EffectMatrixVariable* TexTransform;
	ID3DX11EffectVectorVariable* EyePosW;
	ID3DX11EffectVectorVariable* FogColor;
	ID3DX11EffectScalarVariable* FogStart;
	ID3DX11EffectScalarVariable* FogRange;
	ID3DX11EffectVariable* DirLights;
	ID3DX11EffectVariable* Mat;
	ID3DX11EffectScalarVariable* MinDist;
	ID3DX11EffectScalarVariable* MaxDist;
	ID3DX11EffectScalarVariable* MinTess;
	ID3DX11EffectScalarVariable* MaxTess;
	ID3DX11EffectScalarVariable* TexelCellSpaceU;
	ID3DX11EffectScalarVariable* TexelCellSpaceV;
	ID3DX11EffectScalarVariable* WorldCellSpace;
	ID3DX11EffectVectorVariable* WorldFrustumPlanes;

	ID3DX11EffectShaderResourceVariable* LayerMapArray;
	ID3DX11EffectShaderResourceVariable* BlendMap;
	ID3DX11EffectShaderResourceVariable* HeightMap;
	ID3DX11EffectShaderResourceVariable* GrassMap;
};
#pragma endregion


#pragma region Effects
class Effects
{
public:
	static void InitAll(ID3D11Device* device);
	static void DestroyAll();

	//基础shader
	static BasicEffect* BasicFX;
	//天空盒shader
	static SkyEffect* SkyFX;
	//实例化渲染shader
	static InstancedBasicEffect* InstancedBasicFX;
	//Particle Effect
	static ParticleEffect* FireFX;
	static ParticleEffect* RainFX;
	//Water Refraction Mask Effect 
	static WaterRefractionMaskEffect* WaterRefractionMaskFX;
	//water effect
	static WaterEffect* WaterFX;
	//Terrain Effect
	static TerrainEffect* TerrainFX;
};
#pragma endregion

#endif //EFFECTS_H