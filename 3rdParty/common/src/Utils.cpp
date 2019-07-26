#include "stdafx.h"

#include "Utils.h"
#include <Shellapi.h>
#include <TCHAR.H>
#include <Windows.h>
#include <TlHelp32.h>
#include <algorithm>
#include <Psapi.h>
#include <Mstask.h>
#include <atlbase.h>
//#include <ctype.h>
#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "Version.lib")

namespace Utils
{
	std::wstring ansi_to_unicode(const char *const input, int cp, int cbMultiByte)
	{
		//wchar_t *Buffer;
		std::wstring Buffer;
		int BuffSize, Result;
		BuffSize = MultiByteToWideChar(cp, 0, input, cbMultiByte, NULL, 0);
		if(BuffSize > 0)
		{
			//Buffer = new wchar_t[BuffSize];
			Buffer.resize(BuffSize);
			Result = MultiByteToWideChar(cp, 0, input, cbMultiByte, const_cast<LPWSTR>(&(Buffer.data()[0])), BuffSize);
			return ((Result > 0) && (Result <= BuffSize)) ? Buffer : std::wstring();
		}
		return std::wstring();
	}

	std::string unicode_to_ansi(const wchar_t *const input, int cp, int cchWideChar)
	{
		//char *Buffer;
		std::string Buffer;
		int BuffSize, Result;
		BuffSize = WideCharToMultiByte(cp, 0, input, cchWideChar, NULL, 0, NULL, NULL);
		if(BuffSize > 0)
		{
			//Buffer = new char[BuffSize];
			Buffer.resize(BuffSize);
			Result = WideCharToMultiByte(cp, 0, input, cchWideChar, const_cast<LPSTR>(&(Buffer.data()[0])), BuffSize, NULL, NULL);
			return ((Result > 0) && (Result <= BuffSize)) ? Buffer : std::string();
		}
		return std::string();
	}

	void open_url(const wchar_t* url)
	{
		SHELLEXECUTEINFOW stShellExeInfo = { 0 };
		stShellExeInfo.cbSize = sizeof( SHELLEXECUTEINFO );
		stShellExeInfo.fMask  = SEE_MASK_FLAG_NO_UI;
		stShellExeInfo.lpVerb = L"open";
		stShellExeInfo.lpFile = url;
		stShellExeInfo.nShow = SW_SHOWNORMAL;
		if ( !::ShellExecuteExW( &stShellExeInfo )  )
		{
			stShellExeInfo.fMask  = SEE_MASK_DEFAULT;
			stShellExeInfo.lpFile = L"iexplore.exe";
			stShellExeInfo.lpParameters = url;
			::ShellExecuteExW( &stShellExeInfo );
		}
	}
	
	bool is_vista_or_later()
	{
		OSVERSIONINFO osvi;
		ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx(&osvi);
		return (osvi.dwMajorVersion >= 6);
	}

	bool read_file(const wchar_t* file_path, std::string& out)
	{
		FILE* fp = nullptr;
		
		if (0 != _tfopen_s(&fp, file_path, L"rb"))
		{
			return false;
		}

		bool result = false;
		do 
		{
			fseek(fp, 0, SEEK_END);
			auto file_size = ftell(fp);
			if (file_size <= 0)
			{
				break;
			}

			fseek(fp, 0, SEEK_SET);
			out.resize(file_size);
			auto readed_size = fread_s(const_cast<char*>(out.data()), out.size(), 1, file_size, fp);

			result = (static_cast<long>(readed_size) == file_size);
		} while (false);

		fclose(fp);
		return result;
	}

	std::vector<std::wstring>& SplitWide(wchar_t* source, const wchar_t* token, std::vector<std::wstring>& dsts) 
	{
		wchar_t* next_contens = NULL;

		wchar_t* contents = wcstok_s(source, token, &next_contens);
		while (contents)
		{
			dsts.push_back(contents);
			contents = wcstok_s(NULL, token, &next_contens);
		}
		return dsts;
	}

	std::vector<std::string>& SplitASCII(char* source, const char* token, std::vector<std::string>& dsts) 
	{
		char* next_contens = NULL;

		char* contents = strtok_s(source, token, &next_contens);
		while (contents)
		{
			dsts.push_back(contents);
			contents = strtok_s(NULL, token, &next_contens);
		}
		return dsts;
	}

