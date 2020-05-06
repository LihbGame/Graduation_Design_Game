//-----------------------------------------------------------------------------
//1，不支持Unicode字符集。
//2，log文件存储在与sln相同的目录下。
//   该log文件名的末尾会加上时间戳，精确到天，同一天内会把log写入同一个文件。
//3，线程安全。
//4，只能在主线程中对 GESimpleLog::ms_pInstance 执行操作，
//   这也意味着，下面这些函数只能在主线程中调用：
//   CreateSimpleLog(); ReleaseSimpleLog(); Get(); GELogDebug; GELogWarning; GELogError;
//   要想在子线程中使用GESimpleLog，必须为子线程创建一个GESimpleLog对象。
//--------------------------------------------------------------------
//去除编译warning
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif //_CRT_SECURE_NO_WARNINGS
//-----------------------------------------------------------------------------
#include "GESimpleLog.h"
#include "GETimeHelp.h"
#include "GEStringHelp.h"
#include <cstdio> //磁盘文件操作
#include <Windows.h> //为了使用不定参数va_list，和函数OutputDebugString
//-----------------------------------------------------------------------------
#define GESimpleLog_BuffSize 1024
//-----------------------------------------------------------------------------
GESimpleLog* GESimpleLog::ms_pInstance = 0;
//-----------------------------------------------------------------------------
bool GESimpleLog::CreateSimpleLog()
{
	bool br = true;
	if (ms_pInstance == 0)
	{
		ms_pInstance = new GESimpleLog;
		if (ms_pInstance && ms_pInstance->InitSimpleLog("Log", false, true))
		{
			//do nothing
		}
		else
		{
			ReleaseSimpleLog();
			br = false;
		}
	}
	return br;
}
//-----------------------------------------------------------------------------
void GESimpleLog::ReleaseSimpleLog()
{
	if (ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = 0;
	}
}
//-----------------------------------------------------------------------------
GESimpleLog::GESimpleLog()
:m_fp(0)
,m_pBuff(0)
,m_bOutputDebugString(true)
,m_bFlushImmediately(true)
{

}
//-----------------------------------------------------------------------------
GESimpleLog::~GESimpleLog()
{
	ClearSimpleLog();
}
//-----------------------------------------------------------------------------
bool GESimpleLog::InitSimpleLog(const char* szFileName, bool bOutputDebugString, bool bFlushImmediately)
{
	bool br = true;
	m_pBuff = (char*)malloc(GESimpleLog_BuffSize);
	if (m_pBuff == 0)
	{
		br = false;
	}
	if (br)
	{
		br = OpenDiskFile(szFileName);
	}
	m_bOutputDebugString = bOutputDebugString;
	m_bFlushImmediately = bFlushImmediately;
	return br;
}
//-----------------------------------------------------------------------------
void GESimpleLog::ClearSimpleLog()
{
	CloseDiskFile();
	if (m_pBuff)
	{
		free(m_pBuff);
		m_pBuff = 0;
	}
}
//-----------------------------------------------------------------------------
void GESimpleLog::OutputDebug(const char* pFormat, ...)
{
	if (pFormat == 0)
	{
		return;
	}
	//
	AddLogHead("DEBUG");
	//
	va_list kList;
	va_start(kList, pFormat);
	AddLogBody(pFormat, &kList);
	va_end(kList);
}
//-----------------------------------------------------------------------------
void GESimpleLog::OutputWarning(const char* pFormat, ...)
{
	if (pFormat == 0)
	{
		return;
	}
	//
	AddLogHead("WARNING");
	//
	va_list kList;
	va_start(kList, pFormat);
	AddLogBody(pFormat, &kList);
	va_end(kList);
}
//-----------------------------------------------------------------------------
void GESimpleLog::OutputError(const char* pFormat, ...)
{
	if (pFormat == 0)
	{
		return;
	}
	//
	AddLogHead("ERROR");
	//
	va_list kList;
	va_start(kList, pFormat);
	AddLogBody(pFormat, &kList);
	va_end(kList);
}
//-----------------------------------------------------------------------------
void GESimpleLog::AddLogHead(char* pType)
{
	stTimeTM _tm;
	GETimeHelp::GetLocalTimeTM_Millisecond(&_tm);
	GEStrFmtSelf(m_pBuff, GESimpleLog_BuffSize, "%02u:%02u:%02u:%03u [%s] ", _tm.nHour, _tm.nMinute, _tm.nSecond, _tm.nMillisecond, pType);
	if (m_fp)
	{
		int nLen = GEStrLen(m_pBuff);
		fwrite(m_pBuff, 1, nLen, (FILE*)m_fp);
	}
	if (m_bOutputDebugString)
	{
		OutputDebugStringA(m_pBuff);
	}
}
//-----------------------------------------------------------------------------
void GESimpleLog::AddLogBody(const char* pFormat, const void* pVaList)
{
	GEStrFmtSelfByVaList(m_pBuff, GESimpleLog_BuffSize, pFormat, pVaList);
	if (m_fp)
	{
		int nLen = GEStrLen(m_pBuff);
		if (nLen > GESimpleLog_BuffSize - 2)
		{
			nLen = GESimpleLog_BuffSize - 2;
		}
		//添加换行符
		m_pBuff[nLen] = '\n';
		++nLen;
		m_pBuff[nLen] = 0;
		//
		fwrite(m_pBuff, 1, nLen, (FILE*)m_fp);
		if (m_bFlushImmediately)
		{
			fflush((FILE*)m_fp);
		}
	}
	if (m_bOutputDebugString)
	{
		OutputDebugStringA(m_pBuff);
	}
}
//-----------------------------------------------------------------------------
bool GESimpleLog::OpenDiskFile(const char* szFileName)
{
	//获取当前时间
	stTimeTM _tm;
	GETimeHelp::GetLocalTimeTM(&_tm);

	//组装log文件名字
	const char* szPart = (szFileName != 0) ? szFileName : "";
	GEStrFmtSelf(m_pBuff, GESimpleLog_BuffSize, "%s_%04u%02u%02u.txt", szPart, _tm.nYear, _tm.nMonth, _tm.nDayInMonth);

	//打开文件
	m_fp = fopen(m_pBuff, "at+");
	if (m_fp == 0)
	{
		//打开文件失败。
		return false;
	}

	//写入第一行标志
	GEStrFmtSelf(m_pBuff, GESimpleLog_BuffSize, "========%04u-%02u-%02u %02u:%02u:%02u begin========\n", _tm.nYear, _tm.nMonth, _tm.nDayInMonth, _tm.nHour, _tm.nMinute, _tm.nSecond);
	const int nLen = GEStrLen(m_pBuff);
	fwrite(m_pBuff, 1, nLen, (FILE*)m_fp);

	return true;
}
//-----------------------------------------------------------------------------
void GESimpleLog::CloseDiskFile()
{
	if (m_fp)
	{
		//获取当前时间
		stTimeTM _tm;
		GETimeHelp::GetLocalTimeTM(&_tm);
		//写入末尾行标志
		GEStrFmtSelf(m_pBuff, GESimpleLog_BuffSize, "========%04u-%02u-%02u %02u:%02u:%02u end========\n", _tm.nYear, _tm.nMonth, _tm.nDayInMonth, _tm.nHour, _tm.nMinute, _tm.nSecond);
		const int nLen = GEStrLen(m_pBuff);
		fwrite(m_pBuff, 1, nLen, (FILE*)m_fp);
		//
		fflush((FILE*)m_fp);
		fclose((FILE*)m_fp);
		m_fp = 0;
	}
}
//-----------------------------------------------------------------------------
