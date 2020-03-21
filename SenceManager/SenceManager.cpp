#include "SenceManager.h"
SenceManager::SenceManager()
{
	//��ʼ��
	pLoadFile = new SoSimpleLoadFile;
}

SenceManager::~SenceManager()
{
	delete pLoadFile;
}

//���ֵ�ͼ
void SenceManager::DivisionMap(int UnitMapOffset, std::vector<std::vector<char>>* AIMapData)
{
	//Ԥ���� 9x(10x10) 
	Sence.resize(9);
	char* data = pLoadFile->GetData();
	char* temp =0;
	for (int i = 0; i < 3; ++i)
	{
		for (int k = 0; k < UnitMapSize; ++k)
		{
			std::vector<char> OneRow;
			for (int j = 0; j < 3; ++j)
			{
				for (int m = 0; m < UnitMapSize; ++m)
				{
					temp = &data[i*330+k*33+j*11+m];
					
					//���Կո�ͻ��з�
					while (*temp == ' ' || *temp == '\n')
					{
						temp++;
					}
					//AI data
					if (AIMapData != nullptr)
					{
						OneRow.push_back(*temp);
					}
					//�����ͼ����
					Sence[i * 3 + j].SenceData[k][m] = *temp;
					//�����ͼȫ������
					if (*temp == '+')
					{
						Sence[i * 3 + j].IndexData.push_back(XMINT2(i * UnitMapSize + k, j * UnitMapSize + m));
					}
				}
			}
			//һ��ai��ͼ����
			if (AIMapData != nullptr)
			{
				AIMapData->push_back(OneRow);
			}
		}
	}

	//ÿ���ͼ��ƫ��(10x10)
	int offset = UnitMapOffset * 10;
	for (int x = 0; x < 3; ++x)
	{
		for (int y = 0; y < 3; ++y)
		{
			Sence[x * 3 + y].SenceOffset= XMFLOAT3(x * offset, 0.0f, y * offset);
		}
	}
}


bool SenceManager::ReadMapDataFormFile(const char* szFileName)
{
	if (!pLoadFile->LoadFile(szFileName, true))
	{
		return false;
	}
	return true;
}

std::vector<UnitSence>& SenceManager::GetSenceData()
{
	return Sence;
}
