#pragma once

#include <vector>
#include <map>
#include "SoSimpleLoadFile.h"
#include "d3dUtil.h"
#include "d3dApp.h"
//Manage 10x10 maps 
#define UnitMapSize 10

struct UnitSence
{
	//��ͼ����
	char SenceData[UnitMapSize][UnitMapSize];
	//��ͼƫ����
	XMFLOAT3 SenceOffset;
	//ȫ�ֵ�ͼ����
	std::vector<XMINT2> IndexData;
	//ÿ���ת������
	std::vector<XMFLOAT4X4> TansformData;
	//Model Info
	std::map<char, Model_Tansform_Info> ModelInfo;
};


class SenceManager
{
public:
	SenceManager();
	~SenceManager();
	void DivisionMap(int UnitMapOffset);
	bool ReadMapDataFormFile(const char* szFileName);
	std::vector<UnitSence>& GetSenceData();
private:
	SoSimpleLoadFile* pLoadFile;
	std::vector<UnitSence> Sence;
};


