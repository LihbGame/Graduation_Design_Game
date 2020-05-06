//----------------------------------------------------------------
#ifndef _GED3DModelBase_h_
#define _GED3DModelBase_h_
//----------------------------------------------------------------
#include "d3dApp.h"
//----------------------------------------------------------------
enum GED3DModelType
{
	GED3DModelType_Invalid = -1,
	GED3DModelType_Cube,
	GED3DModelType_Max,
};
//----------------------------------------------------------------
class GED3DModelBase
{
public:
	GED3DModelBase();
	virtual ~GED3DModelBase();

	virtual bool InitModel(void* pInitParam);
	virtual void ClearModel();
	virtual void UpdateModel(float fDeltaTime);
	virtual void RenderModel(Model_Tansform_Info* mode_info, int instance_num);
	virtual void SetWorldMatrix(const XMFLOAT4X4* pMatWorld);

	void SetModelID(int nID);
	int GetModelID() const;
	void SetModelType(GED3DModelType eType);
	GED3DModelType GetModelType() const;

protected:
	int m_nModelID;
	GED3DModelType m_eModelType;
};
//----------------------------------------------------------------
inline void GED3DModelBase::SetModelID(int nID)
{
	m_nModelID = nID;
}
//----------------------------------------------------------------
inline int GED3DModelBase::GetModelID() const
{
	return m_nModelID;
}
//----------------------------------------------------------------
inline void GED3DModelBase::SetModelType(GED3DModelType eType)
{
	m_eModelType = eType;
}
//----------------------------------------------------------------
inline GED3DModelType GED3DModelBase::GetModelType() const
{
	return m_eModelType;
}
//----------------------------------------------------------------
#endif //_GED3DModelBase_h_
//----------------------------------------------------------------
