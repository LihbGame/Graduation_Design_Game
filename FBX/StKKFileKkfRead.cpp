//----------------------------------------------------------------
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif //_CRT_SECURE_NO_WARNINGS
//----------------------------------------------------------------
#include "StKKFileKkfRead.h"
#include "StFBXDefine.h"
#include "d3dApp.h"
#include "GED3DTextureManager.h"
#include "GESimpleLog.h"
//----------------------------------------------------------------
bool StKKFileKkfRead::ReadKkf(const char* szFileName, StKKModelAnimation* pModelAnim)
{
	if (szFileName == 0 || szFileName[0] == 0)
	{
		return false;
	}
	if (pModelAnim == 0)
	{
		return false;
	}

	StKkfFileHead* pFileHead = 0;
	char* pTextureData = 0;

	GED3DTexture* theAnimTexture = 0;
	int nTextureWidth = 0;
	int nTextureHeight = 0;

	GELogDebug("StKKFileKkfRead::ReadKkf : Start; FileName[%s]", szFileName);

	bool br = false;
	do 
	{
		if (LoadFileData(szFileName, &pFileHead, &pTextureData, &nTextureWidth, &nTextureHeight) == false)
		{
			break;
		}

		if (CreateAnimTexture(nTextureWidth, nTextureHeight, pTextureData, &theAnimTexture) == false)
		{
			break;
		}

		br = true;

	} while (0);


	if (br == true)
	{
		pModelAnim->pAnimationTexture = theAnimTexture;
		pModelAnim->nTextureWidth = nTextureWidth;
		pModelAnim->nTextureHeight = nTextureHeight;
		pModelAnim->nTimeLength = pFileHead->TimeLength;
		pModelAnim->nKeyFrameCount = pFileHead->KeyFrameCount;
		pModelAnim->nBoneCount = pFileHead->BoneCount;
	}
	else
	{
		if (theAnimTexture)
		{
			theAnimTexture->RemoveRefTexture();
			theAnimTexture = 0;
		}
	}

	//不管操作是否成功，都要删除下面的内存。
	if (pFileHead)
	{
		free(pFileHead);
		pFileHead = 0;
	}
	if (pTextureData)
	{
		free(pTextureData);
		pTextureData = 0;
	}

	GELogDebug("StKKFileKkfRead::ReadKkf : Finish; Success[%d]", br?1:0);
	return br;
}
//----------------------------------------------------------------
bool StKKFileKkfRead::LoadFileData(const char* szFileName, StKkfFileHead** ppFileHead, char** ppTextureData, int* pTextureWidth, int* pTextureHeight)
{
	FILE* fp = fopen(szFileName, "rb");
	if (fp == 0)
	{
		GELogError("StKKFileKkfRead::LoadFileData : fopen fail! [%s]", szFileName);
		return false;
	}

	StKkfFileHead* pFileHead = 0;
	char* pTextureData = 0;
	int nWidth = 0;
	int nHeight = 0;

	bool br = false;
	do 
	{
		//读取文件头。
		const unsigned int SizeofFileHead = sizeof(StKkfFileHead);
		pFileHead = (StKkfFileHead*)malloc(SizeofFileHead);
		if (pFileHead == 0)
		{
			GELogError("StKKFileKkfRead::LoadFileData : malloc fail! size[%d]", SizeofFileHead);
			break;
		}

		unsigned int ReadCount = fread(pFileHead, 1, SizeofFileHead, fp);
		if (ReadCount != SizeofFileHead)
		{
			GELogError("StKKFileKkfRead::LoadFileData : fread FileHead fail!");
			break;
		}

		//判断文件标记。
		bool bFileFlagOK = true;
		for (int i = 0; i < StKkfFileFlagSize; ++i)
		{
			if (pFileHead->FileFlag[i] != StKkfFileFlagString[i])
			{
				bFileFlagOK = false;
				break;
			}
		}
		if (bFileFlagOK == false)
		{
			GELogError("StKKFileKkfRead::LoadFileData : FileFlag check fail!");
			break;
		}

		//读取顶点元素值的贴图数据。
		//为了创建贴图，必须把末尾行扩充成整行。
		//贴图内存储了(pFileHead->BoneCount * pFileHead->KeyFrameCount)个StKKBoneTransform对象。
		const unsigned int TextureDataSize = pFileHead->BoneCount * pFileHead->KeyFrameCount * sizeof(StKKBoneTransform);
		//一个像素是4个字节
		const int BytePerPixel = 4;
		const int PixelCount = ((int)TextureDataSize) / BytePerPixel;
		GenerateWidthHeight(PixelCount, &nWidth, &nHeight);
		//计算扩充成整行之后的buff大小。
		const int SizeofNewSrcBuff = nWidth * nHeight * BytePerPixel;
		//
		pTextureData = (char*)malloc(SizeofNewSrcBuff);
		if (pTextureData == 0)
		{
			GELogError("StKKFileKkfRead::LoadFileData : malloc fail! size[%d]", SizeofNewSrcBuff);
			break;
		}

		ReadCount = fread(pTextureData, 1, TextureDataSize, fp);
		if (ReadCount != TextureDataSize)
		{
			GELogError("StKKFileKkfRead::LoadFileData : fread TextureData fail!");
			break;
		}

		br = true;

	} while (0);

	if (br == true)
	{
		*ppFileHead = pFileHead;
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
bool StKKFileKkfRead::CreateAnimTexture(int nWidth, int nHeight, const char* pSrcBuff, GED3DTexture** ppTexture)
{
	GED3DTexture* pTex = GED3DTextureManager::Get()->CreateAnimDataTextureDynamic(nWidth, nHeight, (void*)pSrcBuff);
	if (pTex)
	{
		pTex->AddRefTexture();
		*ppTexture = pTex;
		return true;
	}
	else
	{
		GELogError("StKKFileKkfRead::CreateVertexValueTexture : CreateAnimDataTextureDynamic fail!");
		return false;
	}
}
//----------------------------------------------------------------
void StKKFileKkfRead::GenerateWidthHeight(int nPixelCount, int* pWidth, int* pHeight)
{
	//贴图内存储了(pFileHead->BoneCount * pFileHead->KeyFrameCount)个StKKBoneTransform对象。
	//当从贴图内读取StKKBoneTransform数据时，要保证该数据位于同一行内。
	//一个StKKBoneTransform对象含有12个float数据。
	//所以贴图宽度必须是12的整数倍。
	//dds贴图要求宽度是4的整数倍。
	//所以最终贴图宽度是12的整数倍。
	const int TryWidth = 600;
	*pWidth = TryWidth;
	*pHeight = (nPixelCount + TryWidth - 1) / TryWidth;
}
//----------------------------------------------------------------
