#include "mid.h"
#include "base/encrypt/md5.h"
//#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "cpuid.h"
#include <wbemidl.h>
#include <comdef.h>
#include <algorithm>
#include <functional>
#include <ntddndis.h>
//#include <winioctl.h>
#include <assert.h>
#include <nb30.h> // NetBIOS
#include <iphlpapi.h> // GetAdaptersInfo
#include "hdbios.h"
#include "util.h"


#pragma comment(lib, "wbemuuid.lib")

#define MIDREGKEYPATH ("Software\\duowan\\datareportsdk")
#define MIDREGKEYNAME ("mid_2")
#define MIDREGKEYLEN (34) // mid包括“32字符md5”和“后端可能出现的作为标识的1字符”和“字符串结束符”
#define MD5LENGTH (32)
#define MAXMACLENGTH (8)

#define  YPG_MID_REG_PATH          L"SOFTWARE\\duowan\\ygdata_report"
#define  YPG_MID_REG_PATH_HIIDO    L"SOFTWARE\\duowan\\ygdata_report_hiido"
#define  YPG_MID_REG_KEY           L"mid"

CMid::CMid()
	: m_pWbemLocator(NULL)
	, m_pWbemServices(NULL)
	, m_bInitCOM(false)
{
}

CMid::~CMid()
{
	uninitWMI();
}

std::string CMid::GetHiidoMid()
{
	std::string strInfo;
	bool bSucceed = false;

	try
	{
		bSucceed = initWMI();
		if (bSucceed)
		{
			strInfo.append(getCpuInfo());
			strInfo.append("|");
			strInfo.append(getMacInfo());
			strInfo.append("|");
			strInfo.append(getDiskInfo());
			strInfo.append("|");
			strInfo.append(getBiosInfo());
			strInfo.append("|");
			strInfo.append(getBoardInfo());

			uninitWMI();
		}
	}
	catch (...)
	{
		assert(false);
		bSucceed = false;
	}

	// 获取注册表的mid，以便使用或比较
	std::string strRegMid;
	try
	{
		HKEY hKey = NULL;
		LONG lResult = RegOpenKeyExA(HKEY_CURRENT_USER, MIDREGKEYPATH, 0, KEY_QUERY_VALUE, &hKey);
		if ((ERROR_SUCCESS == lResult) && hKey)
		{
			DWORD dwType = 0;
			char pData[MIDREGKEYLEN] = {0};
			DWORD dwLen = MIDREGKEYLEN;
			lResult = RegQueryValueExA(hKey, MIDREGKEYNAME, NULL, &dwType, (BYTE*)pData, &dwLen);
			if ((ERROR_SUCCESS == lResult) &&
				(REG_SZ == dwType) &&
				(dwLen > 0) &&
				(dwLen <= MIDREGKEYLEN) &&
				(0 == pData[MIDREGKEYLEN - 1]))
			{
				strRegMid.assign(pData);
				// 如果从注册表获取的mid不符合格式要求，就会被清空
				formatRegMid(strRegMid);
			}

			lResult = RegCloseKey(hKey);
			assert(ERROR_SUCCESS == lResult);
		}
	}
	catch (...)
	{
		assert(false);
		strRegMid.clear();
	}

	// 当新mid所依靠的WMI不可用时，而且不能从注册表获取mid时，改为跟“PCYY的mid”一样（不再使用GUID）
	// 虽然这样会像“PCYY的mid”受USB等外设、虚拟设备、操作系统的影响，但避免“不能记录注册表于是更新GUID从而使得mid变化”
	// 目前发现一般情况下Guest用户不能使用WMI
	if (!bSucceed)
	{
		if (!strRegMid.empty())
			return strRegMid;
		else
			strInfo = getYYInfoWithoutWMI();
	}

	base::CMD5 md5(strInfo);
	std::string strMid = md5.toString();

	// hiido_mid受限而跟PCYY一样时，结果后端增加减号作为标识
	if (!bSucceed)
		strMid.push_back('-');

	// 把新的mid保存注册表
	if (strRegMid.compare(strMid))
	{
		try
		{
			HKEY hKey = NULL;
			LONG lResult = RegCreateKeyExA(HKEY_CURRENT_USER, MIDREGKEYPATH, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL);
			if ((ERROR_SUCCESS == lResult) && hKey)
			{
				lResult = RegSetValueExA(hKey, MIDREGKEYNAME, 0, REG_SZ, (const BYTE*)strMid.c_str(), (DWORD)(strMid.size() + 1));
				assert(ERROR_SUCCESS == lResult);

				lResult = RegCloseKey(hKey);
				assert(ERROR_SUCCESS == lResult);
			}
		}
		catch (...)
		{
			assert(false);
		}
	}

	return strMid;
}

