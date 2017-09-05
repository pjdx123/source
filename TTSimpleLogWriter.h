// the Header of TTSimpleLogWriter
// the header include lite Function class

#ifndef _TTSimpleLogWriter_H__
#define _TTSimpleLogWriter_H__

#include <locale.h>

namespace NSTTSLogWriter
{
	// Useful maco begin
#define chSTR2(x)	#x
#define chSTR(x)	chSTR2(x)
#define	chMSG(desc)	message(__FILE__ "(" chSTR(__LINE__) "):" #desc )
	// Useful maco end

	// CTTAutoLock Class
 	class CTTAutoLock
 	{
 	public:
 		explicit CTTAutoLock(CRITICAL_SECTION* pCriticalSection) : m_bLocked(TRUE)
 		{
 			if (pCriticalSection)
 			{
 				m_pCriticalSection = pCriticalSection;
 				::EnterCriticalSection(m_pCriticalSection);
 			}
 		}
 		
 		inline void Lock()
 		{
 			if (!m_bLocked)
 			{
 				m_bLocked = TRUE;
 				if (m_pCriticalSection)
 				{
 					::EnterCriticalSection(m_pCriticalSection);
 				}
 			}
 		}
 		
 		inline void Unlock()
 		{
 			if (m_bLocked)
 			{
 				m_bLocked = FALSE;
 				if (m_pCriticalSection)
 				{
 					::LeaveCriticalSection(m_pCriticalSection);
 				}
 			}
 		}
 
 		virtual ~CTTAutoLock()
 		{
 			if (m_bLocked)
 			{
 				m_bLocked = FALSE;
 				if (m_pCriticalSection)
 				{
 					::LeaveCriticalSection(m_pCriticalSection);
 					m_pCriticalSection = NULL;
 				}
 			}
 		}
 	private:
 		BOOL				m_bLocked;
 		CRITICAL_SECTION*	m_pCriticalSection;
 	};



	// CTTLogWriter Class
	// the class need to improve 
	class CTTLogWriter
	{
	public:
		CTTLogWriter()
		{
			InitializeCriticalSection( &m_csWriteFile );
		}

		CTTLogWriter( const CString &strFileName )
		{
			CreateLogFile( strFileName );
			InitializeCriticalSection( &m_csWriteFile );
		}

		virtual ~CTTLogWriter()
		{
			CloseLogFile();
			DeleteCriticalSection( &m_csWriteFile );
		}

		inline BOOL CreateLogFile( const CString &strName )
		{
			setlocale(LC_ALL,   "Chinese_China");

			CString strTmp = strName;
			CString strLogPath = _T("C:\\DebugLog");
			bool bOnlyFileName = (-1 == strName.Find( _T('\\') ));
			if ( !bOnlyFileName )
			{
				strLogPath = strName.Left( strName.ReverseFind(_T('\\')) );
				strTmp = strName.Mid( strName.ReverseFind(_T('\\')) + 1 );
			}

			if ( !PathFileExists( strLogPath ) )
				return FALSE;
		
			CString strFile = _T("");
			// Name Format 2014-09-26_xxxxxx.xxx
			CString strCurDate = _T("");
			CTime t = CTime::GetCurrentTime();
			strCurDate.Format( _T("%d-%02d-%02d"), t.GetYear(),t.GetMonth(), t.GetDay() );

			strFile = strLogPath + _T("\\") + strCurDate + _T("_") + strTmp;
			
			BOOL bRet = m_hLogFile.Open( strFile, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::shareDenyNone );
			return bRet;
		}

		inline void LogDirect( const CString &strLog )
		{
			CTTAutoLock autoLock( &m_csWriteFile );

			CString strCurTime = _T("");
			CTime t = CTime::GetCurrentTime();
			strCurTime.Format( _T("%d-%02d-%02d %02d:%02d:%02d"), t.GetYear(),t.GetMonth(), t.GetDay(), t.GetHour(), t.GetMinute(), t.GetSecond() );

			CString strWriteLog = _T("");
			strWriteLog.Format(_T("%s %s\n"), strCurTime, strLog );

			if ( m_hLogFile.m_hFile != CFile::hFileNull )
			{
				m_hLogFile.SeekToEnd();
				m_hLogFile.WriteString( strWriteLog );
			}
			else 
			{
				OutputDebugString( strWriteLog );
			}
		}

		inline void LogLine( const CString &strLog, int nLine )
		{
			CString strWriteLog = _T("");
			strWriteLog.Format(_T("%s, Line is %d"), strLog, nLine );
			LogDirect( strWriteLog );
		}

		inline void LogRetValue( const CString &strLog, int nRetValue , int nLine = 0 )
		{
			CString strWriteLog = _T("");
			strWriteLog.Format(_T("%s, RetValue is %d, Line is %d"), strLog, nRetValue, nLine );
			LogDirect( strWriteLog );
		}

		inline void LogTimeCost( const CString &strLog, DWORD dwCost )
		{
			CString strWriteLog = _T("");
			strWriteLog.Format(_T("%s, cost time %d"), strLog, dwCost );
			LogDirect( strWriteLog );
		}

		inline void CloseLogFile()
		{
			if ( m_hLogFile.m_hFile != CFile::hFileNull )
			{
				m_hLogFile.Close();
			}
			setlocale(LC_ALL, "");
		}
	private:
		CStdioFile m_hLogFile;
		CRITICAL_SECTION m_csWriteFile;
	};
}

#endif