/* -------------------------------------------------------------------------
//	FileName	��	process.h
//	Creator		��	yanbozhao
//	CreateTime	��	2015.06.01 11:06:00
//	Description	��
//
// -----------------------------------------------------------------------*/
#ifndef __BASE_PROCESS_H__
#define __BASE_PROCESS_H__

#include <windows.h>
// -------------------------------------------------------------------------

namespace base {
	namespace process {
		bool LaunchProcess(LPWSTR cmdLine, long* procId = NULL);
		bool LaunchProcess(LPCWSTR appName, LPWSTR cmdLine, long* procId = NULL);
	}
}

// -------------------------------------------------------------------------

#endif /* __BASE_PROCESS_H__ */