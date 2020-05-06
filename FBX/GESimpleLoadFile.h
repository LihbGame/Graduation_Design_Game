//----------------------------------------------------------------
#ifndef _GESimpleLoadFile_h_
#define _GESimpleLoadFile_h_
//----------------------------------------------------------------
class GESimpleLoadFile
{
public:
	GESimpleLoadFile();
	~GESimpleLoadFile();
	bool LoadFile(const char* szFileName, bool bTextMode);
	void Clear();
	bool IsLoadSuccess() const;
	char* GetData() const;
	int GetSize() const;

private:
	char* m_pData;
	int m_nSize;
};
//----------------------------------------------------------------
inline bool GESimpleLoadFile::IsLoadSuccess() const
{
	return (m_nSize != -1);
}
//----------------------------------------------------------------
inline char* GESimpleLoadFile::GetData() const
{
	return m_pData;
}
//----------------------------------------------------------------
inline int GESimpleLoadFile::GetSize() const
{
	return m_nSize;
}
//----------------------------------------------------------------
#endif
//----------------------------------------------------------------