// mid包括“32字符md5”和“后端可能出现的作为标识的1字符”和“字符串结束符”
// 如果从注册表获取的mid不符合格式要求，就会被清空
void CMid::formatRegMid(std::string& strRegMid)
{
	if ((strRegMid.size() >= MD5LENGTH) &&
		(strRegMid.size() <= (MIDREGKEYLEN - 1)))
	{
		unsigned int nPos = 0;
		std::string::const_iterator it = strRegMid.begin();
		for (; (it != strRegMid.end()) && (nPos < MD5LENGTH); ++it, ++nPos)
		{
			const char ch = *it;
			if (!((ch >= '0') && (ch <= '9')) && !((ch >= 'a') && (ch <= 'f')))
				break;
		} // for

		if (MD5LENGTH == nPos)
			return;
	}

	strRegMid.clear();
}

// CPU：CPU的型号和功能信息
std::string CMid::getCpuInfo()
{
	WORD idext = 0;
	DWORD features = 0;

	try
	{
		idext = cpuid_hiido::wincpuidext();
	}
	catch (...)
	{
	}

	try
	{
		features = cpuid_hiido::wincpufeatures();
	}
	catch (...)
	{
	}

	// 12位的cpuid信息
	char buf[32] = {0};
	sprintf_s(buf, 32, "%.4x%.8x", idext, (unsigned int)features);
	return buf;
}

// 网卡：所有启用的内置（PCI）物理网卡原始MAC地址（全大写字母或数字）的升序排列
std::string CMid::getMacInfo()
{
	return getWMIInfoForMac();
}

// 硬盘：所有内置（IDE、SCSI）硬盘的“型号和序列号”（全大写字母或数字）的升序排列
std::string CMid::getDiskInfo()
{
	return getWMIInfo(L"SELECT * FROM Win32_DiskDrive WHERE InterfaceType = 'IDE' OR InterfaceType = 'SCSI'", L"Model", L"SerialNumber", NULL);
}

// BIOS：BIOS的厂商和序列号（全大写字母或数字）
std::string CMid::getBiosInfo()
{
	return getWMIInfo(L"SELECT * FROM Win32_BIOS", L"Manufacturer", L"SerialNumber", NULL);
}

// 主板：主板的厂商、产品和序列号（全大写字母或数字）
std::string CMid::getBoardInfo()
{
	return getWMIInfo(L"SELECT * FROM Win32_BaseBoard", L"Manufacturer", L"Product", L"SerialNumber");
}

