#pragma once

#include <vector>
#include <map>
#include "GESimpleLoadFile.h"
#include "d3dUtil.h"
#include "d3dApp.h"
//Manage 10x10 maps 
#define UnitMapSize 10

struct UnitSence
{
	//地图数据
	char SenceData[UnitMapSize][UnitMapSize];
	//地图偏移量
	XMFLOAT3 SenceOffset;
	//全局地图索引
	std::vector<XMINT2> IndexData;
	//每块的转换矩阵
	std::vector<XMFLOAT4X4> TansformData;
	//Model Info
	std::map<char, Model_Tansform_Info> ModelInfo;
};


class SenceManager
{
public:
	SenceManager();
	~SenceManager();
	void DivisionMap(int UnitMapOffset, std::vector<std::vector<char>>* aidata=nullptr);
	bool ReadMapDataFormFile(const char* szFileName);
	std::vector<UnitSence>& GetSenceData();
private:
	GESimpleLoadFile* pLoadFile;
	std::vector<UnitSence> Sence;
};


