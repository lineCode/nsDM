#pragma once

//#include <xstring>
#include <string>
namespace base
{
	
	//url±‡¬Î¿‡
	class CUrlEncode
	{
	public:
		void StringToWstring(std::wstring& szDst, std::string str)
		{
			std::string temp = str;
			int len = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)temp.c_str(), -1, NULL, 0);
			wchar_t * wszUtf8 = new wchar_t[len + 1];
			memset(wszUtf8, 0, len * 2 + 2);
			MultiByteToWideChar(CP_ACP, 0, (LPCSTR)temp.c_str(), -1, (LPWSTR)wszUtf8, len);
			szDst = wszUtf8;
			std::wstring r = wszUtf8;
			delete[] wszUtf8;
		}
	
		static std::string EscapeURL(const WCHAR* wszUrl)
		{
			int nNeedLen = WideCharToMultiByte(CP_UTF8, 0, wszUrl, -1, NULL, 0, NULL,NULL);
			char* pszUrl = new char[nNeedLen + 1];
			if (pszUrl == NULL)
				return NULL;
			ZeroMemory(pszUrl, nNeedLen + 1);	
			WideCharToMultiByte(CP_UTF8, 0, wszUrl, -1, pszUrl, nNeedLen, NULL,NULL);
	
			std::string result = "";
			for ( unsigned int i=0; i<strlen(pszUrl); i++ )
			{
				char c = pszUrl[i];
				if (
					( '0'<=c && c<='9' ) ||
					( 'a'<=c && c<='z' ) ||
					( 'A'<=c && c<='Z' ) ||
					c=='/' || c=='.'
					) 
				{
					result += c;
				} 
				else 
				{
					int j = (short int)c;
					if ( j < 0 ) 
					{
						j += 256;
					}
					int i1, i0;
					i1 = j / 16;
					i0 = j - i1*16;
					result += '%';
					result += dec2hexChar(i1);
					result += dec2hexChar(i0);
				}
			}
			delete []pszUrl;
			return result;
		}
	
		static std::wstring DescapeURL(const char* URL) {	
	
			std::string result = "";
			unsigned int nUrlLen = strlen(URL);
			for ( unsigned int i=0; i< nUrlLen; i++ ) {
				char c = URL[i];
				if ( c != '%' ) {
					result += c;
				} else {
					char c1 = URL[++i];
					char c0 = URL[++i];
					int num = 0;
					num += hexChar2dec(c1) * 16 + hexChar2dec(c0);
					result += char(num);
				}
			}
			int nRet = MultiByteToWideChar(CP_UTF8, 0, result.c_str(), -1, NULL, 0);
			WCHAR* pWszParam = new WCHAR[nRet + 1];
			if (pWszParam == NULL)
				return NULL;
			ZeroMemory(pWszParam, sizeof(WCHAR) * (nRet + 1));
			nRet = MultiByteToWideChar(CP_UTF8, 0, result.c_str(), -1, pWszParam, nRet);
			std::wstring wstrRet = pWszParam;
			delete []pWszParam;
			return wstrRet;
		}
	private:
		static char dec2hexChar(short int n) 
		{
			if ( 0 <= n && n <= 9 ) 
			{
				return char( short('0') + n );
			}
			else if ( 10 <= n && n <= 15 ) 
			{
				return char( short('A') + n - 10 );
			} 
			else 
			{
				return char(0);
			}
		}
		static short int hexChar2dec(char c) 
		{
			if ( '0'<=c && c<='9' ) 
			{
				return short(c-'0');
			}
			else if ( 'a'<=c && c<='f' ) 
			{
				return ( short(c-'a') + 10 );
			}
			else if ( 'A'<=c && c<='F' ) 
			{
				return ( short(c-'A') + 10 );
			} 
			else 
			{
				return -1;
			}
		}
	};
}