bool CMid::initWMI()
{
	bool bSucceed = uninitWMI();
	assert(!bSucceed);
	assert(!m_pWbemLocator && !m_pWbemServices);

	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	m_bInitCOM = SUCCEEDED(hr);

	hr = CoInitializeSecurity(NULL,
		-1,
		NULL,
		NULL,
		RPC_C_AUTHN_LEVEL_PKT,
		RPC_C_IMP_LEVEL_IMPERSONATE,
		NULL,
		EOAC_NONE,
		NULL);
	assert(SUCCEEDED(hr) || (RPC_E_TOO_LATE == hr));
	if (FAILED(hr) && (RPC_E_TOO_LATE != hr))
	{
		if (m_bInitCOM)
		{
			CoUninitialize();
			m_bInitCOM = false;
		}
		return false;
	}

	IWbemLocator* pWbemLocator = NULL;
	hr = CoCreateInstance(CLSID_WbemLocator,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator,
		(void**)&pWbemLocator);
	assert(SUCCEEDED(hr) && pWbemLocator);
	if (FAILED(hr) || !pWbemLocator)
	{
		if (m_bInitCOM)
		{
			CoUninitialize();
			m_bInitCOM = false;
		}
		return false;
	}

	IWbemServices* pWbemServices = NULL;
	hr = pWbemLocator->ConnectServer(_bstr_t(L"ROOT\\CIMV2"),
		NULL,
		NULL,
		NULL,
		0,
		NULL,
		NULL,
		&pWbemServices);
	assert(SUCCEEDED(hr) && pWbemServices);
	if (FAILED(hr) || !pWbemServices)
	{
		pWbemLocator->Release();
		if (m_bInitCOM)
		{
			CoUninitialize();
			m_bInitCOM = false;
		}
		return false;
	}

	hr = CoSetProxyBlanket(pWbemServices,
		RPC_C_AUTHN_WINNT,
		RPC_C_AUTHZ_NONE,
		NULL,
		RPC_C_AUTHN_LEVEL_CALL,
		RPC_C_IMP_LEVEL_IMPERSONATE,
		NULL,
		EOAC_NONE);
	assert(SUCCEEDED(hr));
	if (FAILED(hr))
	{
		pWbemServices->Release();
		pWbemLocator->Release();
		if (m_bInitCOM)
		{
			CoUninitialize();
			m_bInitCOM = false;
		}
		return false;
	}

	m_pWbemLocator = pWbemLocator;
	m_pWbemServices = pWbemServices;

	return true;
}

bool CMid::uninitWMI()
{
	if (!m_pWbemLocator && !m_pWbemServices)
		return false;

	assert(m_pWbemLocator && m_pWbemServices);

	if (m_pWbemServices)
	{
		m_pWbemServices->Release();
		m_pWbemServices = NULL;
	}

	if (m_pWbemLocator)
	{
		m_pWbemLocator->Release();
		m_pWbemLocator = NULL;
	}

	if (m_bInitCOM)
	{
		CoUninitialize();
		m_bInitCOM = false;
	}

	return true;
}

// 全大写字母或数字
static std::string formatString(const BSTR& wstrOrig)
{
	std::string strFormat;

	if (!wstrOrig)
		return strFormat;

	const UINT len = SysStringLen(wstrOrig);
	for (UINT index = 0; index < len; ++index)
	{
		wchar_t ch = wstrOrig[index];
		if (((ch >= L'0') && (ch <= L'9')) ||
			((ch >= L'A') && (ch <= L'Z')))
			strFormat.push_back((char)ch);
		else if ((ch >= L'a') && (ch <= L'z'))
			strFormat.push_back((char)(ch - L'a' + L'A'));
	} // for

	return strFormat;
}

