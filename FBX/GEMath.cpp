//----------------------------------------------------------------
#include "GEMath.h"
#include <string> //for memset
//----------------------------------------------------------------
const GEMathFloat2 GEMathFloat2_Zero;
const GEMathFloat3 GEMathFloat3_Zero;
//----------------------------------------------------------------
void GEMath_VectorNormalize(GEMathFloat3* pV)
{
	float f1 = (pV->x)*(pV->x) + (pV->y)*(pV->y) + (pV->z)*(pV->z);
	float f2;
	GEMath_InvSqrt(f1, f2);
	pV->x *= f2;
	pV->y *= f2;
	pV->z *= f2;
}
//----------------------------------------------------------------
float GEMath_VectorDot(const GEMathFloat3* pV1, const GEMathFloat3* pV2)
{
	return (pV1->x*pV2->x + pV1->y*pV2->y + pV1->z*pV2->z);
}
//----------------------------------------------------------------
void GEMath_VectorCross(const GEMathFloat3* pV1, const GEMathFloat3* pV2, GEMathFloat3* pResult, bool bNormalize)
{
	float fResultX = pV1->y*pV2->z - pV1->z*pV2->y;
	float fResultY = pV1->z*pV2->x - pV1->x*pV2->z;
	float fResultZ = pV1->x*pV2->y - pV1->y*pV2->x;
	if (bNormalize)
	{
		float f1 = fResultX*fResultX + fResultY*fResultY + fResultZ*fResultZ;
		float f2;
		GEMath_InvSqrt(f1, f2);
		fResultX *= f2;
		fResultY *= f2;
		fResultZ *= f2;
	}
	pResult->x = fResultX;
	pResult->y = fResultY;
	pResult->z = fResultZ;
}
//----------------------------------------------------------------
const GEMathMatrix4 GEMathMatrix4_Identity(true);
//----------------------------------------------------------------
GEMathMatrix4::GEMathMatrix4()
{
	memset(this, 0, sizeof(GEMathMatrix4));
}
//----------------------------------------------------------------
GEMathMatrix4::GEMathMatrix4(bool bIdentity)
{
	memset(this, 0, sizeof(GEMathMatrix4));
	if (bIdentity)
	{
		element[0][0] = 1.0f;
		element[1][1] = 1.0f;
		element[2][2] = 1.0f;
		element[3][3] = 1.0f;
	}
}
//----------------------------------------------------------------
void GEMath_MatrixIdentity(GEMathMatrix4* pMat)
{
	memset(pMat, 0, sizeof(GEMathMatrix4));
	pMat->element[0][0] = 1.0f;
	pMat->element[1][1] = 1.0f;
	pMat->element[2][2] = 1.0f;
	pMat->element[3][3] = 1.0f;
}
//----------------------------------------------------------------
void GEMath_QuaternionRotate(const GEMathFloat3* pAxis, float fRadian, const GEMathFloat3* pSrcVector, GEMathFloat3* pResultVector, bool bNormalize)
{
	//Q = w + xi + yj + zk
	const float fHalfRadian = fRadian * 0.5f;
	float fCos, fSin, fCosT, fSinT;
	GEMath_Cos(fHalfRadian, fCos);
	GEMath_Sin(fHalfRadian, fSin);
	fCosT = fCos; //GEMath_Cos(-fHalfRadian, fCosT);
	fSinT = -fSin; //GEMath_Sin(-fHalfRadian, fSinT);
	//QuaA
	float fQuaA_w = fCos;
	float fQuaA_x = fSin * pAxis->x;
	float fQuaA_y = fSin * pAxis->y;
	float fQuaA_z = fSin * pAxis->z;
	//QuaB
	float fQuaB_w = fCosT;
	float fQuaB_x = fSinT * pAxis->x;
	float fQuaB_y = fSinT * pAxis->y;
	float fQuaB_z = fSinT * pAxis->z;
	//
	//QuaP = (0.0f, Src.x, Src.y, Src.z)
	//
	//QuaB * QuaP
	float fQuaBP_w = - fQuaB_x*pSrcVector->x - fQuaB_y*pSrcVector->y - fQuaB_z*pSrcVector->z;
	float fQuaBP_x =   fQuaB_w*pSrcVector->x + fQuaB_y*pSrcVector->z - fQuaB_z*pSrcVector->y;
	float fQuaBP_y =   fQuaB_w*pSrcVector->y - fQuaB_x*pSrcVector->z + fQuaB_z*pSrcVector->x;
	float fQuaBP_z =   fQuaB_w*pSrcVector->z + fQuaB_x*pSrcVector->y - fQuaB_y*pSrcVector->x;
	//
	//quaBP * QuaA
	float fResultX = fQuaBP_w*fQuaA_x + fQuaBP_x*fQuaA_w + fQuaBP_y*fQuaA_z - fQuaBP_z*fQuaA_y;
	float fResultY = fQuaBP_w*fQuaA_y - fQuaBP_x*fQuaA_z + fQuaBP_y*fQuaA_w + fQuaBP_z*fQuaA_x;
	float fResultZ = fQuaBP_w*fQuaA_z + fQuaBP_x*fQuaA_y - fQuaBP_y*fQuaA_x + fQuaBP_z*fQuaA_w;
	//
	if (bNormalize)
	{
		float f1 = fResultX*fResultX + fResultY*fResultY + fResultZ*fResultZ;
		float f2;
		GEMath_InvSqrt(f1, f2);
		fResultX *= f2;
		fResultY *= f2;
		fResultZ *= f2;
	}
	//
	pResultVector->x = fResultX;
	pResultVector->y = fResultY;
	pResultVector->z = fResultZ;
}
//----------------------------------------------------------------
