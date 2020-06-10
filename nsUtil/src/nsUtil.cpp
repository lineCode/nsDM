// nsUtil.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"

#include "nsis/pluginapi.h"
#include "nsDefine.h"
#include "base/system/system_util.h"
#include "base/string/string_util.h"
#include "base/encrypt/UrlEncode.h"
#include "Utils.h"
#include "VCDetector.h"

#include <Rpc.h>
#include <cassert>
#include <algorithm>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
//External
HINSTANCE g_hInstance = NULL;
extra_parameters* g_pluginParms = nullptr;

//Global
static volatile bool g_bCallbackRegistred = false;

BOOL APIENTRY DllMain( HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		g_hInstance = hModule;
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

static UINT_PTR PluginCallback(enum NSPIM msg)
{
	switch(msg)
	{
	case NSPIM_UNLOAD:
	case NSPIM_GUIUNLOAD:
		break;
	default:
		OutputDebugStringA("StdUtils: Unknown callback message. Take care!\n");
		break;
	}

	return 0;
}

NSISFUNC(NSGetMachineId)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);

	static std::string str_mid = base::system_util::getMid();
	char mid[64] = {0};
	strcpy_s(mid, str_mid.c_str());

	PushStringA(mid);
}

NSISFUNC(NSCreateGuid)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);

	UUID uid;
	::UuidCreate(&uid);
	
	RPC_WSTR str_uid;
	::UuidToString(&uid, &str_uid);
	pushstring(reinterpret_cast<TCHAR*>(str_uid));
	::RpcStringFree(&str_uid);
}

static std::wstring CreateIve(const std::wstring& file, const std::wstring& product)
{
	// major.minor.revision.build.yyyyMMdd.custom_num
	std::wstring ive;
	ive = product;
	std::replace(ive.begin(), ive.end(), L' ', L'');
	std::replace(ive.begin(), ive.end(), L',', L'.');

	std::vector<std::wstring> dsts;
	std::wstring file_ver = file;
	std::replace(file_ver.begin(), file_ver.end(), L' ', L'');
	std::replace(file_ver.begin(), file_ver.end(), L',', L'.');

	wchar_t* split_src = new wchar_t[file_ver.length()+1];
	wcscpy_s(split_src, file_ver.length()+1, file_ver.c_str());
	base::string_util::SplitWide(split_src, L".", dsts);
	
	if (dsts.size() == 4)
	{
		ive.append(L".");
		for (size_t i = 0; i < 3; ++i)
		{
			ive.append(dsts[i].c_str());
		}
		ive.append(L".");
		ive.append(dsts[3]);
	}
	delete[] split_src;

	return ive;
}

NSISFUNC(NSGetIve)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);

	TCHAR exe_path[MAX_PATH] = {0};
	popstringn(exe_path, sizeof(exe_path));

	std::wstring file, product;
	if (Utils::get_exe_version(exe_path, file, product))
	{
		pushstring(const_cast<TCHAR*>(CreateIve(file, product).c_str()));
	}
	else
	{
		assert(false);
	}
}

NSISFUNC(NSIsVC2008Installed_x86)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);

	pushint(IsVC2008Installed_x86());
}

NSISFUNC(NSIsVC2008SP1Installed_x86)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);

	pushint(IsVC2008SP1Installed_x86());
}

NSISFUNC(NSIsVC2010Installed_x86)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);

	pushint(IsVC2010Installed_x86());
}

NSISFUNC(NSIsVC2010SP1Installed_x86)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);

	pushint(IsVC2010SP1Installed_x86());
}

NSISFUNC(NSUrlEncode)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);

	TCHAR data[1024] = {0};
	popstringn(data, sizeof(data));

	const std::string& result = base::CUrlEncode::EscapeURL(data);
	PushStringA(result.c_str());
}

NSISFUNC(NSUrlDecode)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);

	TCHAR data[1024] = {0};
	popstringn(data, sizeof(data));
	
	pushstring(base::CUrlEncode::DescapeURL(Utils::unicode_to_ansi(data).c_str()).c_str());
}

NSISFUNC(NSKillProcess)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);

	TCHAR exeName[MAX_PATH] = {0};
	TCHAR filePath[MAX_PATH] = {0};
	popstringn(exeName, sizeof(exeName));
	popstringn(filePath, sizeof(filePath));

	std::wstring szExeName = std::wstring(exeName);
	std::wstring szFilePath = std::wstring(filePath);
	bool bsuccess = Utils::terminate_process_by_name(szExeName, szFilePath);
	pushint(bsuccess);
}