std::string CMid::getWMIInfo(const wchar_t* pcwstrWQL,
	const wchar_t* pcwstrProp1,
	const wchar_t* pcwstrProp2,
	const wchar_t* pcwstrProp3)
{
	std::string strResult;

	assert(m_pWbemServices);
	if (!pcwstrWQL ||
		!(*pcwstrWQL) ||
		!m_pWbemServices)
		return strResult;

	IEnumWbemClassObject* pEnumWbemClassObject = NULL;
	HRESULT hr = m_pWbemServices->ExecQuery(_bstr_t(L"WQL"),
		_bstr_t(pcwstrWQL),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL,
		&pEnumWbemClassObject);
	assert(SUCCEEDED(hr) && pEnumWbemClassObject);
	if (FAILED(hr) || !pEnumWbemClassObject)
		return strResult;

	STRVEC resultVec; // 用于排序

	while (true)
	{
		IWbemClassObject* pWbemClassObject = NULL;
		ULONG uReturned = 0;
		hr = pEnumWbemClassObject->Next(WBEM_INFINITE,
			1,
			&pWbemClassObject,
			&uReturned);
		assert(SUCCEEDED(hr));
		if ((WBEM_S_NO_ERROR != hr) || !pWbemClassObject || !uReturned)
			break;

		std::string strValueAll;

		if (pcwstrProp1 && (*pcwstrProp1))
		{
			_variant_t varValue;
			hr = pWbemClassObject->Get(pcwstrProp1,
				0,
				&varValue,
				NULL,
				NULL);
			assert(SUCCEEDED(hr));

			if (SUCCEEDED(hr) && (VT_BSTR == V_VT(&varValue)))
			{
				std::string strValue = formatString(V_BSTR(&varValue));
				strValueAll.append(strValue);
			}
		}

		if (pcwstrProp2 && (*pcwstrProp2))
		{
			_variant_t varValue;
			hr = pWbemClassObject->Get(pcwstrProp2,
				0,
				&varValue,
				NULL,
				NULL);
			assert(SUCCEEDED(hr));

			if (SUCCEEDED(hr) && (VT_BSTR == V_VT(&varValue)))
			{
				std::string strValue = formatString(V_BSTR(&varValue));
				strValueAll.append(strValue);
			}
		}

		if (pcwstrProp3 && (*pcwstrProp3))
		{
			_variant_t varValue;
			hr = pWbemClassObject->Get(pcwstrProp3,
				0,
				&varValue,
				NULL,
				NULL);
			assert(SUCCEEDED(hr));

			if (SUCCEEDED(hr) && (VT_BSTR == V_VT(&varValue)))
			{
				std::string strValue = formatString(V_BSTR(&varValue));
				strValueAll.append(strValue);
			}
		}

		if (!strValueAll.empty())
			resultVec.push_back(strValueAll);

		pWbemClassObject->Release();
		pWbemClassObject = NULL;
	} // while

	pEnumWbemClassObject->Release();

	// 升序排列
	std::sort(resultVec.begin(), resultVec.end(), std::less<std::string>());

	STRVEC::const_iterator it = resultVec.begin();
	for (; it != resultVec.end(); ++it)
	{
		strResult.append(*it);
	} // for

	return strResult;
}

std::string CMid::getWMIInfoForMac()
{
	std::string strResult;

	assert(m_pWbemServices);
	if (!m_pWbemServices)
		return strResult;

	IEnumWbemClassObject* pEnumWbemClassObject = NULL;
	HRESULT hr = m_pWbemServices->ExecQuery(_bstr_t(L"WQL"),
		_bstr_t(L"SELECT * FROM Win32_NetworkAdapter"),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL,
		&pEnumWbemClassObject);
	assert(SUCCEEDED(hr) && pEnumWbemClassObject);
	if (FAILED(hr) || !pEnumWbemClassObject)
		return strResult;

	STRVEC resultVec; // 用于排序

	while (true)
	{
		IWbemClassObject* pWbemClassObject = NULL;
		ULONG uReturned = 0;
		hr = pEnumWbemClassObject->Next(WBEM_INFINITE,
			1,
			&pWbemClassObject,
			&uReturned);
		assert(SUCCEEDED(hr));
		if ((WBEM_S_NO_ERROR != hr) || !pWbemClassObject || !uReturned)
			break;

		// PCI网卡
		// 因为Windows2000不支持WQL的LIKE，所以额外实现
		bool bPCI = false;
		std::wstring wstrPNPDeviceID;

		{
			_variant_t varValue;
			hr = pWbemClassObject->Get(L"PNPDeviceID",
				0,
				&varValue,
				NULL,
				NULL);
			assert(SUCCEEDED(hr));

			if (SUCCEEDED(hr) && (VT_BSTR == V_VT(&varValue)) && V_BSTR(&varValue))
			{
				wstrPNPDeviceID.assign(V_BSTR(&varValue), SysStringLen(V_BSTR(&varValue)));

				bPCI = (0 == _wcsnicmp(wstrPNPDeviceID.c_str(), L"PCI", 3));
			}
		}

		if (!bPCI)
			continue;

		// 原始MAC地址
		std::string strMac = getPermanentMac(wstrPNPDeviceID);

		if (!strMac.empty())
			resultVec.push_back(strMac);

		pWbemClassObject->Release();
		pWbemClassObject = NULL;
	} // while

	pEnumWbemClassObject->Release();

	// 升序排列
	std::sort(resultVec.begin(), resultVec.end(), std::less<std::string>());

	STRVEC::const_iterator it = resultVec.begin();
	for (; it != resultVec.end(); ++it)
	{
		strResult.append(*it);
	} // for

	return strResult;
}

