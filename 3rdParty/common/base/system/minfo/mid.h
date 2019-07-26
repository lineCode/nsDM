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
	// 当新mid所依靠的WMI不可用时，而且不能从注册表获取mid时，改为跟“PCYY的mid”一样（不再使用GUID）
	// 虽然这样会像“PCYY的mid”受USB等外设、虚拟设备、操作系统的影响，但避免“不能记录注册表于是更新GUID从而使得mid变化”
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
	bool					m_bInitCOM; // 配对CoInitialize和CoUninitialize
};
