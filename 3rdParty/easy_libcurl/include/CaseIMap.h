#pragma once
#include <map>
#include <string>

struct StrILesser 
{
	bool operator()(const std::string& l, const std::string& r) const
	{
		return _stricmp(l.c_str(), r.c_str()) < 0;
	}

	bool operator()(const std::wstring& l, const std::wstring& r) const
	{
		return _wcsicmp(l.c_str(), r.c_str()) < 0;
	}
};

/*!
 * \class caseimap
 * \brief 大小写无关的map
 *
 * key: 字符串    value: 自定义
 */
template<typename V>
class caseimap: public std::map<std::string, V, StrILesser>
{
};

