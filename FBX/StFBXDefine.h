//----------------------------------------------------------------
#ifndef _StFBXDefine_h_
#define _StFBXDefine_h_
//----------------------------------------------------------------
#include "SoMath.h"
#include "SoTinyString.h"
#include "SoBitFlag.h"
#include "StFBXUserDefine.h"
//----------------------------------------------------------------
//Vector3��3��floatԪ��
#define StFBX_Sizeof_Vector3 12
//Color��4��floatԪ��
#define StFBX_Sizeof_Color 16
//UV��2��floatԪ��
#define StFBX_Sizeof_UV 8
//����Pixel��Sizeof
#define StFBX_Sizeof_Pixel 4
//������ŵ�sizeofֵ��32λ���͡�
#define StFBX_Sizeof_VertexIndex 4
//----------------------------------------------------------------
enum StFBXElementType
{
	StFBXElement_Position = 0x00000001,
	StFBXElement_Normal = 0x00000002,
	StFBXElement_Tangent = 0x00000004,
	StFBXElement_Color = 0x00000008,
	StFBXElement_UV1 = 0x00000010,
	StFBXElement_UV2 = 0x00000020,
};
//----------------------------------------------------------------
//�洢fbx�ļ������е�Mesh�Ķ������ݡ�
//�ڽ���fbx�ļ���Ϻ�������洢�Ķ������ݾ���ģ��Ĭ��Pose�Ķ������ݡ�
//����ṹ������붥��������������ͬ�ġ�
//�����ζ�ţ����һ�����㱻�������������ι��õĻ�������������ݾͱ������������ݶ���ṹ�塣
struct StFBXMeshData
{
	char* pVertexBuff;
	int nVertexBuffSize;
	int nVertexCount;
	char* pIndexBuff;
	int nIndexBuffSize;
	int nIndexCount;
	//����λ����ŵ����Ƶ�λ����ŵ�ӳ�䡣
	//������ĳ��Ⱦ���nVertexCount��
	int* pVertexIndex2ControlPointIndex;
	//����ṹ���к�����Щ��Ա�����ߣ���ͼ����ȵȣ���
	SoBitFlag kVertexType;
	//����ṹ����ֽ�����
	//����Լ����һ��fbx�ļ��У����е�Mesh�Ķ������ݸ�ʽ����ͬ��
	//��ͬ��mesh�У�����ṹ��ĳ�Ա��ͬ������ṹ����ֽ���Ҳ��ͬ��
	int nSizeofVertexData;

	StFBXMeshData();
	~StFBXMeshData();
	void SetVertexType(const SoBitFlag& kType);
	void ReserveVertexCount(const int nVertexCount);
};
//----------------------------------------------------------------
//�洢������ź͸ù�����ĳ�����Ƶ��Ӱ��Ȩ�ء�
//BoneIndex����StFBXBoneGroup�е������±ꡣ
struct StFBXBoneIndexSkinWeight
{
	SoTinyString kBoneName;
	int nBoneIndex;
	float fSkinWeight;

	StFBXBoneIndexSkinWeight();
};
//----------------------------------------------------------------
//�������Ƶ㡣
struct StFBXControlPoint
{
	//�������ꡣ
	SoMathFloat3 kVertex;
	//�����Ƶ�����Щ������Ӱ�죬�Լ�Ӱ��Ȩ�ء�
	StFBXBoneIndexSkinWeight kPairList[StFBX_MaxCount_BoneIndexSkinWeightPerControlPoint];

	StFBXControlPoint();
	void AddBoneNameSkinWeight(const char* szBoneName, float fSkinWeight);
};
//----------------------------------------------------------------
//�洢fbx�����е�Mesh�Ŀ��Ƶ㡣
struct StFBXBoneGroup; //ǰ������
struct StFBXControlPointGroup
{
private:
	StFBXControlPoint* kControlPointArray;
	int nControlPointValidCount;
	int nControlPointMaxCount;

public:
	StFBXControlPointGroup();
	~StFBXControlPointGroup();
	void ReserveControlPointCount(int nCount);
	//����緵��һ��δʹ�õ� StFBXControlPoint �����������丳ֵ��
	StFBXControlPoint* TakeNew();
	StFBXControlPoint* GetAt(int nIndex) const;
	int GetSize() const;
	//�ʼ��StFBXBoneIndexSkinWeight�����д洢���ǹ������֡�
	//�����˹������ݺ󣬾ͽ���������ת������Ӧ�Ĺ�����š�
	void MakeBoneIndexByBoneName(const StFBXBoneGroup* pBoneGroup);

