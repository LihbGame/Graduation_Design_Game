//----------------------------------------------------------------
#ifndef _SoArrayUID_h_
#define _SoArrayUID_h_
//----------------------------------------------------------------
//#include "SoCodeBaseInclude.h"
#include "SoIntType.h"
//----------------------------------------------------------------
class  SoArrayUID
{
public:
	SoArrayUID();
	~SoArrayUID();
	//��ʼ��������Ҳ������Ϊ�������ú�������һ�����������ó������顣
	//--nElementSize Ԫ�ش�С������Ԫ��ִ��sizeof����õ��ķ���ֵ��
	//--nInitCapacity ��ʼʱ�����Ԫ�ظ�����Ҳ��Ԥ������ڴ��ܹ����ɶ��ٸ�Ԫ�ء�
	//--nAddCount ������Ԫ��������Ԥ��������ڴ�ʱ�������ӵ��ڴ��ܹ����ɶ��ٸ�Ԫ�ء�
	bool InitArray(soint32 nElementSize, soint32 nInitCapacity, soint32 nAddCount);
	//���������ڴ棬�洢�����Ԫ�ء�
	//���newCapacity�ȵ�ǰ��m_nCapacityС����ʲô��������
	bool ResizeArray(soint32 newCapacity);
	//������е�Ԫ�ء��ڴ治�ᱻ�ͷŵ���
	void ClearArray();
	//����Ԫ����䵽nUIDλ�ã����ԭnUIDλ�õ�Ԫ�ظ��ǵ���
	//����ֵΪ��Ԫ�ص�UID��
	//nUID��ֵΪ-1����ʾ�Զ���һ��Status_Emptyλ�ý�����䡣
	soint32 FillAt(soint32 nUID, const void* pElement);
	//����һ����Ԫ�أ�������������Ԫ�ص�ֵ��
	//�����ҵ�״̬ΪStatus_Empty��Ԫ�أ�Ȼ���ٷ��ء�
	//����ֵΪ��Ԫ�ص�λ��������
	soint32 TakeNew(void** ppElement);
	//���nUIDλ�õ�Ԫ�أ�����״̬���ó�Status_Empty��
	void ClearAt(soint32 nUID);
	//��ȡ������������
	soint32 GetCapacity() const;
	//��ȡnUIDλ�õ�Ԫ�أ����Ҹ�Ԫ�ص�״̬������Status_Using�����򷵻�NULL��
	void* GetAt(soint32 nUID) const;
	////״̬������
	//soint32 GetStatus(soint32 nUID) const;
	//��ȡ��ЧԪ�صĸ�����
	soint32 GetUsingElementCount() const;
	//��ȡĳ��Ԫ�ص�UID��
	//--nValidSize Ԫ�ص���Чsize��Ҳ�����ж϶��ٸ��ֽھͿ��Եó���Ԫ����ͬ�����ߡ�Ԫ�ز���ͬ���Ľ��ۡ�
	//             �����Ԫ�����ַ�������nValidSize�����ַ����ĳ��ȡ�
	//���Ԫ�����ַ���������д�ַ����ĳ��Ⱦͷǳ���Ҫ�����磺
	//�������Ԫ�����ַ�����ÿ��Ԫ�ص�size��5��
	//���һ��Ԫ�� "ab0cd" ���ַ���ʵ��ֵ�� "ab" ���������֮��Ԫ�ص��ڴ��ֵ�� "ab0cd" ��
	//������һ��Ԫ�� "ab0ff" ʱ������Ϊ���ҵ��ַ����� "ab" ���϶����ҵ�������ʵ�ʲ��ҵ�Ԫ���� "ab0ff" ��������Ҳ�����
	soint32 GetUID(const void* pElement, soint32 nValidSize) const;

private:
	//�ҵ���һ����Ԫ�ء�
	soint32 FindFirstEmptyElement();

private:
	enum eStatusType
	{
		Status_Invalid = 0,
		Status_Empty = 1, //Ԫ��ֵΪ��
		Status_Using = 2, //Ԫ��ֵ����Чֵ�����ڱ����ʹ��
	};

private:
	//�洢Ԫ�ض��󡣸�������±���ž���UID��
	//�б���Ԫ�صķֲ���������ģ����磬0��Ԫ��Ϊ��Чֵ��1��Ԫ��Ϊ��Чֵ��2��Ԫ��Ϊ��Чֵ���ȵȡ�
	char* m_pElementBuff;
	//�洢Ԫ��״̬����ֵΪeStatusTypeö��ֵ��
	char* m_pStatusBuff;
	soint32 m_nCapacity;
	soint32 m_nUsingElementCount;
	soint32 m_nElementSize;
	soint32 m_nAddCount;
};
//----------------------------------------------------------------
inline soint32 SoArrayUID::GetCapacity() const
{
	return m_nCapacity;
}
//----------------------------------------------------------------
inline soint32 SoArrayUID::GetUsingElementCount() const
{
	return m_nUsingElementCount;
}
//----------------------------------------------------------------
#endif //_SoArrayUID_h_
//----------------------------------------------------------------