	bool get_exe_version(const wchar_t* exe_path, std::wstring& file_ver, std::wstring& product_ver)
	{
		bool bRet = false;
		DWORD length = ::GetFileVersionInfoSize(exe_path, NULL);
		if (0 == length)
		{
			return false;
		}
		char* const buffer = new char[length + 1];
		do 
		{
			bool bSuccess = !!::GetFileVersionInfo(exe_path, NULL, length, buffer);
			if (false == bSuccess)
			{
				break;
			}

			struct LANGANDCODEPAGE
			{
				WORD wLanguage;
				WORD wCodePage;
			} *lpTranslate;

			UINT cbTranslate;
			bSuccess = !!::VerQueryValue(buffer, L"\\VarFileInfo\\Translation", (LPVOID*)&lpTranslate ,&cbTranslate);
			if (false == bSuccess) 
			{
				break;
			}

			wchar_t subBlock[100];
			swprintf_s(subBlock, 100, L"\\StringFileInfo\\%04x%04x\\FileVersion", lpTranslate->wLanguage, lpTranslate->wCodePage);

			wchar_t* verIndex = NULL;
			UINT versionLen = 0;
			bSuccess = !!::VerQueryValue(buffer, subBlock, (LPVOID*)&verIndex, &versionLen);
			if (false == bSuccess) 
			{
				break;
			}
			if (NULL == verIndex || 0 == versionLen) 
			{
				break;
			}
			file_ver = verIndex;

			_sntprintf_s(subBlock, _TRUNCATE, L"\\StringFileInfo\\%04x%04x\\ProductVersion", lpTranslate->wLanguage, lpTranslate->wCodePage);
			bSuccess = !!::VerQueryValue(buffer, subBlock, (LPVOID*)&verIndex, &versionLen);
			if (false == bSuccess)
			{
				break;
			}
			if (NULL == verIndex || 0 == versionLen) 
			{
				break;
			}
			product_ver = verIndex;

			bRet = true;
		} while (false);
		
		delete[] buffer;

		return bRet;
	}

	// 使用TerminateProcess结束进程
	bool killProcByTerminate(int pid, std::wstring path)
	{
		HANDLE hProcess = OpenProcess(/*PROCESS_ALL_ACCESS*/PROCESS_TERMINATE|PROCESS_QUERY_INFORMATION|PROCESS_VM_READ, FALSE, pid);
		//OpenProcess可能会失败，所以使用taskkill来强制结束进程
		if (hProcess == NULL)
		{
			return false;
		}

		BOOL bOkey = FALSE;
		//路径名为空时,直接根据pid结束该进程
		if (path.empty())
		{
			bOkey = TerminateProcess(hProcess, 0);
			if(bOkey)
			{
				WaitForSingleObject(hProcess, 1000);
				CloseHandle(hProcess);
				hProcess = NULL;
			}
		} 
		else
		{
			//根据进程句柄获取文件路径，然后再判断路径是否一致
			wchar_t buf[MAX_PATH] = {0};
			if(GetModuleFileNameExW(hProcess, NULL, buf, MAX_PATH) 
				&& (0 == _tcsicmp(path.c_str(), buf)))
			{
				bOkey = TerminateProcess(hProcess, 0);
				if(bOkey)
				{
					WaitForSingleObject(hProcess, 1000);
					CloseHandle(hProcess);
					hProcess = NULL;
				}
			}
		}

		return !!bOkey;
	}

	// 使用taskkill结束进程
	bool killProcByCmd(int pid)
	{
		bool result = false;
		wchar_t buf[128] = {0};
		swprintf_s(buf, L"taskkill /F /T /PID %d", pid);
		STARTUPINFO sinfo;
		PROCESS_INFORMATION pi;
		ZeroMemory(&sinfo, sizeof(sinfo));
		ZeroMemory(&pi, sizeof(pi));

		sinfo.cb = sizeof(sinfo);

		if (::CreateProcess(NULL, buf, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &sinfo, &pi))
		{
			if (WAIT_OBJECT_0 == ::WaitForSingleObject(pi.hProcess, 3000))
			{
				DWORD dwCode = 0;
				if (::GetExitCodeProcess(pi.hProcess, &dwCode)
					&& 0 == dwCode)
				{
					result = true;
				}
			}
			
			::CloseHandle(pi.hProcess);
			::CloseHandle(pi.hThread);
		}

		return result;
	}

	bool FindModule32ByPid(DWORD dwPID, std::wstring exePath)
	{
		HANDLE hModuleSnap = INVALID_HANDLE_VALUE; 
		MODULEENTRY32 me32 = {0};

		//  Take a snapshot of all modules in the specified process. 
		hModuleSnap = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, dwPID ); 
		if( hModuleSnap == INVALID_HANDLE_VALUE ) 
		{ 
			return false;
		} 

		//  Set the size of the structure before using it. 
		me32.dwSize = sizeof( MODULEENTRY32 ); 

		//  Retrieve information about the first module, 
		//  and exit if unsuccessful 
		if( !Module32First( hModuleSnap, &me32 ) ) 
		{ 
			CloseHandle( hModuleSnap );     // Must clean up the snapshot object! 
			return false;
		} 

