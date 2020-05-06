//----------------------------------------------------------------
#include "GED3DModelBase.h"
//----------------------------------------------------------------
GED3DModelBase::GED3DModelBase()
:m_nModelID(-1)
,m_eModelType(GED3DModelType_Invalid)
{

}
//----------------------------------------------------------------
GED3DModelBase::~GED3DModelBase()
{

}
//----------------------------------------------------------------
bool GED3DModelBase::InitModel(void* pInitParam)
{
	return false;
}
//----------------------------------------------------------------
void GED3DModelBase::ClearModel()
{

}
//----------------------------------------------------------------
void GED3DModelBase::UpdateModel(float fDeltaTime)
{

}
//----------------------------------------------------------------
void GED3DModelBase::RenderModel(Model_Tansform_Info* mode_info, int instance_num)
{

}
//----------------------------------------------------------------
void GED3DModelBase::SetWorldMatrix(const XMFLOAT4X4* pMatWorld)
{

}
//----------------------------------------------------------------