NSISFUNC(NSFindProcess)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);

	TCHAR exeName[MAX_PATH] = {0};
	TCHAR filePath[MAX_PATH] = {0};
	popstringn(exeName, sizeof(exeName));
	popstringn(filePath, sizeof(filePath));

	std::wstring szExeName = std::wstring(exeName);
	std::wstring szFilePath = std::wstring(filePath);
	bool bsuccess = Utils::find_process_by_name(szExeName, szFilePath);
	pushint(bsuccess);
}

std::wstring process_path(const std::wstring& filePath, const std::wstring& extName)
{
	//如果路径不存在，直接返回
	if (!::PathFileExists(filePath.c_str()))
	{
		return filePath;
	} 
	else
	{
		//是否是空文件夹
		if (::PathIsDirectoryEmptyW(filePath.c_str()))
		{
			//为空，则返回当前路径
			return filePath;
		} 
		else
		{
			//不为空
			wchar_t szTmp [MAX_PATH] = {0};
			wcscpy_s(szTmp, filePath.c_str());

			::PathAppendW(szTmp, extName.c_str());
			return process_path(szTmp, extName);
		}
	}
}

NSISFUNC(NSCreateSchedule)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);

	TCHAR taskName[128] = {0};
	TCHAR comment[512] = {0};
	TCHAR workDir[MAX_PATH] = {0};
	TCHAR appName[128] = {0};
	TCHAR runParam[128] = {0};
	TCHAR userName[128] = {0};
	unsigned int minInterval = 60;
	popstringn(taskName, sizeof(taskName));
	popstringn(comment, sizeof(comment));
	popstringn(workDir, sizeof(workDir));
	popstringn(appName, sizeof(appName));
	popstringn(runParam, sizeof(runParam));
	//popstringn(userName, sizeof(userName));
	DWORD dwLen = _countof(userName);
	::GetUserName(userName, &dwLen);
	minInterval = static_cast<unsigned int>(popint());

	Utils::ScheduleData data;
	data.taskName = taskName;
	data.comment = comment;
	data.workdir = workDir;
	data.appName = appName;
	data.runParam = runParam;
	data.userName = userName;
	data.minutesInterval = minInterval;
	
	bool bsuccess = Utils::CreateSchedule(&data);
	pushint(bsuccess);
}

NSISFUNC(NSDetectSchedule)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);

	TCHAR taskName[128] = {0};
	popstringn(taskName, sizeof(taskName));

	std::wstring szTaskName = std::wstring(taskName);
	bool bsuccess = Utils::DetectSchedule(&szTaskName);
	pushint(bsuccess);
}

NSISFUNC(NSUpdateSchedule)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);

	TCHAR taskName[128] = {0};
	TCHAR comment[512] = {0};
	TCHAR workDir[MAX_PATH] = {0};
	TCHAR appName[128] = {0};
	TCHAR runParam[128] = {0};
	TCHAR userName[128] = {0};
	unsigned int minInterval = 60;
	popstringn(taskName, _ARRAYSIZE(taskName));
	popstringn(comment, _ARRAYSIZE(comment));
	popstringn(workDir, _ARRAYSIZE(workDir));
	popstringn(appName, _ARRAYSIZE(appName));
	popstringn(runParam, _ARRAYSIZE(runParam));
	//popstringn(userName, sizeof(userName));
	DWORD dwLen = _countof(userName);
	::GetUserName(userName, &dwLen);
	minInterval = static_cast<unsigned int>(popint());

	Utils::ScheduleData data;
	data.taskName = taskName;
	data.comment = comment;
	data.workdir = workDir;
	data.appName = appName;
	data.runParam = runParam;
	data.userName = userName;
	data.minutesInterval = minInterval;

	bool bsuccess = Utils::UpdateSchedule(&data);
	pushint(bsuccess);
}

NSISFUNC(NSDeleteSchedule)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);

	TCHAR taskName[128] = {0};
	popstringn(taskName, _ARRAYSIZE(taskName));

	std::wstring szTaskName = std::wstring(taskName);
	bool bsuccess = Utils::DeleteSchedule(&szTaskName);
	pushint(bsuccess);
}