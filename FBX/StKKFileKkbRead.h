//----------------------------------------------------------------
#ifndef _StKKFileKkbRead_h_
#define _StKKFileKkbRead_h_
//----------------------------------------------------------------
#include "StKKDefine.h"
//----------------------------------------------------------------
class StKKFileKkbRead
{
public:
	static bool ReadKkb(const char* szFileName, StKKModelData* pModelData);

private:
	static bool LoadFileData(const char* szFileName, StKkbFileHead** ppFileHead, char** ppVertexStructBuff, char** ppTextureData, int* pTextureWidth, int* pTextureHeight);
	static bool CreateVertexStructBuff(const StKkbFileHead* pFileHead, const char* pSrcBuff, ID3D11Buffer** ppDXBuffer);
	static bool CreateIndexBuff(const StKkbFileHead* pFileHead, ID3D11Buffer** ppDXBuffer);
	static bool CreateVertexValueTexture(int nWidth, int nHeight, const char* pSrcBuff, GED3DTexture** ppTexture);
	static void GenerateWidthHeight(int nPixelCount, int* pWidth, int* pHeight);
	static void CalculateMeshBoundingBox(const char* pSrcBuff, int nPosCount, GEMathFloat3* pMinPos, GEMathFloat3* pMaxPos);
};
//----------------------------------------------------------------
#endif //_StKKFileKkbRead_h_
//----------------------------------------------------------------