std::string CMid::getPermanentMac(const std::wstring& wstrPNPDeviceID)
{
	std::string strPermanentMac;

	if (wstrPNPDeviceID.empty())
		return strPermanentMac;

	std::wstring wstrDevicePath(L"\\\\.\\");
	wstrDevicePath.append(wstrPNPDeviceID);
	std::replace(wstrDevicePath.begin() + 4, wstrDevicePath.end(), L'\\', L'#');
	wstrDevicePath.append(L"#{ad498944-762f-11d0-8dcb-00c04fc3358c}");

	try
	{
		HANDLE hDeviceFile = CreateFileW(wstrDevicePath.c_str(),
			0,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			0,
			NULL);

		if (INVALID_HANDLE_VALUE != hDeviceFile)
		{
			DWORD dwID = OID_802_3_PERMANENT_ADDRESS;
			unsigned char ucData[MAXMACLENGTH] = {0};
			DWORD dwBytesReturned = 0;

			BOOL bSucceed = DeviceIoControl(hDeviceFile,
				IOCTL_NDIS_QUERY_GLOBAL_STATS,
				&dwID,
				sizeof(dwID),
				ucData,
				sizeof(ucData),
				&dwBytesReturned,
				NULL);

			if (bSucceed)
				strPermanentMac = formatMac(ucData, dwBytesReturned);

			CloseHandle(hDeviceFile);
		}
	}
	catch (...)
	{
		assert(false);
	}

	return strPermanentMac;
}

std::string CMid::formatMac(const unsigned char* pData, const unsigned long nLen)
{
	std::string strMac;

	for (DWORD index = 0; pData && (index < nLen) && (index < MAXMACLENGTH); ++index)
	{
		BYTE ch = (pData[index] & 0xF0) >> 4;
		strMac.push_back((ch < 10) ? ('0' + ch) : ('A' + ch - 10));
		ch = (pData[index] & 0x0F);
		strMac.push_back((ch < 10) ? ('0' + ch) : ('A' + ch - 10));
	} // for

	return strMac;
}

std::string CMid::getYYInfoWithoutWMI()
{
	std::string info;
	std::string temp;

	// cpu all info 包含了 cpu info
	temp = getCpuInfo();
	info += temp;

	// mac all info 包含了 mac info
	temp = getYYMacAllInfo();
	info += temp;

	// hd
	temp.clear();
	try
	{
		temp = getYYHDinfo();
	}
	catch (...)
	{
	}
	info += temp;

	// bios
	temp.clear();
	try
	{
		temp = getYYBiosInfo();
	}
	catch (...)
	{
	}
	info += temp;

	return info;
}

static UCHAR dwNetbios(PNCB pncb)
{
	UCHAR ret = NRC_PENDING;
	HMODULE h = LoadLibraryA("NETAPI32.DLL");
	if (h)
	{
		void* pfNetbios = (void*)GetProcAddress(h, "Netbios");
		if (pfNetbios)
		{
			ret = ((UCHAR (WINAPI *)(PNCB))pfNetbios)(pncb);
		}

		FreeLibrary(h);
	}

	return ret;
}

