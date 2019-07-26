/* -------------------------------------------------------------------------
//	FileName	£º	system_util.h
//	Creator		£º	yanbozhao
//	CreateTime	£º	2015.07.14 11:06:00
//	Description	£º
//
// -----------------------------------------------------------------------*/
#ifndef __BASE_SYSTEM_UTIL_H__
#define __BASE_SYSTEM_UTIL_H__

#include <windows.h>
#include <string>
// -------------------------------------------------------------------------

namespace base {
	namespace system_util {
		bool openUrl(const std::wstring& url);
		std::string  getMid();

		bool GetNtVersionNumbers(DWORD& dwMajorVer, DWORD& dwMinorVer,DWORD& dwBuildNumber);
		void SafeGetNativeSystemInfo( LPSYSTEM_INFO lpSystemInfo);
		bool Is64BitSystem();

		std::string encryptMid(const std::string& mid);

	}
}

// -------------------------------------------------------------------------

#endif /* __BASE_SYSTEM_UTIL_H__ */