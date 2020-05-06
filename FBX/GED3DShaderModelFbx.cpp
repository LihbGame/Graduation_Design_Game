//----------------------------------------------------------------
#include "GED3DShaderModelFbx.h"
#include "d3dApp.h"
#include "Camera.h"
//----------------------------------------------------------------
#define ShaderModelFbx_File "shader/modelfbx.fxo"
//----------------------------------------------------------------
GED3DShaderModelFbx::GED3DShaderModelFbx()
:m_pInputLayout(0)
,m_pFxEffect(0)
,m_pFxTech(0)
,m_pFxWorldViewProj(0)
,m_pFxTexture1(0)
{

}
//----------------------------------------------------------------
GED3DShaderModelFbx::~GED3DShaderModelFbx()
{
	ClearShaderModelFbx();
}
//----------------------------------------------------------------
bool GED3DShaderModelFbx::InitShaderModelFbx()
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
	if (kShaderFile.LoadFile(ShaderModelFbx_File, false) == false)
	{
		return false;
	}

	HRESULT hr = D3DX11CreateEffectFromMemory(kShaderFile.GetData(), kShaderFile.GetSize(), 0, pD3DDevice, &m_pFxEffect);
	if (FAILED(hr))
	{
		return false;
	}

	m_pFxTech = m_pFxEffect->GetTechniqueByName("ShaderTec");
	m_pFxWorldViewProj = m_pFxEffect->GetVariableByName("g_matWorldViewProj")->AsMatrix();
	ID3DX11EffectVariable* pFxTexture1 = m_pFxEffect->GetVariableByName("g_Texture1");
	m_pFxTexture1 = pFxTexture1->GetElement(0)->AsShaderResource();

	if (CreateInputLayout() == false)
	{
		return false;
	}

	return true;
}
//----------------------------------------------------------------
void GED3DShaderModelFbx::ClearShaderModelFbx()
{
	if (m_pInputLayout)
	{
		m_pInputLayout->Release();
		m_pInputLayout = 0;
	}
	m_pFxTech = 0;
	m_pFxWorldViewProj = 0;
	m_pFxTexture1 = 0;
	if (m_pFxEffect)
	{
		m_pFxEffect->Release();
		m_pFxEffect = 0;
	}
}
//----------------------------------------------------------------
void GED3DShaderModelFbx::ProcessRender(void* pParam)
{
	stShaderModelFbxParam* pModelParam = (stShaderModelFbxParam*)pParam;
	ID3D11DeviceContext* pD3DDeviceContext = D3DApp::Get()->GetD3DDeviceContext();
	pD3DDeviceContext->IASetInputLayout(m_pInputLayout);
	pD3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pFxTexture1->SetResource(pModelParam->pSRV);

	UINT uiStride = pModelParam->uiSizeofVertexType;
	UINT uiOffset = 0;
	
	XMMATRIX worldViewProj = XMMatrixMultiply(*(pModelParam->pMatWorld), Camera::Get()->ViewProj());
	D3DX11_TECHNIQUE_DESC techDesc;
	m_pFxTech->GetDesc(&techDesc);

	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		pD3DDeviceContext->IASetVertexBuffers(0, 1, &(pModelParam->pVB), &uiStride, &uiOffset);
		pD3DDeviceContext->IASetIndexBuffer(pModelParam->pIB, DXGI_FORMAT_R32_UINT, 0);

		m_pFxWorldViewProj->SetMatrix(reinterpret_cast<const float*>(&worldViewProj));

		m_pFxTech->GetPassByIndex(p)->Apply(0, pD3DDeviceContext);
		pD3DDeviceContext->DrawIndexed(pModelParam->uiIndexCount, 0, 0);
	}
}
//----------------------------------------------------------------
bool GED3DShaderModelFbx::CreateInputLayout()
{
	if (m_pFxTech == 0)
	{
		return false;
	}

	ID3D11Device* pD3DDevice = D3DApp::Get()->GetD3DDevice();
	HRESULT hr = S_OK;

	const int nDescCount = 3;
	D3D11_INPUT_ELEMENT_DESC kDesc[3];
	kDesc[0].SemanticName = "POSITION";
	kDesc[0].SemanticIndex = 0;
	kDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	kDesc[0].InputSlot = 0;
	kDesc[0].AlignedByteOffset = 0;
	kDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	kDesc[0].InstanceDataStepRate = 0;
	//
	kDesc[1].SemanticName = "NORMAL";
	kDesc[1].SemanticIndex = 0;
	kDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	kDesc[1].InputSlot = 0;
	kDesc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	kDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	kDesc[1].InstanceDataStepRate = 0;
	//
	kDesc[2].SemanticName = "TEXCOORD";
	kDesc[2].SemanticIndex = 0;
	kDesc[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	kDesc[2].InputSlot = 0;
	kDesc[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	kDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	kDesc[2].InstanceDataStepRate = 0;

	D3DX11_PASS_DESC passDesc;
	m_pFxTech->GetPassByIndex(0)->GetDesc(&passDesc);
	hr = pD3DDevice->CreateInputLayout(kDesc, nDescCount, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &m_pInputLayout);
	return SUCCEEDED(hr);
}
//----------------------------------------------------------------
