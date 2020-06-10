// nsCurl.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "nsDefine.h"
#include "nsis/pluginapi.h"
// #include "Logging.h"
#include "easy_libcurl.h"
#include "Utils.h"

#include <cassert>
#include <process.h>

#ifndef VERIFY
#ifdef _DEBUG
#define VERIFY(_e) assert(_e)
#else
#define VERIFY(_e) (_e)
#endif
#endif

//External
HINSTANCE g_hInstance = NULL;
extra_parameters* g_pluginParms = nullptr;

//Global
static volatile bool g_bCallbackRegistred = false;
static easy_libcurl::RequestParam g_request;
static HANDLE g_hSyncEvent = NULL;
static HANDLE g_hApcHandle = NULL;
static long g_HttpCode = 0;
static const int UM_MSG_URL = WM_USER + 0x10;
static unsigned int g_ApcId = 0;

BOOL APIENTRY DllMain( HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		assert(!g_bCallbackRegistred);
		g_hInstance = hModule;
		if (!g_bCallbackRegistred)
		{
			if (!easy_libcurl::init())
				return FALSE;
			easy_libcurl::prepare_simple_request(&g_request);
			g_hSyncEvent = ::CreateEvent(NULL, TRUE, TRUE, NULL);
		}
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		::CloseHandle(g_hApcHandle);
		::CloseHandle(g_hSyncEvent);
		VERIFY(easy_libcurl::uninit());
		break;
	}
	return TRUE;
}

static unsigned WINAPI HttpAsyncProc(void*)
{
	MSG msg;
	::PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
	BOOL bRet;
	while( (bRet = ::GetMessage( &msg, NULL, 0, 0 )) != 0)
	{ 
		if (bRet == -1)
		{
			assert(false);
			break;
		}
		::TranslateMessage(&msg);

		switch(msg.message)
		{
		case UM_MSG_URL:
			reinterpret_cast<void (CALLBACK*)(ULONG_PTR)>(msg.lParam)(static_cast<ULONG_PTR>(msg.wParam));
			break;
		default:
			::DispatchMessage(&msg);
			break;
		}
	}

	return bRet;
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

static VOID CALLBACK HttpGetProc(
	_In_ ULONG_PTR dwParam)
{
	::ResetEvent(g_hSyncEvent);
	char* url = reinterpret_cast<char*>(dwParam);
	auto resp = easy_libcurl::get(url, &g_request);
	g_HttpCode = resp->status_code;
	delete[] url;
	::SetEvent(g_hSyncEvent);
}

NSISFUNC(NSGet)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);

	char* url = new char[512];
	memset(url, 0, 512);
	PopStringNA(url, 512);

	//TODO 每次请求提供事件id，用以同步
	::ResetEvent(g_hSyncEvent);
	if(::PostThreadMessage(g_ApcId, UM_MSG_URL, reinterpret_cast<WPARAM>(url), reinterpret_cast<LPARAM>(HttpGetProc)))
	{
		::WaitForSingleObject(g_hSyncEvent, INFINITE);
		pushint(static_cast<int>(g_HttpCode));
	}
	else
	{
		pushint(0);
		::SetEvent(g_hSyncEvent);
		assert(false);
	}
}

NSISFUNC(NSGetAsync)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);

	char* url = new char[512];
	memset(url, 0, 512);
	PopStringNA(url, 512);
	pushint(::PostThreadMessage(g_ApcId, UM_MSG_URL, reinterpret_cast<WPARAM>(url), reinterpret_cast<LPARAM>(HttpGetProc)));
}

static VOID CALLBACK HttpPostProc(
	_In_ ULONG_PTR dwParam)
{
	::ResetEvent(g_hSyncEvent);
	DWORD* pArray = reinterpret_cast<DWORD*>(dwParam);
	char* url = reinterpret_cast<char*>(pArray[0]);
	char* data = reinterpret_cast<char*>(pArray[1]);

	auto resp = easy_libcurl::post(url, data, -1, &g_request);
	g_HttpCode = resp->status_code;
	
	delete[] url;
	delete[] data;
	delete[] pArray;
	::SetEvent(g_hSyncEvent);
}

NSISFUNC(NSPost)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);

	char* url = new char[512];
	char* data = new char[1024];
	memset(url, 0, 512);
	memset(data, 0, 1024);
	PopStringNA(url, 512);
	PopStringNA(data, 512);
	DWORD* pArray = new DWORD[2];
	pArray[0] = reinterpret_cast<DWORD>(url);
	pArray[1] = reinterpret_cast<DWORD>(data);

	//TODO 每次请求提供事件id，用以同步
	::ResetEvent(g_hSyncEvent);
	if(::PostThreadMessage(g_ApcId, UM_MSG_URL, reinterpret_cast<WPARAM>(pArray), reinterpret_cast<LPARAM>(HttpPostProc)))
	{
		::WaitForSingleObject(g_hSyncEvent, INFINITE);
		pushint(static_cast<int>(g_HttpCode));
	}
	else
	{
		::SetEvent(g_hSyncEvent);
		pushint(0);
		assert(false);
	}
}

NSISFUNC(NSPostAsync)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);

	char* url = new char[512];
	char* data = new char[1024];
	memset(url, 0, 512);
	memset(data, 0, 1024);
	PopStringNA(url, 512);
	PopStringNA(data, 1024);
	DWORD* pArray = new DWORD[2];
	pArray[0] = reinterpret_cast<DWORD>(url);
	pArray[1] = reinterpret_cast<DWORD>(data);

	pushint(::PostThreadMessage(g_ApcId, UM_MSG_URL, reinterpret_cast<WPARAM>(pArray), reinterpret_cast<LPARAM>(HttpPostProc)));
}


NSISFUNC(NSWaitDone)
{
	EXDLL_INIT();
	REGSITER_CALLBACK(g_hInstance);

	::PostThreadMessage(g_ApcId, WM_QUIT, 0, 0);

	if (WAIT_OBJECT_0 != ::WaitForSingleObject(g_hApcHandle, popint()))
	{
		assert(false);
		::TerminateThread(g_hApcHandle, -1);
		pushint(0);
	}
	else
	{
		pushint(1);
	}
}