#pragma once

//#include "ygmd5.h"
//#include "YGHttpUtility.h"
#include <map>

static std::wstring GetRegValue( HKEY hMainKey, LPCWSTR lpSubKey, LPCWSTR lpValueName )
{
	std::wstring strRet(L"");
	HKEY hKey = NULL;
	if(ERROR_SUCCESS == ::RegCreateKeyEx(hMainKey, lpSubKey, 0, NULL,
		REG_OPTION_NON_VOLATILE, KEY_WRITE | KEY_READ, NULL, &hKey, NULL))
	{
		DWORD dwType = REG_SZ;
		wchar_t *wszValue = NULL;
		DWORD dwValueSize = 0;
		if(ERROR_SUCCESS == ::RegQueryValueEx(hKey, lpValueName, NULL, &dwType, NULL, &dwValueSize))
		{
			wszValue = new wchar_t[dwValueSize];
			if(ERROR_SUCCESS == ::RegQueryValueEx(hKey, lpValueName, NULL, &dwType, (LPBYTE)wszValue, &dwValueSize))
			{
				strRet = wszValue;
			}
			delete wszValue;
		}

		::RegCloseKey(hKey);
		hKey = NULL;
	}
	return strRet;
}

static bool SetRegValue( HKEY hMainKey, LPCWSTR lpSubKey, LPCWSTR lpValueName, std::wstring strValue )
{
	bool bRet = false;
	HKEY hKey = NULL;
	if(ERROR_SUCCESS == ::RegCreateKeyEx(hMainKey, lpSubKey, 0, NULL,
		REG_OPTION_NON_VOLATILE, KEY_WRITE | KEY_READ, NULL, &hKey, NULL))
	{

		if(ERROR_SUCCESS == ::RegSetValueEx(hKey, lpValueName, NULL, REG_SZ, reinterpret_cast<LPBYTE>(const_cast<wchar_t*>(strValue.c_str())),
			sizeof(std::wstring::value_type) * (strValue.size() + 1)))
		{
			bRet = true;
		}

		::RegCloseKey(hKey);
		hKey = NULL;
	}
	return bRet;
}

//static char number_to_char(const long int NumIn)
//{
//	if (NumIn < 10) 
//		return (char)(NumIn+'0'); //如输入小于10则直接返回
//	else 
//		return (char)(NumIn-10+'a'); //如输入大于9则用字母表示，如'A'表示10，'Z'表示35
//}
//static int char_to_number(const char CharIn)
//{
//	if(CharIn>='a') return (CharIn-'a'+10);
//	else if (CharIn<'A') return (CharIn-'0');
//	else return (CharIn-'A'+10);
//}
//
//static std::string bytes_to_hex(const void* digest,int ilen)
//{
//	std::string sHex;
//	register unsigned char* pb=(unsigned char*)digest;
//	for(int i=0;i<ilen;i++)
//	{
//		sHex+=number_to_char(pb[i]>>4);
//		sHex+=number_to_char(pb[i]&0xF);
//	}
//	return sHex;
//}
//
//static std::string hex_to_bytes(const char* szInput)
//{
//	std::string sRet;
//	if (szInput == NULL ) return sRet;
//
//	int nSize = strlen(szInput) / 2;
//	unsigned char ch;
//	INT nPos;
//	for(int i=0;i<nSize;i++)
//	{
//		nPos = i * 2;
//		ch = char_to_number(szInput[nPos]) << 4;
//		ch += char_to_number(szInput[nPos + 1]);
//		sRet.push_back(ch);
//	}
//	return sRet;
//}
//
////获得串的MD5值
//static bool get_md5_bytes(void *vin,int ilen, unsigned char* vout)
//{
//	// 未验证过。by ZC. 2012-6-6 20:08.
//	//_ASSERT_EXPR(FALSE, L"get_md5_bytes");
//	if(vin==NULL || vout==NULL) return false;
//	YGMD5 coder;
//	coder.update((unsigned char*)vin,ilen);
//	coder.finalize();
//	coder.raw_digest(vout);
//	return true;
//}
//
//static std::string encode_to_md5(unsigned char* pbIn, unsigned int sizeIn)
//{
//	std::string str;
//	unsigned char cout[16]={0};
//	if(get_md5_bytes(pbIn,sizeIn,cout))
//		str=bytes_to_hex(cout,16);
//	return str;
//}
//
//static std::string encode_to_md5(std::string& str_in)
//{
//	int cbSize=str_in.size();
//	return encode_to_md5((unsigned char*)str_in.c_str(),cbSize);
//}
//
//static std::wstring encode_to_md5(std::wstring& str_in)
//{
//	int cbSize=str_in.size()*sizeof(wchar_t);
//	std::string str= encode_to_md5((unsigned char*)str_in.c_str(),cbSize);
//	std::wstring wstr(str.begin(),str.end());
//	return wstr;
//}
//
static std::string WStringToUtf8(std::wstring strIn)
{
	int nSize = WideCharToMultiByte(CP_UTF8, 0, strIn.c_str(), -1, NULL, NULL, NULL, NULL);
	char* buffer = new char[nSize+1];
	ZeroMemory(buffer,nSize+1);
	WideCharToMultiByte(CP_UTF8, 0, strIn.c_str(), -1, buffer, nSize, NULL, NULL);
	std::string strRet(buffer);
	delete[] buffer;
	return strRet;
}

