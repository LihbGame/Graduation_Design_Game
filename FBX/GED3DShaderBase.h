//----------------------------------------------------------------
#ifndef _GED3DShaderBase_h_
#define _GED3DShaderBase_h_
//----------------------------------------------------------------
#include "d3dUtil.h"
//----------------------------------------------------------------
enum GED3DShaderType
{
	GED3DShaderType_Invalid = -1,
	GED3DShaderType_Terrain,
	GED3DShaderType_ModelCube,
	GED3DShaderType_ModelFbx,
	GED3DShaderType_ModelKK,
	GED3DShaderType_ModelKKNoBone,
	GED3DShaderType_ShadowKK,
	GED3DShaderType_ShadowKKNoBone,
	GED3DShaderType_GGUI,
	GED3DShaderType_Max,
};
//----------------------------------------------------------------
class GED3DShaderBase
{
public:
	GED3DShaderBase();
	virtual ~GED3DShaderBase();
	//--pParam 是一个结构体指针。不同的shader需要不同的参数，对应着不同的结构体。
	virtual void ProcessRender(void* pParam);
	void SetType(GED3DShaderType eType);
	GED3DShaderType GetType() const;

protected:
	GED3DShaderType m_Type;
};
//----------------------------------------------------------------
inline void GED3DShaderBase::SetType(GED3DShaderType eType)
{
	m_Type = eType;
}
//----------------------------------------------------------------
inline GED3DShaderType GED3DShaderBase::GetType() const
{
	return m_Type;
}
//----------------------------------------------------------------
#endif //_GED3DShaderBase_h_
//----------------------------------------------------------------
