#pragma once

#include<vector>
class CGamePathFinder;

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
	//int dir; // 0:��1:�ϣ�2:�ң�3:��
} PathNode;

typedef std::vector<PathNode> VPath;
/*
�����е�ͼ�е�һ������
���ܿ�����ǽ����ͨ���� �м��ǿյؿ���ͨ��
*/
class CGameMapelement
{
public:
	CGameMapelement();
	CGameMapelement(float x, float y
		, bool bLeft, bool bTop, bool bRight, bool bBottom
		);

	~CGameMapelement();

	
	// �ĸ����������Ƿ���ǽ
	bool m_bLeft{ false };
	bool m_bTop{ false };
	bool m_bRight{ false };
	bool m_bBottom{ false };

};

/* ����������ͼ
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
	/// ·�� ����
	//  path ����·�� [��㣬�������� �յ�]
	void FindPath(int startX, int startY, int targetX, int targetY, VPath &path) const;

	// �����һ�ε�ǰ������
	bool FindNextDirection(DIRECT* dir, int startX, int startY, int targetX, int targetY,ROT_DIR &ROT) const;
	
	

	bool ConvertMap();
private:
	// �潲���еĵ�ͼ
	std::vector<std::vector<CGameMapelement>> m_arr;
public:
	char FinderTankMap[23][42];
	std::vector<TankPosition> m_TankPosition;
	std::vector<TankPosition> m_MyTankPosition;
private:
	// ·������
	CGamePathFinder * m_finder{ nullptr };
};

