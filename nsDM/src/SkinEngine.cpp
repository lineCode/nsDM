#include "StdAfx.h"
#include "SkinEngine.h"
#include "Utils.h"
#include "nsDefine.h"
#include "api.h"
#include "output_debug.h"
#include "nsDmMsg.h"

#include "DUIStatic.h"
#include "DUIEdit.h"

#include <Shobjidl.h>
#include <Shlobj.h>
#include <algorithm>
#include <Shellapi.h>
#include <Psapi.h>
#pragma comment(lib, "Psapi.lib")

extern extra_parameters* g_pluginParms;
extern HINSTANCE g_hInstance;

#define UM_CLICK_RETRY (WM_USER + 0x10)

BEGIN_MSG_MAP(CSkinEngine)
	MSG_WM_INITDIALOG(OnInitDialog)
	//MSG_WM_SHOWWINDOW(OnShowWindow)
	//MSG_WM_SIZE(OnSize)
	MSG_WM_CLOSE(OnClose)
	MESSAGE_HANDLER_EX(UM_CLICK_RETRY, OnClickRetry)
	CHAIN_MSG_MAP(DM::DMHWnd)// 将未处理的消息交由DMHWnd处理
END_MSG_MAP()

BEGIN_EVENT_MAP(CSkinEngine)
	CHAIN_NSDM_HANDLER(DM::DMEVT_RENOTIFY, NSDMNameHandlerEvent)
	CHAIN_NSDM_EVENT(NSDMHandleEvent) // 将未处理的消息交由NSIS脚本处理
END_EVENT_MAP()

BEGIN_MSG_MAP(CNSDMMessageBox)
	MSG_WM_INITDIALOG(OnInitDialog)
	CHAIN_MSG_MAP(DM::DMHDialog)
END_MSG_MAP()

BEGIN_EVENT_MAP(CNSDMMessageBox)
	EVENT_NAME_COMMAND(L"btn_ok", OnClickOk)
	EVENT_NAME_COMMAND(L"btn_cancel", OnClickCancel)
	EVENT_NAME_COMMAND(L"btn_close", OnClickClose)
	EVENT_NAME_COMMAND(L"btn_retry", OnClickRetry)
END_EVENT_MAP()

BEGIN_MSG_MAP(CNSDMLicenseWnd)
	MSG_WM_INITDIALOG(OnInitDialog)
	CHAIN_MSG_MAP(DM::DMHDialog)
END_MSG_MAP()

BEGIN_EVENT_MAP(CNSDMLicenseWnd)
	EVENT_NAME_COMMAND(L"btn_ok", OnClickOk)
	EVENT_NAME_COMMAND(L"btn_close", OnClickClose)
END_EVENT_MAP()

CSkinEngine::CSkinEngine(void)
{
	cur_panel_ = -1;
}


CSkinEngine::~CSkinEngine(void)
{
}

BOOL CSkinEngine::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{
	MACRO_DisableMaxStyle(m_hWnd);

	TCHAR module_name[MAX_PATH] = {0};
	if (0 == ::GetModuleFileNameEx(::GetCurrentProcess(), nullptr, module_name, _countof(module_name)))
	{
		return FALSE;
	}
	WORD icon_id = 0;
	assert(g_hInstance);
	auto hIcon = ::ExtractAssociatedIcon(g_hInstance, module_name, &icon_id);
	assert(hIcon);
	SetIcon(hIcon, TRUE);
	SetIcon(hIcon, FALSE);

	//CWndShadow::Initialize(g_hInstance);
	//wndShadow_.Create(m_hWnd);
	//wndShadow_.SetPosition(0, 0);

	return TRUE;
}

void CSkinEngine::OnSize(UINT nType, DM::CSize size)
{
	//wndShadow_.SetPosition(0, 0);
	SetMsgHandled(FALSE);
}

void CSkinEngine::OnShowWindow(BOOL bShow, UINT nStatus)
{
	if (bShow)
	{
		//wndShadow_.SetPosition(0, 0);
	}
	SetMsgHandled(FALSE);
}

//DMCode CSkinEngine::OnMinimize()
//{
//	SendMessage(WM_SYSCOMMAND,SC_MINIMIZE);
//	return DM::DM_ECODE_OK;
//}


void CSkinEngine::SaveToControlCallbackMap(const wchar_t* ctlName, int call_addr)
{
	ctl_call_map_.insert(std::make_pair(ctlName, call_addr));
}

void CSkinEngine::UpdateBkImage(DM::DUIWindow* pWnd, byte* pBuf, DWORD dwSize)
{
	DM::IDMSkinPtr bkSkin = nullptr;
	pWnd->m_pDUIXmlInfo->m_pStyle->GetBgSkin(&bkSkin);
	bkSkin->SetBitmap(pBuf, dwSize, nullptr);
	pWnd->DM_Invalidate();
}

