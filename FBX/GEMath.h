//----------------------------------------------------------------
#ifndef _GEMath_h_
#define _GEMath_h_
//----------------------------------------------------------------
#include <math.h>
//----------------------------------------------------------------
#define GEMath_float_zero_critical 0.001f
#define GEMath_FloatMax FLT_MAX
#define GEMath_FloatMin FLT_MIN
#define GEMath_IsFloatZero(fValue) ((-GEMath_float_zero_critical < (fValue) && (fValue) < GEMath_float_zero_critical) ? true : false)
#define GEMath_IsFloatZero2(fValue, fCritical) ((-fCritical < (fValue) && (fValue) < fCritical) ? true : false)
//----------------------------------------------------------------
#define GEMath_PI 3.14159265359f
#define GEMath_halfPI 1.57079632679f
#define GEMath_2PI 6.28318530718f
#define GEMath_1overPI 0.31830988618f
//GEMath_SinParam1 = 4 / pi
#define GEMath_SinParam1 1.27323954474f
//GEMath_SinParam2 = 4 /pi / pi
#define GEMath_SinParam2 0.40528473456f
//----------------------------------------------------------------
#define GEMath_Sin1(fRadian, fResult) {fResult = sin(fRadian);}
//GEMath_Sin2 is low precision
#define GEMath_Sin2(fRadian, fResult) {fResult=(fRadian>0.0f)?(GEMath_SinParam1*fRadian-GEMath_SinParam2*fRadian*fRadian):(GEMath_SinParam1*fRadian+GEMath_SinParam2*fRadian*fRadian);}
//GEMath_Sin3 is high precision
#define GEMath_Sin3(fRadian, fResult) {GEMath_Sin2(fRadian, fResult); fResult=(fResult>0.0f)?(0.225f*(fResult*fResult-fResult)+fResult):(0.225f*(fResult*-fResult-fResult)+fResult);}
#define GEMath_Cos1(fRadian, fResult) {fResult = cos(fRadian);}
//GEMath_Cos2 is low precision
#define GEMath_Cos2(fRadian, fResult) {float ft=fRadian; ft+=GEMath_halfPI; if(ft>GEMath_PI){ft-=GEMath_2PI;} GEMath_Sin2(ft, fResult);}
//GEMath_Cos3 is high precision
#define GEMath_Cos3(fRadian, fResult) {float ft=fRadian; ft+=GEMath_halfPI; if(ft>GEMath_PI){ft-=GEMath_2PI;} GEMath_Sin3(ft, fResult);}
//----------------------------------------------------------------
#define GEMath_Sin GEMath_Sin3
#define GEMath_Cos GEMath_Cos3
//----------------------------------------------------------------
union GEMath_SqrtUnionParam
{
	int intPart;
	float floatPart;
};
#define GEMath_Sqrt1(fValue, fResult) {fResult = sqrt(fValue);}
//GEMath_Sqrt2 is only for Windows platform
#define GEMath_Sqrt2(fValue, fResult) {GEMath_SqrtUnionParam kU1, kU2; kU1.floatPart=fValue; kU2.floatPart=fValue; kU1.intPart=0x1FBCF800+(kU1.intPart>>1); kU2.intPart=0x5f375a86-(kU2.intPart>>1); fResult=0.5f*(kU1.floatPart+fValue*kU2.floatPart);} //0x5f3759df
#define GEMath_InvSqrt1(fValue, fResult) {fResult = 1.0f / sqrt(fValue);}
//GEMath_InvSqrt2 is only for Windows platform
#define GEMath_InvSqrt2(fValue, fResult) {float ft=fValue; int ni=*(int*)&ft; ni=0x5f375a86-(ni>>1); ft=*(float*)&ni; fResult=ft*(1.5f-0.5f*fValue*ft*ft);}
//----------------------------------------------------------------
#define GEMath_Sqrt GEMath_Sqrt2
#define GEMath_InvSqrt GEMath_InvSqrt2
//----------------------------------------------------------------
struct GEMathFloat2
{
	float x;
	float y;

	GEMathFloat2() : x(0.0f), y(0.0f) {};
	GEMathFloat2(float _x, float _y) : x(_x), y(_y) {};
};
extern const GEMathFloat2 GEMathFloat2_Zero;
//----------------------------------------------------------------
struct GEMathFloat3
{
	float x;
	float y;
	float z;

	GEMathFloat3() : x(0.0f), y(0.0f), z(0.0f) {};
	GEMathFloat3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {};
};
extern const GEMathFloat3 GEMathFloat3_Zero;
//----------------------------------------------------------------
void GEMath_VectorNormalize(GEMathFloat3* pV);
float GEMath_VectorDot(const GEMathFloat3* pV1, const GEMathFloat3* pV2);
void GEMath_VectorCross(const GEMathFloat3* pV1, const GEMathFloat3* pV2, GEMathFloat3* pResult, bool bNormalize);
//----------------------------------------------------------------
struct GEMathMatrix3
{
	float element[3][3];
};
//----------------------------------------------------------------
struct GEMathMatrix4
{
	float element[4][4];

	GEMathMatrix4();
	GEMathMatrix4(bool bIdentity);
};
extern const GEMathMatrix4 GEMathMatrix4_Identity;
//----------------------------------------------------------------
void GEMath_MatrixIdentity(GEMathMatrix4* pMat);
//----------------------------------------------------------------
//Quaternion = w + xi + yj + zk
struct GEMathQuaternion
{
	float x;
	float y;
	float z;
	float w;
};
//----------------------------------------------------------------
void GEMath_QuaternionRotate(const GEMathFloat3* pAxis, float fRadian, const GEMathFloat3* pSrcVector, GEMathFloat3* pResultVector, bool bNormalize);
//----------------------------------------------------------------
#endif //_GEMath_h_
//----------------------------------------------------------------
