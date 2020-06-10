// nsDM.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"

#include "nsis/pluginapi.h"
#include "nsDM.h"
#include "nsDmMsg.h"
#include "SkinEngine.h"
#include "Logging.h"
#include "Utils.h"

#pragma region DM include
#include "DMResZipParam.h"
#include "DMResZipImpl.h"
#include "DUIProcessCtrl.h"
#include "NotifyEdit.h"
#pragma endregion

#pragma region stl include
#include <cassert>
#pragma endregion


//External
HINSTANCE g_hInstance = NULL;
extra_parameters* g_pluginParms = nullptr;

//Global
static volatile bool g_bCallbackRegistred = false;
static DM::DMApp* theApp = nullptr;
static DM::DMSmartPtrT<CSkinEngine> g_pFrame;

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
		break;
	case DLL_THREAD_ATTACH:		
		break;
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
		OutputDebugStringA("NSDM: Unknown callback message. Take care!\n");
		break;
	}

	return 0;
}

NSISFUNC(NSInitSkinEngine)
{
	EXDLL_INIT();
 
	REGSITER_CALLBACK(g_hInstance);

	g_pluginParms = extra;
	::OleInitialize(NULL);
	{
		static DM::DMApp _theApp(g_hInstance);
		theApp = &_theApp;
	}
	
	NSDM_DEBUG_INFO("\n");

#if WAITING_FOR_DEBUGGER_TO_ATTACH
	MessageBox(NULL, L"Waiting for debugger to attach", L"Info", MB_OK | MB_ICONEXCLAMATION);
#endif
	
	TCHAR skinPath[MAX_PATH] = {0};
	TCHAR xmlid[128] = {0};
	TCHAR wndName[128] = {0};
	popstringn(skinPath, _ARRAYSIZE(skinPath)); // 皮肤路径
	popstringn(xmlid, _ARRAYSIZE(xmlid)); // 主窗口xml id
	popstringn(wndName, _ARRAYSIZE(wndName)); // 窗口控件名
	NSDM_DEBUG_INFO("Load skin '%s', xmlid '%s' \n", 
		Utils::unicode_to_ansi(skinPath).c_str(), 
		Utils::unicode_to_ansi(xmlid).c_str());
	
	auto zipImp = DM::DMRegHelperT<DM::DMResZipImpl>();
	theApp->Register(zipImp, true);
	auto editImp = DM::DMRegHelperT<CNotifyEdit>();
	theApp->Register(editImp, true);
	//theApp->SetDefRegObj(L"DMResZipImpl", DM::DMREG_Res);
	DMResZipParam zipres(skinPath, nullptr);
	if (DM_ECODE_OK != theApp->LoadResPack(reinterpret_cast<WPARAM>(&zipres),
		0, DM::DMResZipImpl::GetClassNameW()))
	{
		pushint(0);
		NSDM_DEBUG_INFO("-XX- LoadResPack failed\n");
		return;
	}
	theApp->InitGlobal();

	g_pFrame.Attach(new CSkinEngine());
	g_pFrame->DM_CreateWindow(xmlid, 0, 0, 0, 0, nullptr, false);
	g_pFrame->SendMessage(WM_INITDIALOG);
	g_pFrame->CenterWindow();
	g_pFrame->ShowWindow(SW_HIDE);
	g_pFrame->InitPageList(wndName);

	pushint(reinterpret_cast<int>(g_pFrame->GetSafeHwnd()));
}

NSISFUNC(NSFindControl)
{
	EXDLL_INIT();
	//REGSITER_CALLBACK(g_hInstance);

	TCHAR controlName[128] = {0};
	popstringn(controlName, _ARRAYSIZE(controlName));
	assert(g_pFrame);
	auto controlWnd = g_pFrame->FindChildByName(controlName);
	if (controlWnd)
	{
		pushint(static_cast<int>(controlWnd->GetDUIWnd()));
	} 
	else
	{
		NSDM_DEBUG_INFO("-XX- FindControl(%s) failed \n", Utils::unicode_to_ansi(controlName).c_str());
		pushint(0);
	}
}