		//  Now walk the module list of the process, 
		//  and display information about each module 
		bool result = false;
		BOOL isDir = ::PathIsDirectory(exePath.c_str());
		TCHAR path_buf[MAX_PATH] = { 0 };
		do 
		{
			if (isDir)
			{
				if (::PathRelativePathTo(path_buf, exePath.c_str(),
					FILE_ATTRIBUTE_DIRECTORY, me32.szExePath, FILE_ATTRIBUTE_DIRECTORY))
				{
					if (path_buf[0] == L'.' && path_buf[1] == L'\\' && ::PathIsRelative(path_buf))
					{
						result = true;
						break;
					}
				}
			} 
			else
			{
				if (_tcsicmp(exePath.c_str(), me32.szExePath) == 0)
				{
					result = true;
					break;
				}
			}

		} while( Module32Next( hModuleSnap, &me32 ) ); 

		//  Do not forget to clean up the snapshot object. 
		CloseHandle( hModuleSnap ); 

		return result;
	}

	std::vector<DWORD> FindProcessByName(std::wstring exeName, std::wstring exePath)
	{
		// Create toolhelp snapshot.
		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		PROCESSENTRY32W process;
		ZeroMemory(&process, sizeof(process));
		process.dwSize = sizeof(process);

		std::vector<DWORD> pids;
		// Walkthrough all processes.
		if (Process32FirstW(snapshot, &process))
		{
			do
			{
				if (0 == _tcsicmp(process.szExeFile, exeName.c_str()))
				{
					DWORD pid = process.th32ProcessID;
					if (!exePath.empty()
						&& !FindModule32ByPid(pid, exePath))
					{
						continue;
					}
					pids.push_back(pid);
				}
			} while (Process32NextW(snapshot, &process));
		}

		CloseHandle(snapshot);

		return pids;
	}
	//
	bool TerminateProcessByName(std::wstring exeName, std::wstring exePath)
	{
		bool result = false;

		std::vector<DWORD> pids = FindProcessByName(exeName, exePath);

		size_t size = pids.size();
		for(size_t i = 0; i < size; ++i)
		{
			result = killProcByTerminate(pids[i], exePath)
				|| killProcByCmd(pids[i]);
			if (!result)
			{
				return result;
			}
		}
		return result;
	}
	//根据进程名字结束进程
	bool terminate_process_by_name(const std::wstring& proc_name, const std::wstring& path)
	{
		if (proc_name.empty())
		{
			return false;
		}
		
		return TerminateProcessByName(proc_name.c_str(), path.c_str());
	}

	// 查找指定进程是否存在
	bool find_process_by_name(const std::wstring& proc_name, const std::wstring& path)
	{
		if (proc_name.empty())
		{
			return false;
		}
		return !FindProcessByName(proc_name, path).empty();
	}

	bool CreateSchedule(const ScheduleData* data)
	{
		HRESULT hResult = S_OK;
		bool result = false;

		HRESULT hInit = ::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
		if (hInit != S_OK // 初始化成功
			&& hInit != S_FALSE // 之前已经初始化
			&& hInit != RPC_E_CHANGED_MODE) // 当前初始化的COM模型与之前初始化的模型不匹配
		{
			return false;
		}

		ITaskScheduler* pTaskSche = NULL;
		ITask* pTask = NULL;
		ITaskTrigger* pTrigger = NULL;
		IPersistFile* pPersist = NULL;

		do
		{
			hResult = ::CoCreateInstance(CLSID_CTaskScheduler, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pTaskSche));
			if (FAILED(hResult))
			{
				break;
			}

			hResult = pTaskSche->NewWorkItem(data->taskName.c_str(), CLSID_CTask, IID_ITask, (IUnknown**)&pTask);
			if (FAILED(hResult))
			{
				hResult = pTaskSche->Activate(data->taskName.c_str(), IID_ITask, (IUnknown**)&pTask);
				if (FAILED(hResult))
				{
					break;
				}

				WORD trigger_count = 0;
				hResult = pTask->GetTriggerCount(&trigger_count);
				if (FAILED(hResult))
				{
					break;
				}

				bool bExit = false;
				for (WORD index = 0; index < trigger_count; ++index)
				{
					hResult = pTask->DeleteTrigger(0);
					if (FAILED(hResult))
					{
						bExit = true;
						break;
					}
				}
				if (bExit)
				{
					break;
				}
			}

			if (!(
				SUCCEEDED(pTask->SetComment(data->comment.c_str())) &&
				SUCCEEDED(pTask->SetWorkingDirectory(data->workdir.c_str())) &&
				SUCCEEDED(pTask->SetApplicationName(data->appName.c_str())) &&
				SUCCEEDED(pTask->SetParameters(data->runParam.c_str())) &&
				SUCCEEDED(pTask->SetFlags(TASK_FLAG_RUN_ONLY_IF_LOGGED_ON)) &&
				SUCCEEDED(pTask->SetAccountInformation(data->userName.c_str(), NULL))))
			{
				break;
			}

			WORD wTrigger = 0;
			hResult = pTask->CreateTrigger(&wTrigger, &pTrigger);
			if (FAILED(hResult))
			{
				break;
			}

			TASK_TRIGGER trigger;
			ZeroMemory(&trigger, sizeof(trigger));
			SYSTEMTIME SystemTime;
			LPSYSTEMTIME lpSystemTime = &SystemTime;
			::GetLocalTime(lpSystemTime);

			// Add code to set trigger structure?
			trigger.wBeginDay = lpSystemTime->wDay;                  // Required
			trigger.wBeginMonth = lpSystemTime->wMonth;                // Required
			trigger.wBeginYear = lpSystemTime->wYear;              // Required
			trigger.cbTriggerSize = sizeof(TASK_TRIGGER);
			if (lpSystemTime->wMinute == 59 && lpSystemTime->wHour == 23)
			{
				trigger.wStartHour = 0;
				trigger.wStartMinute = 0;
			}
			else if (lpSystemTime->wMinute == 59)
			{
				trigger.wStartHour = lpSystemTime->wHour + 1;
				trigger.wStartMinute = 0;
			}
			else
			{
				trigger.wStartHour = lpSystemTime->wHour;
				trigger.wStartMinute = lpSystemTime->wMinute + 1;
			}
			trigger.TriggerType = TASK_TIME_TRIGGER_DAILY;
			trigger.Type.Daily.DaysInterval = 1;
			trigger.MinutesDuration = 24 * 60;
			trigger.MinutesInterval = data->minutesInterval;

			hResult = pTrigger->SetTrigger(&trigger);
			if (FAILED(hResult))
			{
				break;
			}

			hResult = pTask->QueryInterface(IID_IPersistFile, (void**)&pPersist);
			if (FAILED(hResult))
			{
				break;
			}

			pPersist->Save(NULL, TRUE);
			if (FAILED(hResult))
			{
				break;
			}

			result = true;

		} while (false);

		if (pPersist)
		{
			pPersist->Release();
		}

		if (pTrigger)
		{
			pTrigger->Release();
		}

		if (pTask)
		{
			pTask->Release();
		}

		if (pTaskSche)
		{
			pTaskSche->Release();
		}

		if (hInit == S_OK
			|| hInit == S_FALSE)
		{
			CoUninitialize();
		}
		return result;
	}

	bool DetectSchedule(const std::wstring* taskName)
	{
		HRESULT hr = S_OK;
		HRESULT hInit = ::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
		if (hInit != S_OK // 初始化成功
			&& hInit != S_FALSE // 之前已经初始化
			&& hInit != RPC_E_CHANGED_MODE) // 当前初始化的COM模型与之前初始化的模型不匹配
		{
			return false;
		}

		do 
		{
			CComPtr<ITaskScheduler> pITS;
			CComPtr<ITask> pITask;

			if (SUCCEEDED(hr))
			{
				hr = pITS.CoCreateInstance(CLSID_CTaskScheduler,
					NULL,
					CLSCTX_INPROC_SERVER);
				if (FAILED(hr))
				{
					break;
				}
			}
			else
			{
				break;
			}

			hr = pITS->Activate(taskName->c_str(),
				IID_ITask,            // Interface identifier
				(IUnknown**)&pITask); // Address of task 
		} while (false);

		if (hInit == S_OK
			|| hInit == S_FALSE)
		{
			CoUninitialize();
		}
		// 0x80070002(COR_E_FILENOTFOUND)
		return (HRESULT)0x80070002 != hr;
	}

	bool UpdateSchedule(const ScheduleData* data)
	{
		return DeleteSchedule(&data->taskName)
			&& CreateSchedule(data);
	}

	bool DeleteSchedule(const std::wstring* taskName)
	{
		HRESULT hr = S_OK;
		HRESULT hInit = ::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
		if (hInit != S_OK // 初始化成功
			&& hInit != S_FALSE // 之前已经初始化
			&& hInit != RPC_E_CHANGED_MODE) // 当前初始化的COM模型与之前初始化的模型不匹配
		{
			return false;
		}
		do
		{
			CComPtr<ITaskScheduler> pITS;
			if (SUCCEEDED(hr))
			{
				hr = pITS.CoCreateInstance(CLSID_CTaskScheduler,
					NULL,
					CLSCTX_INPROC_SERVER);
				if (FAILED(hr))
				{
					break;
				}
			}
			else
			{
				return true;
			}
			pITS->Delete(taskName->c_str());
		} while (false);

		if (hInit == S_OK
			|| hInit == S_FALSE)
		{
			CoUninitialize();
		}

		return true;
	}

}