
#include "CGameMapelement.h"
#include "GamePathFinder.h"

CGameMapelement::CGameMapelement()
{
}

CGameMapelement::~CGameMapelement()
{

}

CGameMapelement::CGameMapelement(float x, float y
	, bool bLeft, bool bTop, bool bRight, bool bBottom)
	: m_bLeft(bLeft), m_bTop(bTop), m_bRight(bRight), m_bBottom(bBottom)
{
}


CGameMap::CGameMap()
{
	m_finder = nullptr;
	//for (int i = 0; i < 23; i++)
	//	for (int j = 0; j < 42; j++)
	//{
	//		if(i==0||j==0||j==41||i==22)
	//			FinderTankMap[i][j] = '-';//不能通过;
	//		else
	//			FinderTankMap[i][j] = '+';
	//}
}

CGameMap::~CGameMap()
{
}

void CGameMap::FindPath(int startX, int startY, int targetX, int targetY, VPath &path) const
{
	path.clear();
	m_finder->FindPath(startX, startY, targetX, targetY, path);
}


// 获得下一次的前进方向
bool CGameMap::FindNextDirection(DIRECT* newdir, int startX, int startY, int targetX, int targetY,ROT_DIR &ROT) const
{
	VPath path;
	if ((startX == targetX) && (startY == targetY))
	{
	return false;
	}
	FindPath(startX, startY, targetX, targetY, path);
	if (path.empty()) {
		return false;
	}

	// 获得第一个位置
	auto it = path.begin();
	it++;

	if (it == path.end()) {
		return false;
	}



	//前一个方向
	static DIRECT PerDir = MOVE_RIGHT;

	// 计算下一个移动方向和旋转方向
	if (startX == it->x) {
		// 同一行
		if (startY < it->y) {
			// 右侧
			*newdir = MOVE_RIGHT;
			//判断旋转方向
			if (PerDir == MOVE_DOWN)
			{
				ROT = TurnLeft;
			}
			else if (PerDir == MOVE_UP)
			{
				ROT = TurnRight;
			}
			else if(PerDir==MOVE_LEFT)
			{
				ROT = BackWard;
			}
			else
			{
				ROT = NONE;
			}
		}
		else if (startY > it->y) {
			// 左侧
			*newdir = MOVE_LEFT;
			//判断旋转方向
			if (PerDir == MOVE_DOWN)
			{
				ROT = TurnRight;
			}
			else if (PerDir == MOVE_UP)
			{
				ROT = TurnLeft;
			}
			else if (PerDir == MOVE_RIGHT)
			{
				ROT = BackWard;
			}
			else
			{
				ROT = NONE;
			}
		}
	}
	else if (startY == it->y) {
		// 同一列
		if (startX < it->x) {
			// 下方
			*newdir = MOVE_DOWN;
			//判断旋转方向
			if (PerDir == MOVE_RIGHT)
			{
				ROT = TurnRight;
			}
			else if (PerDir == MOVE_LEFT)
			{
				ROT = TurnLeft;
			}
			else if (PerDir == MOVE_UP)
			{
				ROT = BackWard;
			}
			else
			{
				ROT = NONE;
			}
		}
		else if (startX > it->x) {
			// 上方
			*newdir = MOVE_UP;
			//判断旋转方向
			if (PerDir == MOVE_RIGHT)
			{
				ROT = TurnLeft;
			}
			else if (PerDir == MOVE_LEFT)
			{
				ROT = TurnRight;
			}
			else if (PerDir == MOVE_DOWN)
			{
				ROT = BackWard;
			}
			else
			{
				ROT = NONE;
			}
		}
	}

	//更新前一方向
	PerDir = *newdir;

	return true;
}


bool CGameMap::ConvertMap(std::vector<std::vector<char>>& MapData)
{
	bool Left = true;
	bool Top = true;
	bool Right = true;
	bool Botton = true;
	for (int i = 0; i <MapSize; ++i)
	{
		std::vector<CGameMapelement> m_row;//一行地图
		for (int j = 0; j <MapSize; ++j)
		{

			if (j == 0)//第一列
			{
				Left = false;
			}
			else if (j == (MapSize - 1))//最后一列
			{
				Right = false;
			}


			if(i==0)//第一行
			{
				Top = false;
			}
			else if(i==(MapSize-1))//最后一行
			{
				Botton = false;
			}



			m_row.push_back(CGameMapelement(i, j,
				Left ? ((MapData[i][j - 1] == '+') ? true : false) : false,
				Top ? ((MapData[i - 1][j] == '+') ? true : false) : false,
				Right ? ((MapData[i][j + 1] == '+') ? true : false) : false,
				Botton ? ((MapData[i + 1][j] == '+') ? true : false) : false));


			/*m_row.push_back(CGameMapelement(i - 1, j - 1,
				(FinderTankMap[i][j-1] == '-') ? false : true,
				(FinderTankMap[i-1][j] == '-') ? false : true,
				(FinderTankMap[i][j+1]=='-' )? false : true,
				(FinderTankMap[i+1][j]== '-')? false : true));*/

			//reset
			Left = true;
			Top = true;
			Right = true;
			Botton = true;
		}
		//加一行
		m_arr.push_back(m_row);
	}
	m_finder = new CPathFinderBFS(m_arr);
	return true;
}