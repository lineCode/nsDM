#pragma once

#include <string>

namespace easy_libcurl
{
	namespace EasyHelper
	{
		std::string extract_name_from_url(const char* url);
		std::wstring get_default_file_name();
	
		std::wstring mbstr_to_wide(const char* c, size_t len, unsigned int codePage = CP_UTF8);
		std::string wide_to_mbstr(const wchar_t* c, int len, UINT codePage = CP_UTF8);
	
		template<typename charT>
		struct str_i_equal {
			str_i_equal( const std::locale& loc ) : loc_(loc) {}
			bool operator()(charT ch1, charT ch2) {
				return std::toupper(ch1, loc_) == std::toupper(ch2, loc_);
			}
		private:
			const std::locale& loc_;
		};
	
		// find substring (case insensitive)
		template<typename T>
		int ci_find_substr( const T& str1, const T& str2, const std::locale& loc = std::locale() )
		{
			typename T::const_iterator it = std::search( str1.begin(), str1.end(), 
				str2.begin(), str2.end(), str_i_equal<typename T::value_type>(loc) );
			if ( it != str1.end() ) return it - str1.begin();
			else return -1; // not found
		}

		std::wstring get_user_temp_path();
		std::wstring get_temp_file(const wchar_t* prefixStr);
	}
}