void CSkinEngine::UpdateBkImage(DM::DUIWindow* pWnd, LPCWSTR lpszPath)
{
	auto file_size = DM::GetFileSizeW(const_cast<wchar_t*>(lpszPath));
	DM::DMBufT<byte> mem_buf;
	auto pBuf = mem_buf.AllocateBytes(file_size);
	DWORD dwRead = 0;
	DM::GetFileBufW(const_cast<wchar_t*>(lpszPath), reinterpret_cast<void**>(&pBuf), file_size, dwRead);
	UpdateBkImage(pWnd, pBuf, dwRead);
}

int CSkinEngine::NSMessageBox(LPCTSTR lpszXmlId, int width, int height, LPCTSTR lpszTitleId,
	LPCTSTR lpszTitle, LPCTSTR lpszContentId, LPCTSTR lpszContent, int type)
{
	DM::DMSmartPtrT<CNSDMMessageBox> msg_box;
	SIZE size = {width, height};
	msg_box.Attach(new CNSDMMessageBox(lpszTitleId, lpszTitle, lpszContentId, lpszContent, size, type));
	return static_cast<int>(msg_box->DoModal(lpszXmlId, m_hWnd, true));
}

std::wstring CSkinEngine::SelectFolderDialog(LPCTSTR lpszTitle) const
{
	std::wstring result;
	if (Utils::is_vista_or_later())
	{
		promptOpenFolderVistaLater(result, nullptr, lpszTitle);
	}
	else
	{
		promptOpenFolderXp(result, nullptr, lpszTitle);
	}
	return result;
}

std::wstring& CSkinEngine::promptOpenFolderVistaLater(std::wstring& folder_path, PCWSTR initPath, LPCTSTR lpszTitle) const
{
	IFileOpenDialog* pDlg;
	HRESULT hr = ::CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pDlg));
	if (SUCCEEDED(hr))
	{
		DWORD fos;
		pDlg->GetOptions(&fos);
		fos |= (FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM | FOS_NOVALIDATE);
		pDlg->SetOptions(fos);
		//pDlg->SetOkButtonLabel(lpszTitle);
		pDlg->SetTitle(lpszTitle);

		PIDLIST_ABSOLUTE pidl;
		hr = ::SHParseDisplayName(initPath, NULL, &pidl, SFGAO_FOLDER, NULL);
		//IShellItem* pinitShell;
		//hr = ::SHCreateItemFromParsingName(initPath, NULL, IID_IShellItem, reinterpret_cast<void**>(&pinitShell));
		if(SUCCEEDED(hr))
		{
			IShellItem* pinitShell;
			hr = ::SHCreateShellItem(NULL, NULL, pidl, &pinitShell);
			if (SUCCEEDED(hr))
			{
				pDlg->SetFolder(pinitShell);
				pinitShell->Release();
			}
			::ILFree(pidl);
		}

		hr = pDlg->Show(m_hWnd);
		if (SUCCEEDED(hr))
		{
			IShellItem* pItem;
			hr = pDlg->GetResult(&pItem);
			if (SUCCEEDED(hr))
			{
				LPWSTR pszpath;
				pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszpath);
				if (SUCCEEDED(hr))
				{
					folder_path = pszpath;
					::CoTaskMemFree(pszpath);
				}
				pItem->Release();
			}
		}

		pDlg->Release();
	}
	return folder_path;
}

std::wstring& CSkinEngine::promptOpenFolderXp(std::wstring& folder_path, PCWSTR initPath, LPCTSTR lpszTitle) const
{
	BROWSEINFO bi;
	TCHAR szBuffer[MAX_PATH] = { 0 };

	ZeroMemory(&bi, sizeof(BROWSEINFO));
	bi.hwndOwner = m_hWnd;
	bi.pszDisplayName = szBuffer;
	bi.lpszTitle = lpszTitle;
	bi.ulFlags = BIF_RETURNFSANCESTORS | BIF_USENEWUI;
	bi.lpfn = BrowseCallbackProc;
	bi.lParam = reinterpret_cast<LPARAM>(initPath);

	LPITEMIDLIST pidl = ::SHBrowseForFolder(&bi);
	if (NULL == pidl)
		return folder_path;

	::SHGetPathFromIDList(pidl, szBuffer);
	folder_path = szBuffer;

	IMalloc * imalloc = NULL;
	if (SUCCEEDED(::SHGetMalloc(&imalloc)))
	{
		imalloc->Free(pidl);
		imalloc->Release();
	}

	return folder_path;
}

