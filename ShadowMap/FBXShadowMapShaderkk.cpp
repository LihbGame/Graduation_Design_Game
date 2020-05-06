#include "FBXShadowMapShaderkk.h"
#include "GED3DShaderModelKK.h"
#include "d3dApp.h"
#include "Camera.h"

//----------------------------------------------------------------
#define ShadowShaderKK_File "shader/ShadowKK.fxo"
//----------------------------------------------------------------


FBXShadowMapShaderkk::FBXShadowMapShaderkk()
	:m_pInputLayout(0)
	, m_pFxEffect(0)
	, m_pFxTech(0)
	, m_pFxWorldMatrix(0)
	, m_pFxViewProjMatrix(0)
	, m_pFxWorldInvTransposeMatrix(0)
	, m_pFxVertexTextureWidth(0)
	, m_pFxVertexTexturePosCount(0)
	, m_pFxAnimTextureWidth(0)
	, m_pFxAnimTextureBoneCount(0)
	, m_pFxAnimTextureKeyFrameIndex(0)
	, m_pFxVertexTexture(0)
	, m_pFxAnimTexture(0)
{
}

FBXShadowMapShaderkk::~FBXShadowMapShaderkk()
{
	ClearShadowShaderKK();
}

bool FBXShadowMapShaderkk::InitShadowShaderKK()
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
	if (kShaderFile.LoadFile(ShadowShaderKK_File, false) == false)
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
	m_pFxAnimTextureWidth = m_pFxEffect->GetVariableByName("g_AnimTextureWidth")->AsScalar();
	m_pFxAnimTextureBoneCount = m_pFxEffect->GetVariableByName("g_AnimTextureBoneCount")->AsScalar();
	m_pFxAnimTextureKeyFrameIndex = m_pFxEffect->GetVariableByName("g_AnimTextureKeyFrameIndex")->AsScalar();
	//
	ID3DX11EffectVariable* pFxTextureList = m_pFxEffect->GetVariableByName("g_TextureList");
	m_pFxVertexTexture = pFxTextureList->GetElement(1)->AsShaderResource();
	m_pFxAnimTexture = pFxTextureList->GetElement(2)->AsShaderResource();

	if (CreateInputLayout() == false)
	{
		return false;
	}
	return true;
}

void FBXShadowMapShaderkk::ClearShadowShaderKK()
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
	m_pFxAnimTextureWidth = 0;
	m_pFxAnimTextureBoneCount = 0;
	m_pFxAnimTextureKeyFrameIndex = 0;
	m_pFxVertexTexture = 0;
	m_pFxAnimTexture = 0;

	if (m_pFxEffect)
	{
		m_pFxEffect->Release();
		m_pFxEffect = 0;
	}

}

void FBXShadowMapShaderkk::ProcessRender(void* pParam)
{
	stShaderModelKKParam* pModelParam = (stShaderModelKKParam*)pParam;
	ID3D11DeviceContext* pD3DDeviceContext = D3DApp::Get()->GetD3DDeviceContext();
	pD3DDeviceContext->IASetInputLayout(m_pInputLayout);
	pD3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//
	//��������仯�����������ת�þ��󣬴��ݵ�shader�����ڼ��㷨�ߡ�
	XMMATRIX MatWorldInvTranspose = XMMatrixInverse(&XMMatrixDeterminant(*(pModelParam->pMatWorld)), *(pModelParam->pMatWorld));
	MatWorldInvTranspose = XMMatrixTranspose(MatWorldInvTranspose);
	m_pFxWorldMatrix->SetMatrix(reinterpret_cast<const float*>(pModelParam->pMatWorld));
	//��Ϊ���յ���ͼͶӰ����
	m_pFxViewProjMatrix->SetMatrix(reinterpret_cast<const float*>(&(pModelParam->mShadowMap->GetLightViewProj())));
	
	m_pFxWorldInvTransposeMatrix->SetMatrix(reinterpret_cast<const float*>(&MatWorldInvTranspose));
	//
	m_pFxVertexTextureWidth->SetInt(pModelParam->nPosSRVWidth);
	m_pFxVertexTexturePosCount->SetInt(pModelParam->nPosCount);
	m_pFxAnimTextureWidth->SetInt(pModelParam->nAnimSRVWidth);
	m_pFxAnimTextureBoneCount->SetInt(pModelParam->nBoneCount);
	m_pFxAnimTextureKeyFrameIndex->SetInt(pModelParam->nKeyFrameIndex);
	m_pFxVertexTexture->SetResource(pModelParam->pPosSRV);
	m_pFxAnimTexture->SetResource(pModelParam->pAnimSRV);


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

bool FBXShadowMapShaderkk::CreateInputLayout()
{
	if (m_pFxTech == 0)
	{
		return false;
	}

	ID3D11Device* pD3DDevice = D3DApp::Get()->GetD3DDevice();
	HRESULT hr = S_OK;

	const int nDescCount = 3;
	D3D11_INPUT_ELEMENT_DESC kDesc[3];
	//
	kDesc[0].SemanticName = "POSITION";
	kDesc[0].SemanticIndex = 0;
	kDesc[0].Format = DXGI_FORMAT_R32G32B32A32_UINT; //Ԫ�ص�ֵ��4��32λ����
	kDesc[0].InputSlot = 0;
	kDesc[0].AlignedByteOffset = 0;
	kDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	kDesc[0].InstanceDataStepRate = 0;
	//
	kDesc[1].SemanticName = "COLOR";
	kDesc[1].SemanticIndex = 0;
	kDesc[1].Format = DXGI_FORMAT_R32G32B32A32_UINT; //Ԫ�ص�ֵ��4��32λ����
	kDesc[1].InputSlot = 0;
	kDesc[1].AlignedByteOffset = 16;
	kDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	kDesc[1].InstanceDataStepRate = 0;
	//
	kDesc[2].SemanticName = "COLOR";
	kDesc[2].SemanticIndex = 1;
	kDesc[2].Format = DXGI_FORMAT_R32G32B32A32_FLOAT; //Ԫ�ص�ֵ��4��32λ������
	kDesc[2].InputSlot = 0;
	kDesc[2].AlignedByteOffset = 32;
	kDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	kDesc[2].InstanceDataStepRate = 0;
	//
	D3DX11_PASS_DESC passDesc;
	m_pFxTech->GetPassByIndex(0)->GetDesc(&passDesc);
	hr = pD3DDevice->CreateInputLayout(kDesc, nDescCount, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &m_pInputLayout);
	return SUCCEEDED(hr);
}
