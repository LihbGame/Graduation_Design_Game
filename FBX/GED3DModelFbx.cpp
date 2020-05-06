//----------------------------------------------------------------
#include "GED3DModelFbx.h"
#include "d3dApp.h"
#include "GED3DShaderManager.h"
#include "GED3DTexture.h"
#include "GED3DTextureManager.h"
#include "StFBXModel.h"
//----------------------------------------------------------------
GED3DModelFbx::GED3DModelFbx()
:m_pVertexBuffer(0)
,m_pIndexBuffer(0)
,m_pShader(0)
,m_pTexture(0)
,m_nVertexCount(0)
,m_nIndexCount(0)
,m_fAccTime(0.0f)
{

}
//----------------------------------------------------------------
GED3DModelFbx::~GED3DModelFbx()
{

}
//----------------------------------------------------------------
bool GED3DModelFbx::InitModel(void* pInitParam)
{
	GED3DModelInitParam_Fbx* pFbxParam = (GED3DModelInitParam_Fbx*)pInitParam;
	bool bSuccess = false;
	do 
	{
		if (CreateVertexBuffer(pFbxParam->pMeshData) == false)
		{
			break;
		}
		if (CreateIndexBuffer(pFbxParam->pMeshData) == false)
		{
			break;
		}
		if (CreateTexture(NULL) == false)
		{
			break;
		}
		m_pShader = GED3DShaderManager::Get()->GetShader(GED3DShaderType_ModelFbx);
		if (m_pShader == 0)
		{
			break;
		}
		XMStoreFloat4x4(&m_matWorld, XMMatrixIdentity());
		//
		bSuccess = true;
	} while(0);

	if (bSuccess == false)
	{
		ClearModel();
	}
	return bSuccess;
}
//----------------------------------------------------------------
void GED3DModelFbx::ClearModel()
{
	ReleaseVertexBuffer();
	ReleaseIndexBuffer();
	ReleaseTexture();
	m_pShader = 0;
	m_nVertexCount = 0;
	m_nIndexCount = 0;
}
//----------------------------------------------------------------
void GED3DModelFbx::UpdateModel(float fDeltaTime, StFBXModel* pAnimatedModel)
{
	if (pAnimatedModel->GetKeyFrameCount() > 0)
	{
		m_fAccTime += fDeltaTime;
		const StFBXMeshData* pMeshData = pAnimatedModel->GetAnimationMeshData(m_fAccTime);

		ID3D11DeviceContext* pD3DDeviceContext = D3DApp::Get()->GetD3DDeviceContext();
		////<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		//�Ѷ������ݴ�CPU�ڴ濽����GPU�ڴ���
		D3D11_MAPPED_SUBRESOURCE kMappedResource;
		memset(&kMappedResource, 0, sizeof(kMappedResource));
		HRESULT hr = pD3DDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &kMappedResource);
		if (SUCCEEDED(hr))
		{
			memcpy(kMappedResource.pData, pMeshData->pVertexBuff, pMeshData->nVertexBuffSize);
			pD3DDeviceContext->Unmap(m_pVertexBuffer, 0);
		}
		//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	}
}
//----------------------------------------------------------------
void GED3DModelFbx::RenderModel(Model_Tansform_Info *Mode_info,int Instance_Num)
{
	XMMATRIX kWorld = Mode_info->Mat_tansform_Rot_Scal[Instance_Num] * XMLoadFloat4x4(&m_matWorld) * Mode_info->Mat_tansform_Translation[Instance_Num];
	//
	stShaderModelFbxParam kParam;
	kParam.pVB = m_pVertexBuffer;
	kParam.pIB = m_pIndexBuffer;
	kParam.pSRV = m_pTexture->GetTextureSRV();
	kParam.pMatWorld = &kWorld;
	kParam.uiIndexCount = m_nIndexCount;
	kParam.uiSizeofVertexType = sizeof(stVertexType);
	m_pShader->ProcessRender(&kParam);
}
//----------------------------------------------------------------
void GED3DModelFbx::SetWorldMatrix(const XMFLOAT4X4* pMatWorld)
{
	m_matWorld = *pMatWorld;
}
//----------------------------------------------------------------
void GED3DModelFbx::SetTexture(const wchar_t* szFileName)
{
	CreateTexture(szFileName);
}
//----------------------------------------------------------------
bool GED3DModelFbx::CreateVertexBuffer(const StFBXMeshData* pMeshData)
{
	if (pMeshData->pVertexBuff == 0 || pMeshData->nVertexCount == 0)
	{
		return false;
	}

	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(stVertexType) * pMeshData->nVertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = pMeshData->pVertexBuff;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	ID3D11Device* pD3DDevice = D3DApp::Get()->GetD3DDevice();
	HRESULT hr = pD3DDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &m_pVertexBuffer);

	m_nVertexCount = pMeshData->nVertexCount;
	return SUCCEEDED(hr);
}
//----------------------------------------------------------------
void GED3DModelFbx::ReleaseVertexBuffer()
{
	if (m_pVertexBuffer)
	{
		m_pVertexBuffer->Release();
		m_pVertexBuffer = 0;
	}
}
//----------------------------------------------------------------
bool GED3DModelFbx::CreateIndexBuffer(const StFBXMeshData* pMeshData)
{
	if (pMeshData->pIndexBuff == 0 || pMeshData->nIndexCount == 0)
	{
		return false;
	}

	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned int) * pMeshData->nIndexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = pMeshData->pIndexBuff;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	ID3D11Device* pD3DDevice = D3DApp::Get()->GetD3DDevice();
	HRESULT hr = pD3DDevice->CreateBuffer(&indexBufferDesc, &indexData, &m_pIndexBuffer);

	m_nIndexCount = pMeshData->nIndexCount;
	return SUCCEEDED(hr);
}
//----------------------------------------------------------------
void GED3DModelFbx::ReleaseIndexBuffer()
{
	if (m_pIndexBuffer)
	{
		m_pIndexBuffer->Release();
		m_pIndexBuffer = 0;
	}
}
//----------------------------------------------------------------
bool GED3DModelFbx::CreateTexture(const wchar_t* szFileName)
{
	ReleaseTexture();
	//
	const wchar_t* szRealName = szFileName;
	if (szRealName == 0 || szRealName[0] == 0)
	{
		szRealName = L"texture/defaultTex.dds";
	}
	//
	m_pTexture = GED3DTextureManager::Get()->CreateTextureFromFile(szRealName);
	if (m_pTexture)
	{
		m_pTexture->AddRefTexture();
	}
	return (m_pTexture != NULL);
}
//----------------------------------------------------------------
void GED3DModelFbx::ReleaseTexture()
{
	if (m_pTexture)
	{
		m_pTexture->RemoveRefTexture();
		m_pTexture = NULL;
	}
}
//----------------------------------------------------------------