int CALLBACK CSkinEngine::BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	if (uMsg == BFFM_INITIALIZED)
	{
		::SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
	}
	return 0;
}


void CSkinEngine::SelectPanel(int index)
{
	if (index < 0
		|| static_cast<size_t>(index) >= dummy_tab_ctl_.size())
	{
		assert(false);
		return;
	}

	NSDM_DEBUG_INFO("SelectPanel index:%d", index);

	//for each(auto& wnd in dummy_tab_ctl_)
	//{
	//	wnd->DM_SetVisible(false);
	//}
	dummy_tab_ctl_[cur_panel_].second->DM_SetVisible(false);
	dummy_tab_ctl_[index].second->DM_SetVisible(true, true);

	cur_panel_ = index;
}

void CSkinEngine::SelectPanel(LPCTSTR lpszName)
{
	auto itFind = std::find_if(dummy_tab_ctl_.begin(), dummy_tab_ctl_.end(), 
		[lpszName](const std::pair<std::wstring, DM::DUIWindow*>& panel)
	{
		return _tcsicmp(lpszName, panel.first.c_str()) == 0;
	});
	if (itFind == dummy_tab_ctl_.end())
	{
		assert(false);
		return;
	}
	SelectPanel(static_cast<int>(std::distance(dummy_tab_ctl_.begin(), itFind)));
}

void CSkinEngine::PrevPanel()
{
	if (cur_panel_ > 0)
	{
		SelectPanel(cur_panel_ - 1);
	}
	else
	{
		SelectPanel(cur_panel_);
	}
}

void CSkinEngine::NextPanel()
{
	assert(!dummy_tab_ctl_.empty());
	if (static_cast<size_t>(cur_panel_) < dummy_tab_ctl_.size() - 1)
	{
		SelectPanel(cur_panel_ + 1);
	}
	else
	{
		SelectPanel(cur_panel_);
	}
}

void CSkinEngine::InitPageList(LPCTSTR lpszRoot)
{
	auto rootWnd = FindChildByName(lpszRoot);
	assert(rootWnd);

	int index = 0;
	for (auto itWnd = rootWnd->m_Node.m_pFirstChild;
		itWnd != NULL;
		itWnd = itWnd->m_Node.m_pNextSibling, ++index)
	{
		dummy_tab_ctl_.push_back(std::make_pair(itWnd->m_pDUIXmlInfo->m_strName.GetBuffer(), itWnd));
		itWnd->m_pDUIXmlInfo->m_strName.ReleaseBuffer();
		if (itWnd->m_pDUIXmlInfo->m_bVisible)
		{
			NSDM_DEBUG_INFO("InitPageList index:%d cur_panel_:%d", index, cur_panel_);
			assert(cur_panel_ == -1);
			cur_panel_ = index;
		}
	}
}

DMCode CSkinEngine::NSDMHandleEvent(LPCTSTR lpszName)
{
	auto itFind = ctl_call_map_.find(lpszName);
	if (itFind != ctl_call_map_.end())
	{
		g_pluginParms->ExecuteCodeSegment(itFind->second, nullptr);
	}
	return DM::DM_ECODE_OK;
}

int CSkinEngine::ShowLicense(LPCTSTR lpszXmlId, LPCTSTR lpszTitleId, LPCTSTR lpszTitle, 
		LPCTSTR lpszRichId, LPCTSTR lpszLicensePath)
{
	DM::DMSmartPtrT<CNSDMLicenseWnd> liceWnd; 
	liceWnd.Attach(new CNSDMLicenseWnd(lpszTitleId, lpszTitle, lpszRichId, lpszLicensePath));
	return liceWnd->DoModal(lpszXmlId, m_hWnd, true);
}

LRESULT CSkinEngine::OnClickRetry(UINT uMsg, WPARAM wp, LPARAM lp)
{
	UNREFERENCED_PARAMETER(uMsg);
	UNREFERENCED_PARAMETER(lp);
	NSDMHandleEvent(reinterpret_cast<LPCTSTR>(wp));
	return 0;
}


DMCode CSkinEngine::NSDMNameHandlerEvent(DM::DMEventArgs* args)
{
	if (args->GetEventID() == DM::DMEventRENotifyArgs::EventID
		&& static_cast<DM::DMEventRENotifyArgs*>(args)->m_iNotify == EN_CHANGE)
	{
		auto itFind = ctl_call_map_.find(args->m_szNameFrom);
		if (itFind != ctl_call_map_.end())
		{
			g_pluginParms->ExecuteCodeSegment(itFind->second, nullptr);
		}
	}
	
	return DM::DM_ECODE_OK;
}

