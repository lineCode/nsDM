/* -------------------------------------------------------------------------
//	FileName	£º	command_line.cpp
//	Creator		£º	yanbozhao
//	CreateTime	£º	2015.05.29 11:06:00
//	Description	£º
//
// -----------------------------------------------------------------------*/

#include "command_line.h"
#include "string/string_util.h"
#include <windows.h>
#include <assert.h>
// -------------------------------------------------------------------------

namespace base 
{
	const wchar_t kSwitchTerminator[]	   = L"--";
	const wchar_t kSwitchValueSeparator[]  = L"=";
	const wchar_t* const kSwitchPrefixes[] = { L"--", L"-", L"/" };

	size_t switch_prefix_count = sizeof(kSwitchPrefixes) / sizeof(kSwitchPrefixes[0]);

	size_t GetSwitchPrefixLength(const std::wstring& string) {
		for (size_t i = 0; i < switch_prefix_count; ++i) {
			std::wstring prefix(kSwitchPrefixes[i]);
			if (string.compare(0, prefix.length(), prefix) == 0) {
				return prefix.length();
			}
		}
		return 0;
	}

	bool IsSwitch(const std::wstring& string, std::wstring* switch_string, std::wstring* switch_value) {
		switch_string->clear();
		switch_value->clear();
		size_t prefix_length = GetSwitchPrefixLength(string);
		if (prefix_length == 0 || prefix_length == string.length()) {
			return false;
		}

		const size_t equals_position = string.find(kSwitchValueSeparator);
		*switch_string = string.substr(0, equals_position);
		if (equals_position != std::wstring::npos) {
			*switch_value = string.substr(equals_position + 1);
		}
		return true;
	}

	void AppendSwitchesAndArguments(CommandLine& command_line, const std::vector<std::wstring>& argv) {
		bool parse_switches = true;
		for (size_t i = 1; i < argv.size(); ++i) {
			std::wstring arg = argv[i];
			string_util::TrimWhitespace(arg, string_util::eTrim_All, &arg);

			std::wstring switch_string;
			std::wstring switch_value;
			parse_switches &= (arg != kSwitchTerminator);
			if (parse_switches && IsSwitch(arg, &switch_string, &switch_value)) {
				command_line.AppendSwitchNative(string_util::ToStringASCII(switch_string), switch_value);
			}
			else {
				command_line.AppendArgNative(arg);
			}
		}
	}

	CommandLine* CommandLine::current_process_commandline_ = NULL;

	CommandLine::CommandLine() 
		: argv_(1)
		, begin_args_(1) {
	}

	CommandLine::CommandLine(int argc, const wchar_t* const* argv) 
		: argv_(1)
		, begin_args_(1) {
		InitFromArgv(argc, argv);
	}

	CommandLine::CommandLine(const std::vector<std::wstring>& argv)
		: argv_(1)
		, begin_args_(1) {
		InitFromArgv(argv);
	}

	CommandLine::~CommandLine() {
	}

	bool CommandLine::Init(int argc, const char* const* argv) {
		if (current_process_commandline_) {
			return false;
		}
		current_process_commandline_ = new CommandLine();
		current_process_commandline_->ParseFromString(::GetCommandLineW());
		return true;
	}

	void CommandLine::Reset() {
		if (NULL == current_process_commandline_) {
			return;
		}
		delete current_process_commandline_;
		current_process_commandline_ = NULL;
	}

	CommandLine* CommandLine::ForCurrentProcess() {
		assert(current_process_commandline_ != NULL);
		return current_process_commandline_;
	}

	CommandLine CommandLine::FromString(const std::wstring& cmdLine) {
		CommandLine cmd;
		cmd.ParseFromString(cmdLine);
		return cmd;
	}

	void CommandLine::InitFromArgv(int argc, const wchar_t* const* argv) {
		std::vector<std::wstring> new_argv;
		for (int i = 0; i < argc; ++i) {
			new_argv.push_back(argv[i]);
		}
		InitFromArgv(new_argv);
	}

	void CommandLine::InitFromArgv(const std::vector<std::wstring>& argv) {
		argv_ = std::vector<std::wstring>(1);
		switch_map_.clear();
		begin_args_ = 1;
		AppendSwitchesAndArguments(*this, argv);
	}

	void CommandLine::ParseFromString(const std::wstring& cmdLine) {
		std::wstring command_line_string;
		string_util::TrimWhitespace(cmdLine, string_util::eTrim_All, &command_line_string);
		if (command_line_string.empty()) {
			return;
		}
		int num_args = 0;
		wchar_t** args = NULL;
		args = ::CommandLineToArgvW(command_line_string.c_str(), &num_args);
		InitFromArgv(num_args, args);
		LocalFree(args);
	}

	bool CommandLine::HasSwitch(const std::string& switch_string) const {
		return switch_map_.find(string_util::StringToLowerASCII(switch_string)) != switch_map_.end();
	}

	std::string CommandLine::GetSwitchValueASCII(const std::string& switch_string) const {
		std::wstring value = GetSwitchValueNative(switch_string);
		return string_util::ToStringASCII(value);
	}

	std::wstring CommandLine::GetSwitchValueNative(const std::string& switch_string) const {
		std::map<std::string, std::wstring>::const_iterator itFind = switch_map_.find(string_util::StringToLowerASCII(switch_string));
		return itFind == switch_map_.end() ? L"" : itFind->second;
	}

	void CommandLine::AppendSwitchNative(const std::string& switch_string, const std::wstring& value) {
		std::string switch_key(string_util::StringToLowerASCII(switch_string));
		std::wstring combined_switch_string(string_util::ToStringWide(switch_key));

		size_t prefix_length = GetSwitchPrefixLength(combined_switch_string);
		switch_map_[switch_key.substr(prefix_length)] = value;
	
		if (prefix_length == 0) {
			combined_switch_string = kSwitchPrefixes[0] + combined_switch_string;
		}
		if (!value.empty()) {
			combined_switch_string += kSwitchValueSeparator + value;
		}
		
		argv_.insert(argv_.begin() + begin_args_++, combined_switch_string);
	}

	void CommandLine::AppendArgNative(const std::wstring& value) {
		argv_.push_back(value);
	}

}




// -------------------------------------------------------------------------