static std::wstring Utf8ToWString(std::string strIn)
{
	int length = MultiByteToWideChar(CP_UTF8, 0, strIn.c_str(), -1, NULL, 0);
	WCHAR* buf = new WCHAR[length+1];
	ZeroMemory(buf, (length + 1) * sizeof(WCHAR));
	MultiByteToWideChar(CP_UTF8, 0, strIn.c_str(), -1, buf, length);
	std::wstring strRet(buf);
	delete[] buf;
	return strRet;
}

//static std::string URLEncode(LPCSTR pszUrl)
//{
//	static    char hex[] = "0123456789ABCDEF";  
//	std::string dst;  
//	if (!IsValidString(pszUrl)) return dst;
//
//	size_t nSize = strlen(pszUrl);
//	for (size_t i = 0; i < nSize && pszUrl[i] != 0; i++)  
//	{  
//		unsigned char ch = pszUrl[i];  
//		if (isalnum(ch) || ch == '.')  
//		{  
//			dst += ch;  
//		}  
//		else
//		{  
//			unsigned char c = static_cast<unsigned char>(pszUrl[i]);  
//			dst += '%';  
//			dst += hex[c >> 4];  
//			dst += hex[c % 16];  
//		}  
//	}  
//	return dst; 
//}


//static bool httpRequest( const wchar_t* szUrl, std::map<std::wstring,std::wstring> const& params, std::wstring& response, std::wstring& cookie, bool bIgnoreUnknownCa, DWORD nWaitTime )
//{
//	bool bRequestRet = false;
//
//	if ( szUrl != NULL ) {
//		YGHttpUtility httpUtility;
//		httpUtility.SetUrl( szUrl );
//		httpUtility.SetHeaders( L"Content-Type: application/x-www-form-urlencoded\r\n" );
//
//		if ( bIgnoreUnknownCa ) {
//			httpUtility.addSecurityFlags( SECURITY_FLAG_IGNORE_UNKNOWN_CA );
//		}
//
//		std::wstring requestParam = L"";
//		std::map<std::wstring,std::wstring>::const_iterator itParam  = params.begin();
//		std::map<std::wstring,std::wstring>::const_iterator endParam = params.end();
//		while( itParam != endParam ) {
//			if ( !requestParam.empty() ) {
//				requestParam += ( std::wstring( L"&" ) + ( itParam->first ) + L"=" + ( itParam->second ) );
//			} else {
//				requestParam = ( itParam->first ) + L"=" + ( itParam->second );
//			}
//			++itParam;
//		}
//
//		//		YPGLOG_WRITE(emYPGLOG_PRI_INFO, HelpFunc::toNarrowString( requestParam.c_str(), requestParam.length(), CP_UTF8 ).c_str() );
//
//		httpUtility.SetOption( WStringToUtf8( requestParam ).c_str() );
//
//		bRequestRet = httpUtility.SendRequest();
//
//		if (bRequestRet)
//		{
//			response = Utf8ToWString( std::string(reinterpret_cast<const char*>( httpUtility.GetResponse() ), httpUtility.GetResponseSize()) );
//			cookie = httpUtility.GetCookie();
//		}
//	}
//
//	return bRequestRet;
//}
