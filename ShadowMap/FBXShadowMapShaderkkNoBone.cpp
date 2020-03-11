#include "FBXShadowMapShaderkkNoBone.h"
#include "SoD3DShaderModelKK.h"
#include "d3dApp.h"
#include "Camera.h"
//----------------------------------------------------------------
#define ShadowKKNoBone_File "shader/ShadowKK_NoBone.fxo"
//----------------------------------------------------------------


FBXShadowMapShaderkkNoBone::FBXShadowMapShaderkkNoBone()
	:m_pInputLayout(0)
	, m_pFxEffect(0)
	, m_pFxTech(0)
	, m_pFxViewProjMatrix(0)
	, m_pFxVertexTextureWidth(0)
	, m_pFxVertexTexturePosCount(0)
	, m_pFxVertexTexture(0)
{
}

FBXShadowMapShaderkkNoBone::~FBXShadowMapShaderkkNoBone()
{
	ClearShadowShaderKKNoBone();
}

bool FBXShadowMapShaderkkNoBone::InitShadowShaderKKNoBone()
{
	if (D3DApp::Get() == 0)
	{
		return false;
	}
	ID3D11Device* pD3DDevice = D3DApp::Get()->GetD3DDevice();
	if (pD3DDevice == 0)
	{
		return false;
	}

	SoSimpleLoadFile kShaderFile;
	if (kShaderFile.LoadFile(ShadowKKNoBone_File, false) == false)
	{
		return false;
	}

	HRESULT hr = D3DX11CreateEffectFromMemory(kShaderFile.GetData(), kShaderFile.GetSize(), 0, pD3DDevice, &m_pFxEffect);
	if (FAILED(hr))
	{
		return false;
	}

	m_pFxTech = m_pFxEffect->GetTechniqueByName("ShaderTec");
	m_pFxViewProjMatrix = m_pFxEffect->GetVariableByName("g_matViewProj")->AsMatrix();
	//
	m_pFxVertexTextureWidth = m_pFxEffect->GetVariableByName("g_VertexTextureWidth")->AsScalar();
	m_pFxVertexTexturePosCount = m_pFxEffect->GetVariableByName("g_VertexTexturePosCount")->AsScalar();
	m_pFxVertexTextureNormalCount = m_pFxEffect->GetVariableByName("g_VertexTextureNormalCount")->AsScalar();
	m_pFxVertexTextureUVCount = m_pFxEffect->GetVariableByName("g_VertexTextureUVCount")->AsScalar();
	m_pFxVertexTextureTangentCount = m_pFxEffect->GetVariableByName("g_VertexTextureTangentCount")->AsScalar();
	//
	ID3DX11EffectVariable* pFxTextureList = m_pFxEffect->GetVariableByName("g_TextureList");
	m_pFxTexture = pFxTextureList->GetElement(0)->AsShaderResource();
	m_pFxVertexTexture = pFxTextureList->GetElement(1)->AsShaderResource();
	if (CreateInputLayout() == false)
	{
		return false;
	}

	return true;
}

void FBXShadowMapShaderkkNoBone::ClearShadowShaderKKNoBone()
{
	if (m_pInputLayout)
	{
		m_pInputLayout->Release();
		m_pInputLayout = 0;
	}
	m_pFxTech = 0;
	m_pFxViewProjMatrix = 0;
	m_pFxVertexTextureWidth = 0;
	m_pFxVertexTexturePosCount = 0;
	m_pFxVertexTexture = 0;
	if (m_pFxEffect)
	{
		m_pFxEffect->Release();
		m_pFxEffect = 0;
	}
}

void FBXShadowMapShaderkkNoBone::ProcessRender(void* pParam)
{
	stShaderModelKKParam* pModelParam = (stShaderModelKKParam*)pParam;
	ID3D11DeviceContext* pD3DDeviceContext = D3DApp::Get()->GetD3DDeviceContext();
	pD3DDeviceContext->IASetInputLayout(m_pInputLayout);
	pD3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//
	m_pFxViewProjMatrix->SetMatrix(reinterpret_cast<const float*>(&(pModelParam->mShadowMap->GetLightViewProj())));
	//
	m_pFxVertexTextureWidth->SetInt(pModelParam->nPosSRVWidth);
	m_pFxVertexTexturePosCount->SetInt(pModelParam->nPosCount);
	m_pFxVertexTextureNormalCount->SetInt(pModelParam->nNormalCount);
	m_pFxVertexTextureUVCount->SetInt(pModelParam->nUVCount);
	m_pFxVertexTextureTangentCount->SetInt(pModelParam->nTangentCount);
	m_pFxVertexTexture->SetResource(pModelParam->pPosSRV);
	m_pFxTexture->SetResource(pModelParam->pTextureSRV);
	//
	UINT stride[2] = { pModelParam->uiSizeofVertex, sizeof(XMFLOAT4X4) };
	UINT offset[2] = { 0,0 };
	ID3D11Buffer* vbs[2] = { pModelParam->pVB, pModelParam->pInstanceVB };

	D3DX11_TECHNIQUE_DESC techDesc;
	m_pFxTech->GetDesc(&techDesc);

	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		pD3DDeviceContext->IASetVertexBuffers(0, 2, vbs, stride, offset);
		pD3DDeviceContext->IASetIndexBuffer(pModelParam->pIB, DXGI_FORMAT_R32_UINT, 0);
		m_pFxTech->GetPassByIndex(p)->Apply(0, pD3DDeviceContext);
		pD3DDeviceContext->DrawIndexedInstanced(pModelParam->uiIndexCount, pModelParam->nInstanceCount, 0, 0, 0);
	}
}

bool FBXShadowMapShaderkkNoBone::CreateInputLayout()
{
	if (m_pFxTech == 0)
	{
		return false;
	}

	ID3D11Device* pD3DDevice = D3DApp::Get()->GetD3DDevice();
	HRESULT hr = S_OK;

	const int nDescCount = 5;
	D3D11_INPUT_ELEMENT_DESC kDesc[5] =
	{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{ "WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	{ "WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	{ "WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	{ "WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	};


	//
	D3DX11_PASS_DESC passDesc;
	m_pFxTech->GetPassByIndex(0)->GetDesc(&passDesc);
	hr = pD3DDevice->CreateInputLayout(kDesc, nDescCount, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &m_pInputLayout);
	return SUCCEEDED(hr);
}
