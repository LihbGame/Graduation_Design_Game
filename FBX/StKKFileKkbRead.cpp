//----------------------------------------------------------------
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif //_CRT_SECURE_NO_WARNINGS
//----------------------------------------------------------------
#include "StKKFileKkbRead.h"
#include "StFBXDefine.h"
#include "d3dApp.h"
#include "SoD3DTextureManager.h"
#include "SoSimpleLog.h"
//----------------------------------------------------------------
bool StKKFileKkbRead::ReadKkb(const char* szFileName, StKKModelData* pModelData)
{
	if (szFileName == 0 || szFileName[0] == 0)
	{
		return false;
	}
	if (pModelData == 0)
	{
		return false;
	}

	StKkbFileHead* pFileHead = 0;
	char* pVertexStructBuff = 0;
	char* pTextureData = 0;

	ID3D11Buffer* theVertexStructBuffer = 0;
	ID3D11Buffer* theIndexBuffer = 0;
	SoD3DTexture* theVertexValueTexture = 0;
	int nTextureWidth = 0;
	int nTextureHeight = 0;

	SoLogDebug("StKKFileKkbRead::ReadKkb : Start; FileName[%s]", szFileName);

	bool br = false;
	do 
	{
		if (LoadFileData(szFileName, &pFileHead, &pVertexStructBuff, &pTextureData, &nTextureWidth, &nTextureHeight) == false)
		{
			break;
		}

		if (CreateVertexStructBuff(pFileHead, pVertexStructBuff, &theVertexStructBuffer) == false)
		{
			break;
		}

		if (CreateIndexBuff(pFileHead, &theIndexBuffer) == false)
		{
			break;
		}

		if (CreateVertexValueTexture(nTextureWidth, nTextureHeight, pTextureData, &theVertexValueTexture) == false)
		{
			break;
		}

		br = true;

	} while (0);


	if (br == true)
	{
		pModelData->pVertexStructBuffer = theVertexStructBuffer;
		pModelData->pIndexBuffer = theIndexBuffer;
		pModelData->pVertexValueTexture = theVertexValueTexture;
		pModelData->nVertexType = pFileHead->VertexType;
		pModelData->nVertexCount = pFileHead->VertexCount;
		pModelData->nIndexCount = pFileHead->VertexCount;
		pModelData->nSizeofVertexStruct = pFileHead->VertexSize;
		pModelData->nTextureWidth = nTextureWidth;
		pModelData->nTextureHeight = nTextureHeight;
		pModelData->nPosCount = pFileHead->PosCount;
		pModelData->nNormalCount = pFileHead->NormalCount;
		pModelData->nUVCount = pFileHead->UVCount;
		pModelData->nTangentCount = pFileHead->TangentCount;
		CalculateMeshBoundingBox(pTextureData, pFileHead->PosCount, &(pModelData->kPosMinValue), &(pModelData->kPosMaxValue));
	}
	else
	{
		if (theVertexStructBuffer)
		{
			theVertexStructBuffer->Release();
			theVertexStructBuffer = 0;
		}
		if (theIndexBuffer)
		{
			theIndexBuffer->Release();
			theIndexBuffer = 0;
		}
		if (theVertexValueTexture)
		{
			theVertexValueTexture->RemoveRefTexture();
			theVertexValueTexture = 0;
		}
	}

	//���ܲ����Ƿ�ɹ�����Ҫɾ��������ڴ档
	if (pFileHead)
	{
		free(pFileHead);
		pFileHead = 0;
	}
	if (pVertexStructBuff)
	{
		free(pVertexStructBuff);
		pVertexStructBuff = 0;
	}
	if (pTextureData)
	{
		free(pTextureData);
		pTextureData = 0;
	}

	SoLogDebug("StKKFileKkbRead::ReadKkb : Finish; Success[%d]", br?1:0);
	return br;
}
//----------------------------------------------------------------
bool StKKFileKkbRead::LoadFileData(const char* szFileName, StKkbFileHead** ppFileHead, char** ppVertexStructBuff, char** ppTextureData, int* pTextureWidth, int* pTextureHeight)
{
	FILE* fp = fopen(szFileName, "rb");
	if (fp == 0)
	{
		SoLogError("StKKFileKkbRead::LoadFileData : fopen fail! [%s]", szFileName);
		return false;
	}

	StKkbFileHead* pFileHead = 0;
	char* pVertexStructBuff = 0;
	char* pTextureData = 0;
	int nWidth = 0;
	int nHeight = 0;

	bool br = false;
	do 
	{
		//��ȡ�ļ�ͷ��
		const unsigned int SizeofFileHead = sizeof(StKkbFileHead);
		pFileHead = (StKkbFileHead*)malloc(SizeofFileHead);
		if (pFileHead == 0)
		{
			SoLogError("StKKFileKkbRead::LoadFileData : malloc fail! size[%d]", SizeofFileHead);
			break;
		}

		unsigned int ReadCount = fread(pFileHead, 1, SizeofFileHead, fp);
		if (ReadCount != SizeofFileHead)
		{
			SoLogError("StKKFileKkbRead::LoadFileData : fread FileHead fail!");
			break;
		}

		//�ж��ļ���ǡ�
		bool bFileFlagOK = true;
		for (int i = 0; i < StKkbFileFlagSize; ++i)
		{
			if (pFileHead->FileFlag[i] != StKkbFileFlagString[i])
			{
				bFileFlagOK = false;
				break;
			}
		}
		if (bFileFlagOK == false)
		{
			SoLogError("StKKFileKkbRead::LoadFileData : FileFlag check fail!");
			break;
		}

		//��ȡ����ṹ�����ݡ�
		const unsigned int VertexStructBuffSize = pFileHead->VertexSize * pFileHead->VertexCount;
		pVertexStructBuff = (char*)malloc(VertexStructBuffSize);
		if (pVertexStructBuff == 0)
		{
			SoLogError("StKKFileKkbRead::LoadFileData : malloc fail! size[%d]", VertexStructBuffSize);
			break;
		}

		ReadCount = fread(pVertexStructBuff, 1, VertexStructBuffSize, fp);
		if (ReadCount != VertexStructBuffSize)
		{
			SoLogError("StKKFileKkbRead::LoadFileData : fread VertexStructBuff fail!");
			break;
		}

		//��ȡ����Ԫ��ֵ����ͼ���ݡ�
		//Ϊ�˴�����ͼ�������ĩβ����������С�
		//����ͼ�ڻ��ȴ洢Pos���ݣ�float3������ΪpFileHead->PosCount������
		//Ȼ��洢Normal���ݣ�float3������ΪpFileHead->NormalCount������
		//tangent data
		//���洢UV���ݣ�float2������ΪpFileHead->UVCount������
		const unsigned int TextureDataSize = StFBX_Sizeof_Vector3 * (pFileHead->PosCount + pFileHead->NormalCount+ pFileHead->TangentCount) + StFBX_Sizeof_UV * pFileHead->UVCount;
		//һ��������4���ֽ�
		const int BytePerPixel = 4;
		const int PixelCount = ((int)TextureDataSize) / BytePerPixel;
		GenerateWidthHeight(PixelCount, &nWidth, &nHeight);
		//�������������֮���buff��С��
		const int SizeofNewSrcBuff = nWidth * nHeight * BytePerPixel;
		//
		pTextureData = (char*)malloc(SizeofNewSrcBuff);
		if (pTextureData == 0)
		{
			SoLogError("StKKFileKkbRead::LoadFileData : malloc fail! size[%d]", SizeofNewSrcBuff);
			break;
		}

		ReadCount = fread(pTextureData, 1, TextureDataSize, fp);
		if (ReadCount != TextureDataSize)
		{
			SoLogError("StKKFileKkbRead::LoadFileData : fread TextureData fail!");
			break;
		}

		br = true;

	} while (0);

	if (br == true)
	{
		*ppFileHead = pFileHead;
		*ppVertexStructBuff = pVertexStructBuff;
		*ppTextureData = pTextureData;
		*pTextureWidth = nWidth;
		*pTextureHeight = nHeight;
	}
	else
	{
		if (pFileHead)
		{
			free(pFileHead);
			pFileHead = 0;
		}
		if (pVertexStructBuff)
		{
			free(pVertexStructBuff);
			pVertexStructBuff = 0;
		}
		if (pTextureData)
		{
			free(pTextureData);
			pTextureData = 0;
		}
	}

	fclose(fp);
	return br;
}
//----------------------------------------------------------------
bool StKKFileKkbRead::CreateVertexStructBuff(const StKkbFileHead* pFileHead, const char* pSrcBuff, ID3D11Buffer** ppDXBuffer)
{
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = pFileHead->VertexSize * pFileHead->VertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = pSrcBuff;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	ID3D11Device* pD3DDevice = D3DApp::Get()->GetD3DDevice();
	HRESULT hr = pD3DDevice->CreateBuffer(&vertexBufferDesc, &vertexData, ppDXBuffer);
	if (SUCCEEDED(hr))
	{
		return true;
	}
	else
	{
		SoLogError("StKKFileKkbRead::CreateVertexStructBuff : CreateBuffer fail!");
		return false;
	}
}
//----------------------------------------------------------------
bool StKKFileKkbRead::CreateIndexBuff(const StKkbFileHead* pFileHead, ID3D11Buffer** ppDXBuffer)
{
	const int nIndexCount = pFileHead->VertexCount;
	const int nTriangleIndex_123 = pFileHead->TriangleIndex_123;

	const int SizeofIndexArray = sizeof(unsigned int) * nIndexCount;
	unsigned int* pIndexArray = (unsigned int*)malloc(SizeofIndexArray);
	if (pIndexArray == NULL)
	{
		SoLogError("StKKFileKkbRead::CreateIndexBuff : malloc fail! size[%d]", SizeofIndexArray);
		return false;
	}

	const int nTriangleCount = nIndexCount / 3;
	for (int i = 0; i < nTriangleCount; ++i)
	{
		int nIndex = i * 3;
		if (nTriangleIndex_123)
		{
			pIndexArray[nIndex+0] = nIndex + 0;
			pIndexArray[nIndex+1] = nIndex + 1;
			pIndexArray[nIndex+2] = nIndex + 2;
		}
		else
		{
			pIndexArray[nIndex+0] = nIndex + 2;
			pIndexArray[nIndex+1] = nIndex + 1;
			pIndexArray[nIndex+2] = nIndex + 0;
		}
	}

	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = SizeofIndexArray;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = pIndexArray;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	ID3D11Device* pD3DDevice = D3DApp::Get()->GetD3DDevice();
	HRESULT hr = pD3DDevice->CreateBuffer(&indexBufferDesc, &indexData, ppDXBuffer);
	free(pIndexArray);

	if (SUCCEEDED(hr))
	{
		return true;
	}
	else
	{
		SoLogError("StKKFileKkbRead::CreateIndexBuff : CreateBuffer fail!");
		return false;
	}
}
//----------------------------------------------------------------
bool StKKFileKkbRead::CreateVertexValueTexture(int nWidth, int nHeight, const char* pSrcBuff, SoD3DTexture** ppTexture)
{
	SoD3DTexture* pTex = SoD3DTextureManager::Get()->CreateAnimDataTextureDynamic(nWidth, nHeight, (void*)pSrcBuff);
	if (pTex)
	{
		pTex->AddRefTexture();
		*ppTexture = pTex;
		return true;
	}
	else
	{
		SoLogError("StKKFileKkbRead::CreateVertexValueTexture : CreateAnimDataTextureDynamic fail!");
		return false;
	}
}
//----------------------------------------------------------------
void StKKFileKkbRead::GenerateWidthHeight(int nPixelCount, int* pWidth, int* pHeight)
{
	//����ͼ�ڻ��ȴ洢Pos���ݣ�float3������ΪpFileHead->PosCount������
	//Ȼ��洢Normal���ݣ�float3������ΪpFileHead->NormalCount������
	//���洢UV���ݣ�float2������ΪpFileHead->UVCount������
	//����Ҫ��֤����һ���£�������ͼ�ڶ�ȡĳ������ʱ�������ݱ�����ͬһ���ڡ�
	//���磬����ͼ�ڶ�ȡPos���ݣ���ôPos.x��Pos.y��Pos.z������ͬһ���ڡ�
	//�����Ļ�����ͼ��ȱ�����6����������
	//dds��ͼҪ������4����������
	//����������ͼ�����24����������
	const int TryWidth = 600;
	*pWidth = TryWidth;
	*pHeight = (nPixelCount + TryWidth - 1) / TryWidth;
}
//----------------------------------------------------------------
void StKKFileKkbRead::CalculateMeshBoundingBox(const char* pSrcBuff, int nPosCount, SoMathFloat3* pMinPos, SoMathFloat3* pMaxPos)
{
	float fMinX = SoMath_FloatMax;
	float fMinY = SoMath_FloatMax;
	float fMinZ = SoMath_FloatMax;
	float fMaxX = SoMath_FloatMin;
	float fMaxY = SoMath_FloatMin;
	float fMaxZ = SoMath_FloatMin;

	SoMathFloat3* pPosList = (SoMathFloat3*)pSrcBuff;
	for (int i = 0; i < nPosCount; ++i)
	{
		if (pPosList[i].x < fMinX)
		{
			fMinX = pPosList[i].x;
		}
		if (pPosList[i].x > fMaxX)
		{
			fMaxX = pPosList[i].x;
		}

		if (pPosList[i].y < fMinY)
		{
			fMinY = pPosList[i].y;
		}
		if (pPosList[i].y > fMaxY)
		{
			fMaxY = pPosList[i].y;
		}

		if (pPosList[i].z < fMinZ)
		{
			fMinZ = pPosList[i].z;
		}
		if (pPosList[i].z > fMaxZ)
		{
			fMaxZ = pPosList[i].z;
		}
	}

	pMinPos->x = fMinX;
	pMinPos->y = fMinY;
	pMinPos->z = fMinZ;
	pMaxPos->x = fMaxX;
	pMaxPos->y = fMaxY;
	pMaxPos->z = fMaxZ;
}
//----------------------------------------------------------------
