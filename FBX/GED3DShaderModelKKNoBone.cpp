//----------------------------------------------------------------
#include "GED3DShaderModelKKNoBone.h"
#include "GED3DShaderModelKK.h"
#include "d3dApp.h"
#include "Camera.h"
//----------------------------------------------------------------
#define ShaderModelKKNoBone_File "shader/ModelKK_NoBone.fxo"
//----------------------------------------------------------------
extern bool ghaveTangent;

GED3DShaderModelKKNoBone::GED3DShaderModelKKNoBone()
:m_pInputLayout(0)
,m_pFxEffect(0)
,m_pFxTech(0)
,m_pFxWorldMatrix(0)
,m_pFxViewProjMatrix(0)
,m_pFxWorldInvTransposeMatrix(0)
,m_pFxVertexTextureWidth(0)
,m_pFxVertexTexturePosCount(0)
,m_pFxVertexTextureNormalCount(0)
,m_pFxVertexTextureUVCount(0)
,m_pFxTexture(0)
,m_pFxVertexTexture(0)
{

}
//----------------------------------------------------------------
GED3DShaderModelKKNoBone::~GED3DShaderModelKKNoBone()
{
	ClearShaderModelKKNoBone();
}
//----------------------------------------------------------------
bool GED3DShaderModelKKNoBone::InitShaderModelKKNoBone()
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

	GESimpleLoadFile kShaderFile;
	if (kShaderFile.LoadFile(ShaderModelKKNoBone_File, false) == false)
	{
		return false;
	}

	HRESULT hr = D3DX11CreateEffectFromMemory(kShaderFile.GetData(), kShaderFile.GetSize(), 0, pD3DDevice, &m_pFxEffect);
	if (FAILED(hr))
	{
		return false;
	}

	m_pFxTech = m_pFxEffect->GetTechniqueByName("ShaderTec");
	m_pFxWorldMatrix = m_pFxEffect->GetVariableByName("g_matWorld")->AsMatrix();
	m_pFxViewProjMatrix = m_pFxEffect->GetVariableByName("g_matViewProj")->AsMatrix();
	m_pFxWorldInvTransposeMatrix = m_pFxEffect->GetVariableByName("g_matWorldInvTranspose")->AsMatrix();
	//
	m_pFxVertexTextureWidth = m_pFxEffect->GetVariableByName("g_VertexTextureWidth")->AsScalar();
	m_pFxVertexTexturePosCount = m_pFxEffect->GetVariableByName("g_VertexTexturePosCount")->AsScalar();
	m_pFxVertexTextureNormalCount = m_pFxEffect->GetVariableByName("g_VertexTextureNormalCount")->AsScalar();
	m_pFxVertexTextureUVCount = m_pFxEffect->GetVariableByName("g_VertexTextureUVCount")->AsScalar();
	m_pFxVertexTextureTangentCount = m_pFxEffect->GetVariableByName("g_VertexTextureTangentCount")->AsScalar();
	m_pFxhaveTangent= m_pFxEffect->GetVariableByName("IshaveTangent")->AsScalar();
	EyePosW = m_pFxEffect->GetVariableByName("gEyePosW")->AsVector();
	//
	ID3DX11EffectVariable* pFxTextureList = m_pFxEffect->GetVariableByName("g_TextureList");
	m_pFxTexture = pFxTextureList->GetElement(0)->AsShaderResource();
	m_pFxVertexTexture = pFxTextureList->GetElement(1)->AsShaderResource();
	m_pFxNormalTexture = m_pFxEffect->GetVariableByName("g_NormalTexture")->AsShaderResource();
	if (CreateInputLayout() == false)
	{
		return false;
	}

	return true;
}
//----------------------------------------------------------------
void GED3DShaderModelKKNoBone::ClearShaderModelKKNoBone()
{
	if (m_pInputLayout)
	{
		m_pInputLayout->Release();
		m_pInputLayout = 0;
	}
	m_pFxTech = 0;
	m_pFxWorldMatrix = 0;
	m_pFxViewProjMatrix = 0;
	m_pFxWorldInvTransposeMatrix = 0;
	m_pFxVertexTextureWidth = 0;
	m_pFxVertexTexturePosCount = 0;
	m_pFxVertexTextureNormalCount = 0;
	m_pFxVertexTextureUVCount = 0;
	m_pFxTexture = 0;
	m_pFxVertexTexture = 0;
	if (m_pFxEffect)
	{
		m_pFxEffect->Release();
		m_pFxEffect = 0;
	}
}
//----------------------------------------------------------------
void GED3DShaderModelKKNoBone::ProcessRender(void* pParam)
{
	stShaderModelKKParam* pModelParam = (stShaderModelKKParam*)pParam;
	ID3D11DeviceContext* pD3DDeviceContext = D3DApp::Get()->GetD3DDeviceContext();
	pD3DDeviceContext->IASetInputLayout(m_pInputLayout);
	pD3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//
	//计算世界变化矩阵的逆矩阵的转置矩阵，传递到shader中用于计算法线。
	
	XMMATRIX MatWorldInvTranspose = XMMatrixInverse(&XMMatrixDeterminant(*(pModelParam->pMatWorld)), *(pModelParam->pMatWorld));
	MatWorldInvTranspose = XMMatrixTranspose(MatWorldInvTranspose);
	m_pFxWorldMatrix->SetMatrix(reinterpret_cast<const float*>(pModelParam->pMatWorld));
	m_pFxViewProjMatrix->SetMatrix(reinterpret_cast<const float*>(&(Camera::Get()->ViewProj())));
	m_pFxWorldInvTransposeMatrix->SetMatrix(reinterpret_cast<const float*>(&MatWorldInvTranspose));
	//
	m_pFxVertexTextureWidth->SetInt(pModelParam->nPosSRVWidth);
	m_pFxVertexTexturePosCount->SetInt(pModelParam->nPosCount);
	m_pFxVertexTextureNormalCount->SetInt(pModelParam->nNormalCount);
	m_pFxVertexTextureUVCount->SetInt(pModelParam->nUVCount);
	m_pFxhaveTangent->SetBool(ghaveTangent);
	m_pFxVertexTextureTangentCount->SetInt(pModelParam->nTangentCount);
	m_pFxTexture->SetResource(pModelParam->pTextureSRV);
	m_pFxVertexTexture->SetResource(pModelParam->pPosSRV);
	if (ghaveTangent)
	{
		m_pFxNormalTexture->SetResource(pModelParam->pNormalTextureSRV);
	}
	EyePosW->SetRawValue(&Camera::Get()->GetPosition(), 0, sizeof(XMFLOAT3));
	//
	UINT uiStride = pModelParam->uiSizeofVertex;
	UINT uiOffset = 0;
	D3DX11_TECHNIQUE_DESC techDesc;
	m_pFxTech->GetDesc(&techDesc);

	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		pD3DDeviceContext->IASetVertexBuffers(0, 1, &(pModelParam->pVB), &uiStride, &uiOffset);
		pD3DDeviceContext->IASetIndexBuffer(pModelParam->pIB, DXGI_FORMAT_R32_UINT, 0);
		m_pFxTech->GetPassByIndex(p)->Apply(0, pD3DDeviceContext);
		pD3DDeviceContext->DrawIndexed(pModelParam->uiIndexCount, 0, 0);
	}
}
//----------------------------------------------------------------
bool GED3DShaderModelKKNoBone::CreateInputLayout()
{
	if (m_pFxTech == 0)
	{
		return false;
	}

	ID3D11Device* pD3DDevice = D3DApp::Get()->GetD3DDevice();
	HRESULT hr = S_OK;

	const int nDescCount = 1;
	D3D11_INPUT_ELEMENT_DESC kDesc[1];
	//
	kDesc[0].SemanticName = "POSITION";
	kDesc[0].SemanticIndex = 0;
	kDesc[0].Format = DXGI_FORMAT_R32G32B32A32_UINT; //元素的值是4个32位整型
	kDesc[0].InputSlot = 0;
	kDesc[0].AlignedByteOffset = 0;
	kDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	kDesc[0].InstanceDataStepRate = 0;

	//
	D3DX11_PASS_DESC passDesc;
	m_pFxTech->GetPassByIndex(0)->GetDesc(&passDesc);
	hr = pD3DDevice->CreateInputLayout(kDesc, nDescCount, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &m_pInputLayout);
	return SUCCEEDED(hr);
}
//----------------------------------------------------------------
