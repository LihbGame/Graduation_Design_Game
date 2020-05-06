//-----------------------------------------------------------------------------
#ifndef _GEBitFlag_h_
#define _GEBitFlag_h_
//-----------------------------------------------------------------------------
//for example
//enum eDisableReasonForShortcut
//{
//	DisableReasonForShortcut_1 = 0x00000001,
//	DisableReasonForShortcut_2 = 0x00000002,
//	DisableReasonForShortcut_3 = 0x00000004,
//	DisableReasonForShortcut_4 = 0x00000008,
//	DisableReasonForShortcut_5 = 0x00000010,
//	DisableReasonForShortcut_6 = 0x00000020,
//};
//-----------------------------------------------------------------------------
class GEBitFlag
{
public:
	GEBitFlag();

	void AddFlag(unsigned int theFlag);
	void RemoveFlag(unsigned int theFlag);
	void Clear();

	bool IsZero() const;
	bool IsFlagExist(unsigned int theFlag) const;

	void SetValue(unsigned int uiValue);
	unsigned int GetValue() const;

private:
	unsigned int m_uiValue;
};
//-----------------------------------------------------------------------------
inline GEBitFlag::GEBitFlag()
{
	m_uiValue = 0;
}
//-----------------------------------------------------------------------------
inline void GEBitFlag::AddFlag(unsigned int theFlag)
{
	m_uiValue = m_uiValue | theFlag;
}
//-----------------------------------------------------------------------------
inline void GEBitFlag::RemoveFlag(unsigned int theFlag)
{
	m_uiValue = m_uiValue & (~theFlag);
}
//-----------------------------------------------------------------------------
inline void GEBitFlag::Clear()
{
	m_uiValue = 0;
}
//-----------------------------------------------------------------------------
inline bool GEBitFlag::IsZero() const
{
	return (m_uiValue == 0);
}
//-----------------------------------------------------------------------------
inline bool GEBitFlag::IsFlagExist(unsigned int theFlag) const
{
	return (m_uiValue & theFlag) != 0;
}
//-----------------------------------------------------------------------------
inline void GEBitFlag::SetValue(unsigned int uiValue)
{
	m_uiValue = uiValue;
}
//-----------------------------------------------------------------------------
inline unsigned int GEBitFlag::GetValue() const
{
	return m_uiValue;
}
//-----------------------------------------------------------------------------
#endif //_GEBitFlag_h_
//-----------------------------------------------------------------------------