NSISFUNC(NSRegisterCommandNotify)
{
	EXDLL_INIT();
	//REGSITER_CALLBACK(g_hInstance);

	TCHAR controlName[MAX_PATH] = {0};
	popstringn(controlName, _ARRAYSIZE(controlName));
	int callback_addr = popint();
	g_pFrame->SaveToControlCallbackMap(controlName, callback_addr);
	NSDM_DEBUG_INFO("control '%s' bind to address 0x%x \n", 
		Utils::unicode_to_ansi(controlName).c_str(), callback_addr);
}

NSISFUNC(NSShowLicense)
{
	EXDLL_INIT();
	//REGSITER_CALLBACK(g_hInstance);

	TCHAR xmlid[128] = {0};
	TCHAR title_id[128] = {0};
	TCHAR title[512] = {0};
	TCHAR content_id[128] = {0};
	TCHAR content[1024] = {0};
	popstringn(xmlid, _ARRAYSIZE(xmlid));
	popstringn(title_id, _ARRAYSIZE(title_id));
	popstringn(title, _ARRAYSIZE(title));
	popstringn(content_id, _ARRAYSIZE(content_id));
	popstringn(content, _ARRAYSIZE(content));
	pushint(g_pFrame->ShowLicense(xmlid, title_id, title, content_id, content));
}

NSISFUNC(NSSetControlAttr)
{
	EXDLL_INIT();
	//REGSITER_CALLBACK(g_hInstance);

	TCHAR controlName[128] = {0};
	TCHAR controlData[512] = {0};
	TCHAR dataType[128] = {0};
	
	popstringn(controlName, _ARRAYSIZE(controlName));
	popstringn(controlData, _ARRAYSIZE(controlData));
	popstringn(dataType, _ARRAYSIZE(dataType));

	assert(g_pFrame);
	auto pControl = g_pFrame->FindChildByName(controlName);
	if (!pControl)
	{
		NSDM_DEBUG_INFO("-XX- Can't find control %s \n", Utils::unicode_to_ansi(controlName).c_str());
		return;
	}

	if (_tcsicmp( dataType, _T("bkimage") ) == 0)
	{
		if (_tcslen(controlData) == 0 || _tcsicmp(controlData, _T("error")) == 0)
		{
			pControl->DM_Invalidate();
		}
		else
		{
			g_pFrame->UpdateBkImage(pControl, controlData);
		}
	}
	else if ( _tcsicmp( dataType, _T("check") ) == 0 )
	{
		pControl->DM_SetCheck(_tcsicmp(controlData, L"0") != 0);
	}
	else if ( _tcsicmp( dataType, _T("bdisable") ) == 0 )
	{
		pControl->DM_EnableWindow(_tcsicmp(controlData, L"0") == 0, true);
	}
	else
	{
		if (_tcslen(controlData) == 0 || _tcsicmp(controlData, _T("error")) == 0)
		{
			pControl->DM_Invalidate();
		}
		else
		{
			pControl->SetAttribute(dataType, controlData);
		}
		
	}
}

NSISFUNC(NSGetControlAttr)
{
	EXDLL_INIT();
	//REGSITER_CALLBACK(g_hInstance);

	TCHAR ctlName[128] = {0};
	TCHAR dataType[128] = {0};
	popstringn(ctlName, _ARRAYSIZE(ctlName));
	popstringn(dataType, _ARRAYSIZE(dataType));

	auto pControl = g_pFrame->FindChildByName(ctlName);
	if (!pControl)
	{
		pushstring(_T("error"));
		return;
	}

	if (_tcsicmp( dataType, _T("text") ) == 0)
	{
		auto pEdit = dynamic_cast<DM::DUIEdit*>(pControl);
		DM::CStringW strText;
		if (pEdit)
		{
			strText = pEdit->GetWindowText();
		}
		else
		{
			strText = pControl->m_pDUIXmlInfo->m_strText;
		}
		pushstring(strText.GetBuffer());
		strText.ReleaseBuffer();
	}
	else if (_tcsicmp( dataType, _T("check") ) == 0)
	{
		pushint(pControl->DM_IsChecked());
	}
	else
	{
		assert(false);
		pushstring(_T("error"));
	}
	
}