typedef struct _ASTAT_
{
	ADAPTER_STATUS adapt;
	NAME_BUFFER NameBuff[30];
} ASTAT, * PASTAT;

static bool getMac(int lana_num, std::string& info)
{
	// 定义一个存放返回网卡信息的变量
	ASTAT Adapter = {0};
	NCB ncb = {0};
	memset(&ncb, 0, sizeof(ncb));
	ncb.ncb_command = NCBRESET;
	ncb.ncb_lana_num = lana_num ;// 指定网卡号
	// 首先对选定的网卡发送一个NCBRESET命令,以便进行初始化
	UCHAR uRetCode = dwNetbios(&ncb);
	memset(&ncb, 0, sizeof(ncb));

	ncb.ncb_command = NCBASTAT;

	ncb.ncb_lana_num = lana_num; // 指定网卡号

	strcpy_s((char*)ncb.ncb_callname, NCBNAMSZ, "*");

	ncb.ncb_buffer = (unsigned char*)&Adapter; // 指定返回的信息存放的变量

	ncb.ncb_length = sizeof(Adapter);

	// 接着,可以发送NCBASTAT命令以获取网卡的信息
	uRetCode = dwNetbios(&ncb);
	if (0 != uRetCode)
	{
		return false;
	}

	// 把网卡MAC地址格式化成常用的16进制形式,如0010A4E45802
	char buf[64] = {0};
	int len = sprintf_s(buf, 64, "%02d%02X%02X%02X%02X%02X%02X",
		lana_num % 100,
		Adapter.adapt.adapter_address[0],
		Adapter.adapt.adapter_address[1],
		Adapter.adapt.adapter_address[2],
		Adapter.adapt.adapter_address[3],
		Adapter.adapt.adapter_address[4],
		Adapter.adapt.adapter_address[5]);
	info.append(buf, len);
	return true;
}

static ULONG dwGetAdaptersInfo(IN PIP_ADAPTER_INFO AdapterInfo,
	IN OUT PULONG SizePointer)
{
	ULONG ret = ERROR_NOT_SUPPORTED;
	HMODULE h = LoadLibraryA("IPHLPAPI.DLL");
	if (h)
	{
		void* pfGetAdaptersInfo = (void*)GetProcAddress(h, "GetAdaptersInfo");
		if (pfGetAdaptersInfo)
		{
			ret = ((ULONG (WINAPI *)(PIP_ADAPTER_INFO, PULONG))pfGetAdaptersInfo)(AdapterInfo, SizePointer);
		}

		FreeLibrary(h);
	}

	return ret;
}

static BOOL getMacWithAdapter(IP_ADAPTER_INFO* AdapterInfo, DWORD& dwSize)
{
	return (ERROR_SUCCESS == dwGetAdaptersInfo( // Call GetAdapterInfo
		AdapterInfo, // [out] buffer to receive data
		&dwSize)) ? TRUE : FALSE; // [in] size of receive data buffer
}

// Fetches the MAC address and prints it
static BOOL getMacStringWithAdapter(std::string& address, BOOL bALL)
{
	IP_ADAPTER_INFO AdapterInfo[32] = {0}; // Allocate information
	DWORD dwBufLen = sizeof(AdapterInfo); // Save memory size of buffer

	if (!getMacWithAdapter(AdapterInfo, dwBufLen))
	{
		return FALSE;
	}

	// valid, no buffer overflow
	PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo; // Contains pointer to		
	do 
	{
		char byte[10] = {0};
		sprintf_s(byte, 10, "%02d", pAdapterInfo->Index % 100);
		address += byte;

		for (UINT i = 0; i < pAdapterInfo->AddressLength; ++i)
		{	
			byte[2] = byte[3] = '\0';
			sprintf_s(byte, 10, "%02X", pAdapterInfo->Address[i]);
			address += byte;
		}

		if(!bALL)
		{// once get one
			break;
		}
		pAdapterInfo = pAdapterInfo->Next; // Progress through
	} while (pAdapterInfo); // Terminate if last adapter

	return TRUE;
}

