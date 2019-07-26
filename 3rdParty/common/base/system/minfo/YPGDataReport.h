#pragma once
#include "ypgdatareport_framework.h"

#include <string>
#include <map>
#include <deque>

#include "ypglock.h"
#include <vector>

// 此类是从 YPGDataReport.dll 导出的
class YPGDATAREPORT_API YPGDataReport {
	struct struct_session_data 
	{
		//初始的主字段
		//std::wstring strAct;
		std::wstring strPro;
		std::wstring strMid;
		std::wstring strHiidoMid;
		std::wstring strIve;
		std::wstring strUve;
		std::wstring strOs;
		std::wstring strBve;
		std::wstring strSre;
		std::wstring strSco;
		std::wstring strLla;
		//std::wstring strIsp;
		//初始的扩展字段
		std::wstring strDty;
		std::wstring strCha;
		std::wstring strChannelDesc;
		std::wstring strSdkVer;
		std::wstring strMachine;
		std::wstring strSessionId;
		//登录相关的主字段
		std::wstring strUid;
		std::wstring strPas;
		std::wstring strRso;
		//登录相关的扩展字段
		std::wstring strRsoDesc;
		std::wstring strRef;
		std::wstring strRefDesc;
		std::wstring strGam;
		std::wstring strGse;
		std::wstring strMpassport;
		std::wstring strTraceId;
		std::wstring strExtra;
		//旧规范相关的扩展字段
		std::wstring strPid;
	};

	struct struct_post_data
	{
		//主字段
		std::wstring strTime;
		std::wstring strKey;
		std::wstring strAti;
		UINT64 dwDur;
		//扩展字段
		std::wstring strEid;
		std::wstring strEidDesc;
		std::wstring strContent;
		std::map<std::wstring, std::wstring> mapExtra;
		//旧规范相关的扩展字段
		std::wstring strOAct;
		std::wstring strPid;
		std::wstring strFormat;		//内容规范 0是旧规范转换而来 1是对应新规范

		struct_post_data() 
			: dwDur(0)
		{}

		void clear()
		{dwDur = 0; strTime.clear(); strKey.clear(); strAti.clear(); strEid.clear(); strEidDesc.clear(); strContent.clear(); mapExtra.clear();}
	};
public:
	YPGDataReport(void);
	~YPGDataReport(void);

	bool Init(LPCWSTR pro, LPCWSTR ive, LPCWSTR dty, LPCWSTR cha, LPCWSTR channel_desc = NULL);
	bool InitSession(LPCWSTR pas, LPCWSTR uid, LPCWSTR rso, LPCWSTR rso_desc, LPCWSTR ref = NULL, LPCWSTR ref_desc = NULL, LPCWSTR gam = NULL, LPCWSTR gse = NULL, LPCWSTR mpassport = NULL, LPCWSTR trace_id = NULL, LPCWSTR extra = NULL);
	bool InitSessionWithPid(LPCWSTR pas, LPCWSTR uid, LPCWSTR rso, LPCWSTR rso_desc, LPCWSTR pid, LPCWSTR gam = NULL, LPCWSTR gse = NULL, LPCWSTR mpassport = NULL, LPCWSTR trace_id = NULL, LPCWSTR extra = NULL);
	void Start();
	bool StopSync();
	void Stop();
	bool PostEvent(LPCWSTR eid, LPCWSTR eid_desc);
	bool PostEvent(LPCWSTR eid, LPCWSTR eid_desc, UINT64 dur);
	bool PostEvent(LPCWSTR eid, LPCWSTR eid_desc, LPCWSTR content);
	bool PostEvent(LPCWSTR eid, LPCWSTR eid_desc, LPCWSTR content, UINT64 dur);
	bool PostEventOldFormat(LPCWSTR oact, LPCWSTR pid);
	bool PostEvent(struct_post_data data);

	bool GetMachineInfoReady();
	void SetMachineInfoReady(bool bOk);

	bool GetSessionOk();
	void SetSessionOk(bool bOk);

	bool GetQuit();
	void SetQuit(bool bOk);

	bool IsQueueEmpty();
	void PushQueue(struct_post_data data);
	bool PopQueue(struct_post_data &data);
	bool PopQueueLast(struct_post_data &data);

protected:
	static unsigned __stdcall InitMachineInfoThread(void *pVoid);
	void InitMachineInfo();

	std::wstring GetMid();
	std::wstring GetHiidoMid();

	static unsigned __stdcall PostDataThread(void *pVoid);
	void PostDataProc();
	std::wstring MakeUrl(struct_post_data data);
	bool PostData(struct_post_data data);
	bool PostData(std::wstring strUrl);

	void PostAsync(std::vector<std::wstring> vecUrl);
	static unsigned __stdcall PostQuitDataThread(void *pVoid);

	void AddTimeValue(struct_post_data &data);
	UINT64 GetTickNow();
private:
	struct_session_data session_data; 

	YPGLock session_lock;
	YPGLock machine_lock;
	YPGLock session_lock1;
	YPGLock quit_lock;
	YPGLock queue_lock;
	bool m_bMachineInfoReady;
	bool m_bSessionOk;
	bool m_bQuit;
	UINT64 m_dwStartTick;
	UINT64 m_dwHeartbeatTick;

	HANDLE m_hPostThread;
	std::deque<struct_post_data> m_queue;
	HANDLE m_hThreadQuitEvent;

	HANDLE m_hMidThread;
	HANDLE m_hMidEvent;
};