	//���ÿ�����Ƶ���ܹ���Ӱ��Ȩ�أ���֤Ȩ�ص��ܺ���1��
	//��������Ϊ����Ҫ��֤Ȩ�ص��ܺ���1��
	//���磬��һ�����㣬��ֻ��������������΢��Ӱ�죬
	//Ȩ��ֵ�Ƚ�С�������֡���΢������ôȨ�ص��ܺͱض�����1��
	//void CheckBoneSkinWeight();
};
//----------------------------------------------------------------
//�����ڵ�
struct StFBXBone
{
	//�����������֡�
	SoTinyString kBoneName;
	//�������ڲ㼶�б��е����λ�á�
	int nParentIndex;
	//�ӹ����ڲ㼶�б��е����λ�á�ÿ��Ԫ�ض���һ��intֵ��
	int kChildIndexList[StFBX_MaxCount_ChildBone];
	//�������Ź����У���������ļ��㹫ʽ��
	//VertexAtTimeT = VertexFromControlPoint * kMatFromBoneSpaceToWorldSpace * KeyFrameMatrixAtTimeT
	SoMathMatrix4 kMatFromBoneSpaceToModelSpace;

	StFBXBone();
	~StFBXBone();
};
//----------------------------------------------------------------
//�洢fbx�ļ������е�Bone�������㼶�ṹ Hierarchy��
struct StFBXBoneGroup
{
private:
	StFBXBone* kBoneArray;
	int nBoneValidCount;
	int nBoneMaxCount;

public:
	StFBXBoneGroup();
	~StFBXBoneGroup();
	void ReserveBoneCount(int nCount);
	//���������б��ֱ�����ÿ���������ӹ����б�
	void GenerateChildren();
	int GetBoneIndexByBoneName(const char* szBoneName) const;
	//����緵��һ��δʹ�õ� StFBXBone �����������丳ֵ��
	StFBXBone* TakeNew(int* pIndex);
	StFBXBone* GetAt(int nIndex) const;
	int GetSize() const;
};
//----------------------------------------------------------------
//����֡
struct StFBXKeyFrame
{
	float fKeyTime;
	SoMathMatrix4 matKeyTransform;

	StFBXKeyFrame();
};
//----------------------------------------------------------------
//��һ�������У�һ��Bone��Я�������еĶ���֡��
//BoneIndex����StFBXBoneGroup�е������±ꡣ
struct StFBXBoneAnimation
{
	StFBXKeyFrame* kKeyFrameArray;
	int nFrameValidCount;
	int nFrameMaxCount;
	int nBoneIndex;

	StFBXBoneAnimation();
	~StFBXBoneAnimation();
	void ReserveKeyFrameCount(int nCount);
	//����緵��һ��δʹ�õ� StFBXKeyFrame �����������丳ֵ��
	StFBXKeyFrame* TakeNew();
	StFBXKeyFrame* GetAt(int nIndex) const;
	int GetSize() const;
};
//----------------------------------------------------------------
//��һ�������У�һ��ģ����Я�������еĶ���֡��
struct StFBXModelAnimation
{
	//���й����Ķ���֡�б�ÿ��Ԫ�ض���һ��StFBXBoneAnimation����
	//һ������£��������size���ǹ����ܸ�����
	//�п��ܳ�������������ڶ����У�ֻ��һ���ֹ������������ã�
	//��ô�������size�ͻ�С�ڹ����ܸ�����
	StFBXBoneAnimation* kBoneAnimationArray;
	int nAnimValidCount;
	int nAnimMaxCount;
	//�����ж���֡��
	int nFrameCount;
	//�����ĳ���ʱ������λ�롣
	float fAnimLength;

	StFBXModelAnimation();
	~StFBXModelAnimation();
	void ReserveBoneCount(int nBoneCount);
	int GetSize() const;
	int GetFrameCount() const;
	//����緵��һ��δʹ�õ� StFBXBoneAnimation �����������丳ֵ��
	StFBXBoneAnimation* TakeNew();
	StFBXBoneAnimation* GetAt(int nIndex) const;
	StFBXBoneAnimation* GetBoneAnimation(int nBoneIndex) const;
	//����ָ����ʱ�䣬��ȡ��ʱ��Ҫ���Ŷ�������һ֡��
	int GetKeyFrameIndexByTime(float fTime);
};
//----------------------------------------------------------------
#endif //_StFBXDefine_h_
//----------------------------------------------------------------
