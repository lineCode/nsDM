#pragma once

#include <string>
#include <vector>

struct IWbemLocator;
struct IWbemServices;

class CMid
{
public:
	CMid();
	~CMid();

public:
	std::string GetHiidoMid();
	std::wstring GetMid();
	// ����mid��������WMI������ʱ�����Ҳ��ܴ�ע����ȡmidʱ����Ϊ����PCYY��mid��һ��������ʹ��GUID��
	// ��Ȼ��������PCYY��mid����USB�����衢�����豸������ϵͳ��Ӱ�죬�����⡰���ܼ�¼ע������Ǹ���GUID�Ӷ�ʹ��mid�仯��
	std::string getYYInfoWithoutWMI();
	std::string getYYInfo();
	
private:
	typedef std::vector<std::string> STRVEC;

	void formatRegMid(std::string& strRegMid);

	std::string getCpuInfo();
	std::string getMacInfo();
	std::string getDiskInfo();
	std::string getBiosInfo();
	std::string getBoardInfo();

	bool initWMI();
	bool uninitWMI();
	std::string getWMIInfo(const wchar_t* pcwstrWQL,
		const wchar_t* pcwstrProp1,
		const wchar_t* pcwstrProp2,
		const wchar_t* pcwstrProp3);
	std::string getWMIInfoForMac();

	std::string getPermanentMac(const std::wstring& wstrPNPDeviceID);
	std::string formatMac(const unsigned char* pData, const unsigned long nLen);


	std::string getYYMacAllInfo();
	std::string getYYHDinfo();
	std::string getYYBiosInfo();

private:
	IWbemLocator*			m_pWbemLocator;
	IWbemServices*			m_pWbemServices;
	bool					m_bInitCOM; // ���CoInitialize��CoUninitialize
};
