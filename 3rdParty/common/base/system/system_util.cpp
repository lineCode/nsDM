/* -------------------------------------------------------------------------
//	FileName	：	system_util.cpp
//	Creator		：	yanbozhao
//	CreateTime	：	2015.07.14 11:06:00
//	Description	：
//
// -----------------------------------------------------------------------*/

#include "system_util.h"
#include "minfo/mid.h"
#include "base/string/string_util.h"
#include "base/encrypt/md5.h"
#include <process.h>
#include <assert.h>
#include <Shlwapi.h>
#include <ShlObj.h>
#include <stdlib.h>

#include <Nb30.h>
#include <iphlpapi.h>
// -------------------------------------------------------------------------


bool openUrlWithIE(const wchar_t* url) {
	if (!url || !(*url)) {
		return false;
	}

	wchar_t buf[MAX_PATH + 1] = { 0 };
	::SHGetSpecialFolderPathW(NULL, buf, CSIDL_PROGRAM_FILES, FALSE);
	buf[MAX_PATH] = '\0';
	std::wstring ie(buf);
	ie += L"\\Internet Explorer\\iexplore.exe";

	bool isIEPathExist = (GetFileAttributes(ie.c_str()) != INVALID_FILE_ATTRIBUTES);
	if (!isIEPathExist || (DWORD)::ShellExecuteW(NULL, L"open", ie.c_str(), url, NULL, SW_SHOW) <= 32) {
		return (DWORD)::ShellExecute(NULL, L"open", L"iexplore.exe", url, NULL, SW_SHOW) > 32;
	}
	return true;
}

static unsigned __stdcall openUrlThread(void* param) {
	assert(param != NULL);
	wchar_t* url = (wchar_t*)param;

	wchar_t pathBrowser[MAX_PATH] = {0};
	DWORD nLength = sizeof(wchar_t) * MAX_PATH;
	DWORD dwType = 0;
	SHGetValue(HKEY_CLASSES_ROOT, L"http\\shell\\open\\command", L"", &dwType, pathBrowser, &nLength);
	std::wstring str(pathBrowser);
	if (str.find(L"%1") != std::wstring::npos) {
		// 用shellExecuteEx
		SHELLEXECUTEINFO shelli;
		ZeroMemory(&shelli, sizeof(SHELLEXECUTEINFO));
		shelli.cbSize = sizeof(SHELLEXECUTEINFO);
		shelli.fMask = SEE_MASK_FLAG_NO_UI;
		shelli.lpVerb = L"open";
		shelli.lpFile = url;
		shelli.nShow = SW_SHOWDEFAULT;

		//先尝试使用默认浏览器打开，如果打开失败，则显式使用IE浏览器
		if (!ShellExecuteEx(&shelli)) {
			openUrlWithIE(url);
		}
	}
	else {
		str += L" ";
		str += url;

		// 后面带有 -nohome的，用createProcess (360修改IE的。）
		wchar_t buf[4096] = {0};;
		wcscpy_s(buf, _countof(buf), str.c_str());

		STARTUPINFO si = {0};
		si.cb = sizeof(si);
		PROCESS_INFORMATION pInfo = {0};
		if (!::CreateProcess(
			NULL, 
			buf, 
			NULL, NULL, NULL, 
			CREATE_NO_WINDOW, 
			NULL, NULL, 
			&si, 
			&pInfo)
			) {
			// 有些BT的电脑上http\\shell\\open不存在，所以这里强制使用IE打开
			openUrlWithIE(url);
		}

		::CloseHandle(pInfo.hProcess);
		::CloseHandle(pInfo.hThread);
	}

	delete[] url;
	return 0;
}


namespace base {
	namespace system_util {
		bool openUrl(const std::wstring& url) {
			if (url.empty()) {
				return false;
			}

			wchar_t* param = new wchar_t[url.length() + 1];
			wcscpy_s(param, url.length() + 1, url.c_str());
			HANDLE h = (HANDLE)_beginthreadex(NULL, 0, openUrlThread, param, CREATE_SUSPENDED, NULL);
			if (h == NULL) {
				delete[] param;
				return false;
			}

			::ResumeThread(h);
			::CloseHandle(h);
			return true;
		}

		

		std::string getMid()
		{
			CMid mid;
			return base::string_util::ToStringASCII(mid.GetMid(), CP_UTF8);
		}

		bool GetNtVersionNumbers(DWORD& dwMajorVer, DWORD& dwMinorVer,DWORD& dwBuildNumber)
		{
			bool bRet= false;
			HMODULE hModNtdll= NULL;
			if (hModNtdll= ::LoadLibraryW(L"ntdll.dll"))
			{
				typedef void (WINAPI *pfRTLGETNTVERSIONNUMBERS)(DWORD*,DWORD*, DWORD*);
				pfRTLGETNTVERSIONNUMBERS pfRtlGetNtVersionNumbers;
				pfRtlGetNtVersionNumbers = (pfRTLGETNTVERSIONNUMBERS)::GetProcAddress(hModNtdll, "RtlGetNtVersionNumbers");
				if (pfRtlGetNtVersionNumbers)
				{
					pfRtlGetNtVersionNumbers(&dwMajorVer, &dwMinorVer,&dwBuildNumber);
					dwBuildNumber&= 0x0ffff;
					bRet = true;
				}

				::FreeLibrary(hModNtdll);
				hModNtdll = NULL;
			}

			return bRet;
		}
		void SafeGetNativeSystemInfo( LPSYSTEM_INFO lpSystemInfo)
		{
			if (NULL == lpSystemInfo)
				return;
			typedef VOID (WINAPI *LPFN_GetNativeSystemInfo)(LPSYSTEM_INFO lpSystemInfo);
			LPFN_GetNativeSystemInfo nsInfo = 
				(LPFN_GetNativeSystemInfo)GetProcAddress(GetModuleHandle(L"kernel32"), "GetNativeSystemInfo");
			if (NULL != nsInfo)
			{
				nsInfo(lpSystemInfo);
			}
			else
			{
				GetSystemInfo(lpSystemInfo);
			}
		}

		bool Is64BitSystem()
		{
			bool bRet= false;
			HMODULE hModKernel32= NULL;
			if (hModKernel32= ::LoadLibraryW(L"kernel32.dll"))
			{
				typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
				LPFN_ISWOW64PROCESS fnIsWow64 = (LPFN_ISWOW64PROCESS)GetProcAddress(hModKernel32, "IsWow64Process");
				if (fnIsWow64)
				{
					BOOL bWow64 = FALSE;
					fnIsWow64(GetCurrentProcess(), &bWow64);
					bRet = bWow64?true:false;
				}
			}
			return bRet;
		}

		std::string encryptMid(const std::string& mid)
		{
			char src[33] = {0};
			memset(src, '0', _countof(src));
			_snprintf_s(src, _countof(src),  _TRUNCATE, "%s", mid.c_str());
			const unsigned int encrypt_count = 16;
			assert(encrypt_count < _countof(src));
			char encrypted_src[encrypt_count+1] = {0};
			
			char num_buf[2] = {0};
			for (unsigned int index = 0; index < encrypt_count; ++index)
			{
				num_buf[0] = src[32-encrypt_count+index];
				unsigned long num = strtoul(num_buf, NULL, 16);
				encrypted_src[index] = src[num] ^ num_buf[0];
			}

			std::string encrypted_str(encrypted_src, encrypt_count);
			const char* key = "yymachine";
			encrypted_str += key;

			std::string result;
			base::CMD5 md5_encoder(encrypted_str);
			return md5_encoder.toString();
			
			return result;
		}

	}
}

// -------------------------------------------------------------------------
