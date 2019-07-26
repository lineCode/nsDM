#pragma once
#include <hash_map>
#include <string>
#include <vector>

#include "Noncopyable.h"
#include "WndShadow.h"

using DM::DMCode;
using DM::DM_ECODE_FAIL;
using DM::DM_ECODE_OK;

class CSkinEngine: public DM::DMHWnd, private base::NonCopyable
{
public:
	CSkinEngine(void);
	~CSkinEngine(void);

protected:
	BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);
	void OnSize(UINT nType, DM::CSize size);	
	void OnShowWindow(BOOL bShow, UINT nStatus);
	DMCode NSDMHandleEvent(LPCTSTR lpszName);
	LRESULT OnClickRetry(UINT uMsg, WPARAM wp, LPARAM lp);
	DMCode NSDMNameHandlerEvent(DM::DMEventArgs* args);
	DMCode OnClose();

	DECLARE_MESSAGE_MAP()
	DECLARE_EVENT_MAP()

public:

	void SaveToControlCallbackMap(const wchar_t* ctlName, int call_addr);
	void UpdateBkImage(DM::DUIWindow* pWnd, byte* pBuf, DWORD dwSize);
	void UpdateBkImage(DM::DUIWindow* pWnd, LPCWSTR lpszPath);
	int NSMessageBox(LPCTSTR lpszXmlId, int width, int height, LPCTSTR lpszTitleId,
		LPCTSTR lpszTitle, LPCTSTR lpszContentId, LPCTSTR lpszContent, int type);
	std::wstring SelectFolderDialog(LPCTSTR lpszTitle) const;
	void SelectPanel(int index);
	void SelectPanel(LPCTSTR lpszName);
	void PrevPanel();
	void NextPanel();
	void InitPageList(LPCTSTR lpszRoot);
	int ShowLicense(LPCTSTR lpszXmlId, LPCTSTR lpszTitleId, LPCTSTR lpszTitle, 
		LPCTSTR lpszRichId, LPCTSTR lpszLicensePath);
	

private:
	std::wstring& promptOpenFolderVistaLater(std::wstring& folder_path, PCWSTR initPath, LPCTSTR lpszTitle) const;
	std::wstring& promptOpenFolderXp(std::wstring& folder_path, PCWSTR initPath, LPCTSTR lpszTitle) const;
	static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);
	void showMore(bool show);

public:
	std::hash_map<std::wstring, int> ctl_call_map_;

private:
	int cur_panel_;
	std::vector<std::pair<std::wstring, DM::DUIWindow*> > dummy_tab_ctl_;
	//CWndShadow wndShadow_;
};

class CNSDMMessageBox: public DM::DMHDialog
{
public:
	DECLARE_MESSAGE_MAP()
	DECLARE_EVENT_MAP()

	CNSDMMessageBox(LPCTSTR lpszTitleId, LPCTSTR lpszTitle, 
		LPCTSTR lpszContentId, LPCTSTR lpszContent, const SIZE& size, int type);
	~CNSDMMessageBox();
	BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);
	DMCode OnClickClose();
	DMCode OnClickOk();
	DMCode OnClickCancel();
	DMCode OnClickRetry();

private:
	std::wstring str_title_;
	std::wstring title_id_;
	std::wstring str_content_;
	std::wstring content_id_;
	int type_;
	int width_, height_;
};

class CNSDMLicenseWnd: public DM::DMHDialog
{
public:
	DECLARE_MESSAGE_MAP()
	DECLARE_EVENT_MAP()

	CNSDMLicenseWnd(LPCTSTR lpszTitleId, LPCTSTR lpszTitle, 
		LPCTSTR lpszRichId, LPCTSTR lpszLicensePath);
	BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);
	DMCode OnClickClose();
	DMCode OnClickOk();


private:
	std::wstring str_title_;
	std::wstring title_id_;
	std::wstring license_path_;
	std::wstring content_id_;
};