NSISFUNC(NSSetWindowAttr)
{
	EXDLL_INIT();
	//REGSITER_CALLBACK(g_hInstance);

	TCHAR controlData[512] = {0};
	TCHAR dataType[128] = {0};

	popstringn(controlData, _ARRAYSIZE(controlData));
	popstringn(dataType, _ARRAYSIZE(dataType));

	if (_tcsicmp(L"possize", dataType) == 0)
	{
		std::vector<std::wstring> pos;
		Utils::SplitWide(controlData, L",", pos);
		if (pos.size() == 2)
		{
			RECT rcWnd = {0, 0, _ttoi(pos[0].c_str()), _ttoi(pos[1].c_str())};
			g_pFrame->SetWindowPos(HWND_TOP, &rcWnd, SWP_NOMOVE);
		}
	}
	else
	{
		g_pFrame->SetAttribute(dataType, controlData);
		g_pFrame->UpdateWindow();
	}
	
}

NSISFUNC(NSGetWindowAttr)
{
	EXDLL_INIT();
	//REGSITER_CALLBACK(g_hInstance);

	TCHAR ctlName[128] = {0};
	popstringn(ctlName, _ARRAYSIZE(ctlName));
	pushstring(L"error");
}

static VOID CALLBACK TimerProc(
	_In_ HWND     hwnd,
	_In_ UINT     uMsg,
	_In_ UINT_PTR idEvent,
	_In_ DWORD    dwTime
	)
{
	assert(g_pluginParms);
	g_pluginParms->ExecuteCodeSegment(idEvent, nullptr);
}

NSISFUNC(NSCreateTimer)
{
	EXDLL_INIT();
	//REGSITER_CALLBACK(g_hInstance);
	
	int timer_id = popint();
	int interval = popint();
	if (timer_id == 0
		|| interval == 0)
	{
		NSDM_DEBUG_INFO("-XX- NSDMCreateTimer(%d, %d) invalid arguments \n", timer_id, interval);
		return;
	}

	::SetTimer(g_pFrame->GetSafeHwnd(), timer_id, interval, TimerProc);
}

NSISFUNC(NSKillTimer)
{
	EXDLL_INIT();
	//REGSITER_CALLBACK(g_hInstance);
	int timer_id = popint();
	::KillTimer(g_pFrame->GetSafeHwnd(), timer_id);
}

NSISFUNC(NSSendMessage)
{
	EXDLL_INIT();
	//REGSITER_CALLBACK(g_hInstance);

	TCHAR msgID[128] = {0};
	TCHAR wParam[128] = {0};
	TCHAR lParam[512] = {0};
	popstringn( msgID,_ARRAYSIZE(msgID));
	popstringn( wParam,_ARRAYSIZE(wParam));
	popstringn( lParam ,_ARRAYSIZE(lParam));

	if (_tcsicmp(msgID, _T("UM_NSDM_MIN")) == 0)
		g_pFrame->PostMessage(WM_SYSCOMMAND, SC_MINIMIZE);
	else if( _tcsicmp( msgID, _T("UM_NSDM_CLOSE")) == 0 )
		g_pFrame->PostMessage(WM_SYSCOMMAND, SC_CLOSE);
	else if( _tcsicmp( msgID, _T("UM_NSDM_CANCEL")) == 0 )
	{
		//TCHAR xml_id[128] = {0};
		//popstring(xml_id, sizeof(xml_id));
		//LPCTSTR lpTitle = wParam;
		//LPCTSTR lpText = lParam;
		//if (IDOK == g_pFrame->NSMessageBox(xml_id, g_pFrame->GetSafeHwnd(), lpTitle, lpText))
		//{
		//	pushint(1);
		//	g_pFrame->SendMessage(NSDM::UM_NSDM_CLOSE, reinterpret_cast<WPARAM>(wParam), reinterpret_cast<LPARAM>(lParam));
		//}
		//else
		//{
		//	pushint(0);
		//}
	}
	else if (_tcsicmp( msgID, _T("UM_NSDM_QUERY_CANCEL")) == 0)
	{
		//TCHAR xml_id[128] = {0};
		//popstring(xml_id, sizeof(xml_id));
		//LPCTSTR lpTitle = wParam;
		//LPCTSTR lpText = lParam;
		//if (IDOK == g_pFrame->NSMessageBox(xml_id, g_pFrame->GetSafeHwnd(), lpTitle, lpText))
		//{
		//	pushint(1);
		//}
		//else
		//{
		//	pushint(0);
		//}
	}
	else if (_tcsicmp(msgID,_T("UM_NSDM_EXIT")) == 0)
	{
		g_pFrame->SendMessage(WM_QUIT);
	}
	else if (_tcsicmp( msgID, _T("UM_NSDM_OPEN_URL")) == 0)
	{
		Utils::open_url(wParam);
	}
	else
	{
		int id = 0;
		if (::StrToIntEx(msgID, STIF_SUPPORT_HEX, &id)
			&& id != 0)
		{
			g_pFrame->SendMessage(static_cast<UINT>(id), reinterpret_cast<WPARAM>(wParam), reinterpret_cast<LPARAM>(lParam));
		}
	}
}

