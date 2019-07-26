#pragma once
#include <string>
#include <vector>

namespace Utils
{
	std::wstring ansi_to_unicode(const char *const input, int cp = CP_ACP, int cbMultiByte = -1);
	std::string unicode_to_ansi(const wchar_t *const input, int cp = CP_ACP, int cchWideChar = -1);
	void open_url(const wchar_t* url);
	bool is_vista_or_later();
	bool read_file(const wchar_t* file_path, std::string& out);
	std::vector<std::wstring>& SplitWide(wchar_t* source, const wchar_t* token, std::vector<std::wstring>& dsts);
	std::vector<std::string>& SplitASCII(char* source, const char* token, std::vector<std::string>& dsts);
	bool get_exe_version(const wchar_t* exe_path, std::wstring& file_ver, std::wstring& product_ver);
	bool terminate_process_by_name(const std::wstring& proc_name, const std::wstring& path);
	bool find_process_by_name(const std::wstring& proc_name, const std::wstring& path);

	struct ScheduleData
	{
		std::wstring taskName; // -t
		std::wstring comment; // -o
		std::wstring workdir; // -w
		std::wstring appName; // -p
		std::wstring runParam; // -a
		std::wstring userName; // -r
		unsigned int minutesInterval; // -m
	};
	bool CreateSchedule(const ScheduleData* data);
	bool DetectSchedule(const std::wstring* taskName);
	bool UpdateSchedule(const ScheduleData* data);
	bool DeleteSchedule(const std::wstring* taskName);
}