std::string CMid::getYYMacAllInfo()
{
	std::string info; // 返回值

	NCB ncb = {0};
	LANA_ENUM lana_enum = {0};
	memset(&ncb, 0, sizeof(ncb));
	ncb.ncb_command = NCBENUM;
	ncb.ncb_buffer = (unsigned char*)&lana_enum;
	ncb.ncb_length = sizeof(lana_enum);

	// 向网卡发送NCBENUM命令,以获取当前机器的网卡信息,如有多少个网卡、每张网卡的编号等
	UCHAR uRetCode = dwNetbios(&ncb);

	if (0 == uRetCode)
	{
		// 对每一张网卡,以其网卡编号为输入编号,获取其MAC地址
		for (int i = 0; i < lana_enum.length; ++i)
		{
			getMac(lana_enum.lana[i], info);
		}
	}
	else
	{
		getMacStringWithAdapter(info, TRUE);
	}

	return info;
}

std::string CMid::getYYHDinfo()
{
	std::string info; // 返回值

	BYTE szSystemInfo[4096] = {0};
	UINT uSystemInfoLen = 0;

	{
		OSVERSIONINFO ovi = {0};
		ovi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx(&ovi);

		if (VER_PLATFORM_WIN32_NT == ovi.dwPlatformId)
		{
			// Only Windows 2000, Windows XP, Windows Server 2003...
			if (!hdbios_hiido::WinNTHDSerialNumAsPhysicalRead(szSystemInfo, &uSystemInfoLen, 1024))
			{
				hdbios_hiido::WinNTHDSerialNumAsScsiRead(szSystemInfo, &uSystemInfoLen, 1024);
			}
		}
	}

	if (uSystemInfoLen > 0)
	{
		char* temp = new char[uSystemInfoLen + 1];
		int count = 0;
		for (int i = uSystemInfoLen - 1; i > 0; --i)
		{
			if (' ' != szSystemInfo[i])
			{
				temp[count] = szSystemInfo[i];
				++count;
			}
		}
		temp[count] = 0;
		info = temp;
		delete[] temp;
	}
	return info;
}

std::string CMid::getYYBiosInfo()
{
	std::string info; // 返回值

	LARGE_INTEGER so = {0};
	so.LowPart = 0x000f0000;
	so.HighPart = 0x00000000;
	SIZE_T ssize = 0xffff;
	wchar_t strPH[30] = L"\\device\\physicalmemory";

	DWORD ba = 0;

	UNICODE_STRING struniph = {0};
	struniph.Buffer = (wchar_t*)strPH;
	struniph.Length = 0x2c;
	struniph.MaximumLength = 0x2e;

	OBJECT_ATTRIBUTES obj_ar = {0};
	obj_ar.Attributes = 64;
	obj_ar.Length = 24;
	obj_ar.ObjectName = &struniph;
	obj_ar.RootDirectory = 0;
	obj_ar.SecurityDescriptor = 0;
	obj_ar.SecurityQualityOfService = 0;

	HMODULE hinstLib = LoadLibraryA("ntdll.dll");
	ZWOS ZWopenS = (ZWOS)GetProcAddress(hinstLib, "ZwOpenSection");
	ZWMV ZWmapV = (ZWMV)GetProcAddress(hinstLib, "ZwMapViewOfSection");
	ZWUMV ZWunmapV = (ZWUMV)GetProcAddress(hinstLib, "ZwUnmapViewOfSection");
	//调用函数，对物理内存进行映射
	HANDLE hSection = NULL;
	if (0 == ZWopenS(&hSection, 4, &obj_ar))
	{
		if (
			0 == ZWmapV(
			(HANDLE)hSection,     //打开Section时得到的句柄
			(HANDLE)(DWORD_PTR)0xFFFFFFFF, //将要映射进程的句柄，
			&ba,                  //映射的基址
			0,
			0xFFFF,               //分配的大小
			&so,                  //物理内存的地址
			&ssize,               //指向读取内存块大小的指针
			1,                    //子进程的可继承性设定
			0,                    //分配类型
			2                     //保护类型
			))
			//执行后会在当前进程的空间开辟一段64k的空间，并把f000:0000到f000:ffff处的内容映射到这里
			//映射的基址由ba返回,如果映射不再有用,应该用ZwUnmapViewOfSection断开映射
		{
			BYTE* pBiosSerial = (BYTE*)(DWORD_PTR)ba;
			UINT uBiosSerialLen = hdbios_hiido::FindAwardBios(&pBiosSerial);
			if (uBiosSerialLen == 0U)
			{
				uBiosSerialLen = hdbios_hiido::FindAmiBios(&pBiosSerial);
				if (uBiosSerialLen == 0U)
				{
					uBiosSerialLen = hdbios_hiido::FindPhoenixBios(&pBiosSerial);
				}
			}
			if (uBiosSerialLen != 0U)
			{
				info.append((const char*)pBiosSerial, uBiosSerialLen);
			}
			ZWunmapV((HANDLE)(DWORD_PTR)0xFFFFFFFF, (void*)(DWORD_PTR)ba);
		}
	}
	return info;
}

