//----------------------------------------------------------------
#ifndef _StKKDefine_h_
#define _StKKDefine_h_
//----------------------------------------------------------------
#include "d3dUtil.h"
#include "GED3DTexture.h"
//----------------------------------------------------------------
//ÿ����������ܼ�������Ӱ�졣
#define StKK_MaxBonePerVertex 4
//----------------------------------------------------------------
enum StKKVertexType
{
	//��Чֵ
	StKKVertexType_Invalid,
	//����ṹ��������꣬���ߣ�UV������������Щ����Ӱ���Լ�Ӱ��Ȩ�أ���
	StKKVertexType_Pos_Normal_UV_Bone,
	//����ṹ��������꣬���ߣ�UV��
	StKKVertexType_Pos_Normal_UV,
};
//----------------------------------------------------------------
struct StKKVertexStruct_Pos_Normal_UV
{
	int PosIndex;
	int NormalIndex;
	int TangentIndex;
	int UVIndex;
};
//----------------------------------------------------------------
struct StKKVertexStruct_Pos_Normal_UV_Bone
{
	int PosIndex;
	int NormalIndex;
	int TangentIndex;
	int UVIndex;
	int BoneIndex[StKK_MaxBonePerVertex];
	float BoneWeight[StKK_MaxBonePerVertex];
};
//----------------------------------------------------------------
struct StKKBoneTransform
{
	GEMathMatrix3 Rotate;
	GEMathFloat3 Translate;
};
//----------------------------------------------------------------
struct StKKModelData
{
	ID3D11Buffer* pVertexStructBuffer;
	ID3D11Buffer* pIndexBuffer;
	GED3DTexture* pVertexValueTexture;
	//����ṹ�����ͣ�ֵΪ StKKVertexType ö��ֵ��
	int nVertexType;
	int nVertexCount;
	int nIndexCount;
	int nSizeofVertexStruct;
	int nTextureWidth;
	int nTextureHeight;
	//����ֵ�ĸ�����
	int nPosCount;
	//����ֵ�ĸ�����
	int nNormalCount;
	int nTangentCount;
	//UVֵ�ĸ�����
	int nUVCount;
	GEMathFloat3 kPosMinValue;
	GEMathFloat3 kPosMaxValue;

	StKKModelData();
	~StKKModelData();
};
//----------------------------------------------------------------
struct StKKModelAnimation
{
	GED3DTexture* pAnimationTexture;
	int nAnimID;
	int nTextureWidth;
	int nTextureHeight;
	//��������ʱ������λ���롣
	int nTimeLength;
	//һ���ж��ٸ�����֡��
	int nKeyFrameCount;
	//һ���ж��ٸ�������
	int nBoneCount;

	StKKModelAnimation();
	~StKKModelAnimation();
};
//----------------------------------------------------------------
//                           Kkb
//----------------------------------------------------------------
//Kkb�ļ���ʽ
//1��StKkbFileHead
//2��StKKVertexStruct �б�����ṹ��
//3��GEMathFloat3 �б�����ֵ
//4��GEMathFloat3 �б�����ֵ
//5��GEMathFloat2 �б�UVֵ
//----------------------------------------------------------------
#define StKkbFileFlagSize 5
extern const char* StKkbFileFlagString;
//----------------------------------------------------------------
struct StKkbFileHead
{
	//�ļ���ǡ�
	char FileFlag[StKkbFileFlagSize];
	//����ṹ�����ͣ�ֵΪ StKKVertexType ö��ֵ��
	int VertexType;
	//����ṹ���sizeof��
	int VertexSize;
	//����ṹ��ĸ�����
	int VertexCount;
	//����ֵ�ĸ�����
	int PosCount;
	//����ֵ�ĸ�����
	int NormalCount;
	//����ֵ�ĸ�����
	int TangentCount;
	//UVֵ�ĸ�����
	int UVCount;
	//��������Ƭ�����������з�ʽ��Ҳ������
	//�����ֵΪ1����ʾʹ�á�123˳�򹹳������Ρ���ֵΪ0��ʾʹ�á�321˳�򹹳������Ρ���
	//���������ĸ����붥��ṹ��ĸ�����ͬ��
	int TriangleIndex_123;
};
//----------------------------------------------------------------
//                           Kkf
//----------------------------------------------------------------
//Kkf�ļ���ʽ
//1��StKkfFileHead
//2��StKKBoneTransform �б�
//----------------------------------------------------------------
#define StKkfFileFlagSize 5
extern const char* StKkfFileFlagString;
//----------------------------------------------------------------
struct StKkfFileHead
{
	//�ļ���ǡ�
	char FileFlag[StKkfFileFlagSize];
	//��������ʱ������λ���롣
	int TimeLength;
	//һ���ж��ٸ�����֡��
	int KeyFrameCount;
	//һ���ж��ٸ�������
	int BoneCount;
};
//----------------------------------------------------------------
#endif //_StKKDefine_h_
//----------------------------------------------------------------
