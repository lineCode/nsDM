/* -------------------------------------------------------------------------
//	FileName	：	command_line.h
//	Creator		：	yanbozhao	
//	CreateTime	：	2015.05.29 11:06:00
//	Description	：	
//
// -----------------------------------------------------------------------*/
#ifndef __BASE_COMMAND_LINE_H__
#define __BASE_COMMAND_LINE_H__

#include <string>
#include <vector>
#include <map>
// -------------------------------------------------------------------------

namespace base {
	
	class CommandLine
	{
	public:
		/*===================================================//
			   构造函数声明为explicit，不允许做隐式转换
		//===================================================*/
		explicit CommandLine();
		explicit CommandLine(int argc, const wchar_t* const* argv);
		explicit CommandLine(const std::vector<std::wstring>& argv);
		//
		~CommandLine();

	public:
		static bool Init(int argc, const char* const* argv);
		static void Reset();
		static CommandLine* ForCurrentProcess();
		static CommandLine FromString(const std::wstring& cmdLine);

	public:
		void InitFromArgv(int argc, const wchar_t* const* argv);
		void InitFromArgv(const std::vector<std::wstring>& argv);

		void ParseFromString(const std::wstring& cmdLine);

		bool HasSwitch(const std::string& switch_string) const;

		std::string GetSwitchValueASCII(const std::string& switch_string) const;
		std::wstring GetSwitchValueNative(const std::string& switch_string) const;
	
		void AppendSwitchNative(const std::string& switch_string, const std::wstring& value);
		void AppendArgNative(const std::wstring& value);

	private:
		static CommandLine* current_process_commandline_;

		std::vector<std::wstring>			argv_;
		std::map<std::string, std::wstring> switch_map_;
		size_t								begin_args_;
	};
}

// -------------------------------------------------------------------------

#endif /* __BASE_COMMAND_LINE_H__ */