DMCode CSkinEngine::OnClose()
{
	//wndShadow_.SetSize();
	DestroyWindow();
	return DM_ECODE_OK;
}

CNSDMMessageBox::CNSDMMessageBox(LPCTSTR lpszTitleId, LPCTSTR lpszTitle, 
	LPCTSTR lpszContentId, LPCTSTR lpszContent, const SIZE& size, int type)
{
	str_title_ = lpszTitle;
	title_id_ = lpszTitleId;
	str_content_ = lpszContent;
	content_id_ = lpszContentId;
	type_ = type;
	width_ = static_cast<int>(size.cx);
	height_ = static_cast<int>(size.cy);
}

CNSDMMessageBox::~CNSDMMessageBox()
{

}

BOOL CNSDMMessageBox::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{
	FindChildByName(title_id_.c_str())->DV_SetWindowText(str_title_.c_str());
	FindChildByName(content_id_.c_str())->DV_SetWindowText(str_content_.c_str());

	RECT rcWnd;
	GetWindowRect(&rcWnd);
	width_ = (width_ < 0) ? (rcWnd.right - rcWnd.left) : width_;
	height_ = (height_ < 0) ? (rcWnd.bottom - rcWnd.top) : height_;
	
	SetWindowPos(HWND_TOP, 0, 0, width_, height_, SWP_NOMOVE);
	CenterWindow();

	switch(type_)
	{
	case MB_RETRYCANCEL:
		{
			auto btn_ok = FindChildByName(L"btn_ok");
			if (btn_ok)
			{
				btn_ok->DM_SetVisible(false);
			}

			auto btn_cancel = FindChildByName(L"btn_cancel");
			if (btn_cancel)
			{
				btn_cancel->DM_SetVisible(true);
			}

			auto btn_retry = FindChildByName(L"btn_retry");
			if (btn_retry)
			{
				btn_retry->DM_SetVisible(true);
			}
		}
		break;
	case MB_OKCANCEL:
	default:
		{
			auto btn_ok = FindChildByName(L"btn_ok");
			if (btn_ok)
			{
				btn_ok->DM_SetVisible(true);
			}

			auto btn_cancel = FindChildByName(L"btn_cancel");
			if (btn_cancel)
			{
				btn_cancel->DM_SetVisible(true);
			}

			auto btn_retry = FindChildByName(L"btn_retry");
			if (btn_retry)
			{
				btn_retry->DM_SetVisible(false);
			}
		}
		break;
	}

	MACRO_DisableMaxStyle(m_hWnd);
	return TRUE;
}

DMCode CNSDMMessageBox::OnClickClose()
{
	EndDialog(IDCLOSE);
	return DM::DM_ECODE_OK;
}

DMCode CNSDMMessageBox::OnClickOk()
{
	EndDialog(IDOK);
	return DM::DM_ECODE_OK;
}

DMCode CNSDMMessageBox::OnClickCancel()
{
	EndDialog(IDCANCEL);
	return DM::DM_ECODE_OK;
}

DMCode CNSDMMessageBox::OnClickRetry()
{
	GetParent().SendMessage(UM_CLICK_RETRY, reinterpret_cast<WPARAM>(L"btn_retry"));
	return DM::DM_ECODE_OK;
}

CNSDMLicenseWnd::CNSDMLicenseWnd(LPCTSTR lpszTitleId, LPCTSTR lpszTitle, 
		LPCTSTR lpszRichId, LPCTSTR lpszLicensePath)
{
	str_title_ = lpszTitle;
	title_id_ = lpszTitleId;
	content_id_ = lpszRichId;
	license_path_ = lpszLicensePath;
}

BOOL CNSDMLicenseWnd::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{
	FindChildByName(title_id_.c_str())->DV_SetWindowText(str_title_.c_str());

	std::string file_content;
	if (!Utils::read_file(license_path_.c_str(), file_content))
	{
		return FALSE;
	}
	file_content.append("\0");
	std::wstring unicode_content = Utils::ansi_to_unicode(file_content.c_str());
	FindChildByNameT<DM::DUIRichEdit>(content_id_.c_str())->SetWindowText(unicode_content.c_str());

	::SetForegroundWindow(m_hWnd);

	MACRO_DisableMaxStyle(m_hWnd);
	return TRUE;
}

DMCode CNSDMLicenseWnd::OnClickClose()
{
	EndDialog(IDCLOSE);
	return DM::DM_ECODE_OK;
}

DMCode CNSDMLicenseWnd::OnClickOk()
{
	EndDialog(IDOK);
	return DM::DM_ECODE_OK;
}