NSISFUNC(NSSelectFolderDialog)
{
	EXDLL_INIT();
	//REGSITER_CALLBACK(g_hInstance);

	TCHAR title[128] = {0};
	popstringn(title, _ARRAYSIZE(title));
	
	assert(g_pFrame);
	auto select_path = g_pFrame->SelectFolderDialog(title);
	if (select_path.empty())
	{
		pushint(0);
	}
	else
	{
		pushstring(const_cast<wchar_t*>(select_path.c_str()));
	}
}

NSISFUNC(NSRun)
{
	EXDLL_INIT();
	//REGSITER_CALLBACK(g_hInstance);

	g_pFrame->ShowWindow(SW_SHOW);
	if (DM_ECODE_OK == theApp->Run(g_pFrame->GetSafeHwnd()))
	{
		pushint(1);
	}
	else
	{
		pushint(0);
	}
	g_pFrame.Release();
}

NSISFUNC(NSUninitSkinEngine)
{
	EXDLL_INIT();
	//REGSITER_CALLBACK(g_hInstance);
	g_pFrame->SendMessage(WM_CLOSE);
	theApp = nullptr;
}

NSISFUNC(NSMessageBox)
{
	EXDLL_INIT();
	//REGSITER_CALLBACK(g_hInstance);
	TCHAR xmlid[128] = {0};
	TCHAR size[128] = {0};
	TCHAR title_id[128] = {0};
	TCHAR title[512] = {0};
	TCHAR content_id[128] = {0};
	TCHAR content[1024] = {0};
	popstringn(xmlid, _ARRAYSIZE(xmlid));
	popstringn(size, _ARRAYSIZE(size));
	popstringn(title_id, _ARRAYSIZE(title_id));
	popstringn(title, _ARRAYSIZE(title));
	popstringn(content_id, _ARRAYSIZE(content_id));
	popstringn(content, _ARRAYSIZE(content));
	int type = popint();

	std::vector<std::wstring> vtsize;
	Utils::SplitWide(size, L",", vtsize);
	long width = -1;
	long height = -1;

	if (vtsize.size() >= 2)
	{
		width = _tcstol(vtsize[0].c_str(), NULL, 10);
		height = _tcstol(vtsize[1].c_str(), NULL, 10);
	}

	pushint(g_pFrame->NSMessageBox(xmlid, static_cast<int>(width), static_cast<int>(height), title_id, title, content_id, content, type));	
}

NSISFUNC(NSVerifyCharaters)
{
	EXDLL_INIT();
	//REGSITER_CALLBACK(g_hInstance);

	TCHAR characters[512];

	int ret = popstringn(characters, _ARRAYSIZE(characters));
	if (ret != 0){
		/*we have error*/
		pushint(0);
		return ;
	}

	for (int i = 0; i < _countof(characters); i++){
		if (characters[i] == 0x0){
			break;
		}

		if ((characters[i] >= 'a' && characters[i] <= 'z') ||
			(characters[i] >= 'A' && characters[i] <= 'Z') || 
			(characters[i] >= '0' && characters[i] <= '9') ||
			characters[i] == '_'){
				continue;
		}

		/*this is error code*/
		pushint(0);
		return;
	}

	pushint(1);
	return;
}

NSISFUNC(NSSelectPage)
{
	EXDLL_INIT();
	//REGSITER_CALLBACK(g_hInstance);
	TCHAR name[128] = {0};
	popstringn(name, _ARRAYSIZE(name));
	g_pFrame->SelectPanel(name);
}

NSISFUNC(NSPrevPage)
{
	EXDLL_INIT();
	//REGSITER_CALLBACK(g_hInstance);
	g_pFrame->PrevPanel();
}

NSISFUNC(NSNextPage)
{
	EXDLL_INIT();
	//REGSITER_CALLBACK(g_hInstance);
	g_pFrame->NextPanel();
}