std::wstring CMid::GetMid()
{
	// 优先从注册表读取MID
	std::wstring strMid = L"";
	
	strMid = GetRegValue(HKEY_CURRENT_USER, YPG_MID_REG_PATH ,YPG_MID_REG_KEY);
	if (strMid.empty())
	{
		strMid = GetRegValue(HKEY_CURRENT_USER, YPG_MID_REG_PATH_HIIDO , YPG_MID_REG_KEY);
	}
	if (strMid.empty())
	{
		// 从注册表读取不到则重新计算MID
		std::string strTmpMid = getYYInfo();
		base::CMD5 md5(strTmpMid);
		strMid = Utf8ToWString(md5.toString());

		if (!strMid.empty())
		{
			// 将计算到的MID保存到注册表
			SetRegValue(HKEY_CURRENT_USER,YPG_MID_REG_PATH ,YPG_MID_REG_KEY,strMid);
			SetRegValue(HKEY_CURRENT_USER, YPG_MID_REG_PATH_HIIDO ,YPG_MID_REG_KEY,strMid);
		}
	}
	return strMid;
}

std::string CMid::getYYInfo()
{
	std::string info;
	std::string temp,tmpWmi;

	// cpu all info 包含了 cpu info
	temp = getCpuInfo();
	info += temp;

	// mac all info 包含了 mac info
	temp = getYYMacAllInfo();
	info += temp;

	// wmi
	bool bSucceed = initWMI();
	if (bSucceed)
	{
		tmpWmi = getWMIInfo(L"SELECT * FROM Win32_LogicalDisk",L"VolumeSerialNumber",NULL,NULL);
		info += tmpWmi;
		tmpWmi = getWMIInfo(L"SELECT * FROM Win32_OperatingSystem",L"SerialNumber",NULL,NULL);
		info += tmpWmi;
		tmpWmi = getWMIInfo(L"SELECT * FROM Win32_DiskDrive",L"PNPDeviceID",NULL,NULL);
		info += tmpWmi;
		tmpWmi = getWMIInfo(L"SELECT * FROM Win32_SoundDevice",L"PNPDeviceID",NULL,NULL);
		info += tmpWmi;
		tmpWmi = getWMIInfo(L"SELECT * FROM Win32_VideoController",L"PNPDeviceID",NULL,NULL);
		info += tmpWmi;
	}

	// hd
	temp.clear();
	try
	{
		temp = getYYHDinfo();
	}
	catch (...)
	{
	}
	info += temp;

	// bios
	temp.clear();
	try
	{
		temp = getYYBiosInfo();
	}
	catch (...)
	{
	}
	info += temp;

	return info;
}
