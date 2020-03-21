#pragma once

#include<vector>
class CGamePathFinder;
#define MapSize 30
enum DIRECT
{
	MOVE_UP = 0,
	MOVE_DOWN = 1,
	MOVE_LEFT = 2,
	MOVE_RIGHT = 3,
	NO_MOVE = 4
};

enum ROT_DIR
{
	Forward = 0,
	TurnRight = 1,
	TurnLeft = 2,
	BackWard = 3,
	NONE=4
};

typedef struct PathNode {
	int x;
	int y;
	//int dir; // 0:左，1:上，2:右，3:下
} PathNode;

typedef std::vector<PathNode> VPath;
/*
代表中地图中的一个方块
四周可能有墙不可通过， 中间是空地可以通过
*/
class CGameMapelement
{
public:
	CGameMapelement();
	CGameMapelement(float x, float y
		, bool bLeft, bool bTop, bool bRight, bool bBottom
		);

	~CGameMapelement();

	
	// 四个方向上面是否有墙
	bool m_bLeft{ false };
	bool m_bTop{ false };
	bool m_bRight{ false };
	bool m_bBottom{ false };

};

/* 代表整个地图
*/
struct TankPosition
{
	int m_x;
	int m_y;
};
class CGameMap 
{
public:
	CGameMap();
	virtual ~CGameMap();

	
	/// ///////////////////////////////////////////////////////////////////////////////
	/// 路径 搜索
	//  path 返回路径 [起点，。。。， 终点]
	void FindPath(int startX, int startY, int targetX, int targetY, VPath &path) const;

	// 获得下一次的前进方向
	bool FindNextDirection(DIRECT* dir, int startX, int startY, int targetX, int targetY,ROT_DIR &ROT) const;
	
	

	bool ConvertMap(std::vector<std::vector<char>>& Aidata);
private:
	// 存讲所有的地图
	std::vector<std::vector<CGameMapelement>> m_arr;
public:
	std::vector<TankPosition> m_TankPosition;
	std::vector<TankPosition> m_MyTankPosition;
private:
	// 路径搜索
	CGamePathFinder * m_finder{ nullptr };
};

