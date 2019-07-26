/* -------------------------------------------------------------------------
//	FileName	£º	process.cpp
//	Creator		£º	yanbozhao
//	CreateTime	£º	2015.06.01 11:06:00
//	Description	£º
//
// -----------------------------------------------------------------------*/

#include "process.h"
#include <string>
// -------------------------------------------------------------------------

namespace base {
	namespace process {

		bool LaunchProcess(LPWSTR cmdLine, long* procId /*= NULL*/) {
			STARTUPINFO si;
			ZeroMemory(&si, sizeof(si));
			si.cb = sizeof(si);

			PROCESS_INFORMATION pi;
			ZeroMemory(&pi, sizeof(pi));

			// Start the child process. 
			if (!CreateProcess(NULL,   // No module name (use command line)
				cmdLine,         // Command line
				NULL,            // Process handle not inheritable
				NULL,            // Thread handle not inheritable
				FALSE,           // Set handle inheritance to FALSE
				0,               // No creation flags
				NULL,            // Use parent's environment block
				NULL,            // Use parent's starting directory 
				&si,             // Pointer to STARTUPINFO structure
				&pi)             // Pointer to PROCESS_INFORMATION structure
				) {
				return false;
			}

			*procId = pi.dwProcessId;

			// Wait until child process exits.(MSDN???)
			// WaitForSingleObject(pi.hProcess, INFINITE);

			// Close process and thread handles. 
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);

			return true;
		}

		bool LaunchProcess(LPCWSTR appName, LPWSTR cmdLine, long* procId /*= NULL*/) {
			std::wstring params = L"\"";
			params.append(appName);
			params.append(L"\"");

			params.append(L" \"");
			params.append(cmdLine);
			params.append(L"\"");

			int size = params.size();
			wchar_t* tempParams = new wchar_t[size + 1];
			wcscpy_s(tempParams, size + 1, params.c_str());

			bool bRet = LaunchProcess(tempParams, procId);

			if (tempParams) {
				delete[] tempParams;
				tempParams = NULL;
			}

			return bRet;
		}
	}
}

// -------------------------------------------------------------------------
