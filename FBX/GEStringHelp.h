//--------------------------------------------------------------------
#ifndef _GEStringHelp_h_
#define _GEStringHelp_h_
//--------------------------------------------------------------------
#include <cstdlib>
//--------------------------------------------------------------------
#define GEStringHelp_GlobalBuffSize 2048
//--------------------------------------------------------------------
//�����ַ������ȡ�
//�ַ������Ȳ��ܴ���0x3FFFFFFF��1G�ࣩ��
//�����������ֵ��Ŀ���ǣ���ֹ�û�������һ��û����0��β���ַ�����������ѭ����
int GEStrLen(const char* szString);
int GEWStrLen(const wchar_t* szString);

//�ַ���������
//�����ַ������ȡ�
int GEStrCpy(char* DestBuff, const int nDestBuffSize, const char* szSourceString);
int GEWStrCpy(wchar_t* DestBuff, const int nDestBuffSize, const wchar_t* szSourceString);

//�ַ�����ʽ����ʹ�ñ�ģ���ṩ���ַ������档
//����ֵ����const char*���ǿ����޸ĵ��ַ������棬�����������Զ��ַ�����һЩ�޸ġ�
char* GEStrFmt(const char* szFormat, ...);
//�ַ�����ʽ����ʹ��ָ�����ַ�������
void GEStrFmtSelf(char* Buff, const int nBuffSize, const char* szFormat, ...);
char* GEStrFmtByVaList(const char* szFormat, const void* pValist);
void GEStrFmtSelfByVaList(char* Buff, const int nBuffSize, const char* szFormat, const void* pValist);

//�Ƚ������ַ���
int GEStrCmp(const char* s1, const char* s2);
int GEWStrCmp(const wchar_t* s1, const wchar_t* s2);
//�Ƚ������ַ����������Ǵ�Сд����
int GEStrCmpNoCase(const char* s1, const char* s2);
int GEWStrCmpNoCase(const wchar_t* s1, const wchar_t* s2);

//�����ַ�c���ַ���str���״γ��ֵ�λ����š�
//�����Դ����ַ���str�ĳ��ȣ����������Ļ��������ڲ�������ַ������ȡ�
//����ҵĵ�������ֵ��Χ[0, GEStrLen(str))��
//����Ҳ���������ֵΪ-1��
int GEStrChr(const char* str, const char c, int nStrLen = -1);
int GEWStrChr(const wchar_t* str, const wchar_t c, int nStrLen = -1);
//�����ַ�c���ַ���str��ĩ�γ��ֵ�λ����š�
//�����Դ����ַ���str�ĳ��ȣ����������Ļ��������ڲ�������ַ������ȡ�
//����ҵĵ�������ֵ��Χ[0, GEStrLen(str))��
//����Ҳ���������ֵΪ-1��
int GEStrRChr(const char* str, const char c, int nStrLen = -1);
int GEWStrRChr(const wchar_t* str, const wchar_t c, int nStrLen = -1);

//��str�л�ȡһ�����ַ�����
//���ַ��������������ǣ�λ���������[nStartIndex, nEndIndex]�ڵ��ַ���
char* GESubStr(const char* str, int nStartIndex, int nEndIndex);

//�ַ���������ͷ����β���Ŀո��tab�ַ�ȥ����
char* GEStrSlim(const char* str, int nStrLen = -1);

//���ַ����е��ַ��滻�ɴ�д��ĸ
char* GEStrToUpper(const char* szString);
void GEStrToUpperSelf(char* szString);

//���ַ����е��ַ��滻��Сд��ĸ
char* GEStrToLower(const char* szString);
void GEStrToLowerSelf(char* szString);

//·���ַ�����׼��
//�Ѵ����·���ַ����޸ĳ����¸�ʽ��
//1��ȥ��·��ͷ����'.'��'/'��'\\'���ո�tab�ַ���
//   ȥ��·��β����'.'���ո�tab�ַ���
//2����'\\'�޸ĳ�'/'��
//3���Ѵ�д��ĸ�޸ĳ�Сд��ĸ��
//--bXieGang ֵΪtrue����ʾ�ַ���ĩβ������'/'��ֵΪfalse��ʾ����Ҫ��
//����ֵ��һ��ȫ�ֵ��ַ������飬���Ҫ���콫����ֵȡ�ߡ�
char* GEPathNormalize(const char* szPath, bool bXieGang);
//����Ҫȥ��ͷ���ַ�����ĩβ����ַ������Բ���ֱ�Ӷ�Դ�ַ������޸ģ����Բ��ṩ�������������
//void GEPathNormalizeSelf(char* szString);


//��ȡutf8�ַ����е�һ���ַ���
//--pByteCountOfTheCharacter ���Ϊ��Чֵ�������˳���Я�����ַ����ֽڸ�����
char* GEGetOneCharacterFromUtf8String(const char* pszUtf8Str, const int nUtf8StrLen, const int nStartIndex, int* pByteCountOfTheCharacter);
//�ж�utf8�ַ����Ƿ���Ϲ淶��
bool GECheckUtf8WellFormed(const unsigned char *str, int len);

// Same to ::atof, but strip the string, remain 7 numbers after '.' before call atof.
// Why we need this? Because in android c++_static, atof ( and std::atof ) is unsupported for numbers
// have long decimal part and contain several numbers can approximate to 1 ( like 90.099998474121094 ), 
// it will return inf. This function is used to fix this bug.
double GEatof(const char* szString);
double GEWatof(const wchar_t* szString);
// Same to atoi
#define GEatoi atoi
#define GEWatoi _wtoi
#define GEstrtol strtol
#define GEWstrtol wcstol
#define GEstrtoul strtoul
#define GEWstrtoul wcstoul
//--------------------------------------------------------------------
#endif //_GEStringHelp_h_
//--------------------------------------------------------------------
