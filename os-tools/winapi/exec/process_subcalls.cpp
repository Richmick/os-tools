module os.winapi;

#ifdef _WIN32
import std;

// Char (ascii)
std::string winapi::prepare_cmd_args(const char* app, std::span< const char*const > args)
{
	std::string cmd = "\"";
	cmd += app;
	cmd += '"';
	if (args.size() != 0)
	{
		cmd += ' ';
		cmd += join_args(args);
	}
	return cmd;
}
std::string winapi::prepare_cmd_args(const char* app, std::span< const std::string > args)
{
	std::string_view app_name = app;
	if (app_name.contains('"')) throw std::invalid_argument{"appname cannot have '\"'"};
	std::string cmd = "\"";
	cmd += app_name;
	cmd += '"';
	if (args.size() != 0)
	{
		cmd += ' ';
		cmd += join_args(args);
	}
	return cmd;
}
std::string winapi::join_args(std::span< const std::string > data)
{
	std::string result;
	for (const std::string& arg: data)
	{
		if (!result.empty()) result += ' ';
		result += screen(arg);
	}
	return result;
}
std::string winapi::join_args(std::span< const char* const > data)
{
	std::string result;
	for (const char* arg: data)
	{
		if (!result.empty()) result += ' ';
		result += screen(arg);
	}
	return result;
}
std::string winapi::screen(const std::string& arg)
{
	return screen(arg.c_str());
}
std::string winapi::screen(const char* arg)
{
	if (arg == nullptr) return {};
	std::string result = "\"";
	for (; *arg != '\0'; arg++)
	{
		if (*arg == '"') result += "\\\"";
		else if (*arg == '\\') result += "\\\\";
		else result += *arg;
	}
	return result += '"';
}
winapi::unique_handle winapi::process::create(const char* app, char* console, std::error_code& err)
{
	startup_info_t< char > startup{.cb = sizeof(startup_info_t)};
	process_info_t result;
	if (!create_process_aw(app, startup, result, console))
	{
		err = winapi::last_error_code();
		return {};
	}
	err.clear();
	unique_handle thread_handle{result.hThread};
	return unique_handle{result.hProcess};
}
winapi::unique_handle winapi::process::create(initial_params& params, const char* app, char* console,
			std::error_code& err)
{
	startup_info_t< char > startup{.cb = sizeof(startup_info_t)};
	process_info_t result;
	if (!create_process_aw(app, startup, result, console))
	{
		err = winapi::last_error_code();
		return {};
	}
	err.clear();
	unique_handle thread_handle{result.hThread};
	return unique_handle{result.hProcess};
}

// Wchar_t (windows)
std::wstring winapi::prepare_cmd_args(const wchar_t* app, std::span< const wchar_t*const > args)
{
	std::wstring cmd = L"\"";
	cmd += app;
	cmd += L'"';
	if (args.size() != 0)
	{
		cmd += L' ';
		cmd += join_args(args);
	}
	return cmd;
}
std::wstring winapi::prepare_cmd_args(const wchar_t* app, std::span< const std::wstring > args)
{
	std::wstring_view app_name = app;
	if (app_name.contains('"')) throw std::invalid_argument{"appname cannot have '\"'"};
	std::wstring cmd = L"\"";
	cmd += app_name;
	cmd += L'"';
	if (args.size() != 0)
	{
		cmd += L' ';
		cmd += join_args(args);
	}
	return cmd;
}
std::wstring winapi::join_args(std::span< const std::wstring > data)
{
	std::wstring result;
	for (const std::wstring& arg: data)
	{
		if (!result.empty()) result += L' ';
		result += screen(arg);
	}
	return result;
}
std::wstring winapi::join_args(std::span< const wchar_t* const > data)
{
	std::wstring result;
	for (const wchar_t* arg: data)
	{
		if (!result.empty()) result += L' ';
		result += screen(arg);
	}
	return result;
}
std::wstring winapi::screen(const std::wstring& arg)
{
	return screen(arg.c_str());
}
std::wstring winapi::screen(const wchar_t* arg)
{
	if (arg == nullptr) return {};
	std::wstring result = L"\"";
	for (; *arg != L'\0'; arg++)
	{
		if (*arg == L'"') result += L"\\\"";
		else if (*arg == L'\\') result += L"\\\\";
		else result += *arg;
	}
	return result += L'"';
}
winapi::unique_handle winapi::process::create(const wchar_t* app, wchar_t* console, std::error_code& err)
{
	startup_info_t< wchar_t > startup{.cb = sizeof(startup_info_t< wchar_t >)};
	process_info_t result;
	if (!create_process_aw(app, startup, result, console))
	{
		err = winapi::last_error_code();
		return {};
	}
	err.clear();
	unique_handle thread_handle{result.hThread};
	return unique_handle{result.hProcess};
}
winapi::unique_handle winapi::process::create(initial_params& params, const wchar_t* app, wchar_t* console,
			std::error_code& err)
{
	if (params.stdin != nullptr) params.startup_info.hStdInput = params.stdin;
	if (params.stdout != nullptr) params.startup_info.hStdOutput = params.stdout;
	if (params.stderr != nullptr) params.startup_info.hStdError = params.stderr;
	if ((params.stdin != nullptr) || (params.stdout != nullptr) || (params.stderr != nullptr))
		params.startup_info.dwFlags |= use_provided_handles;
	if (params.startup_info.cb == 0) params.startup_info.cb = sizeof(startup_info_t< wchar_t >);
	process_info_t result;
	if (params.priority != process::priority::unset) params.flags |= static_cast< dword_t >(params.priority);
	if (params.create_group) params.flags |= create_new_group;
	if (!create_process_aw(app, params.startup_info, result, console, nullptr, nullptr,
					static_cast< bool >(params.inherit_handles), params.flags))
	{
		err = winapi::last_error_code();
		return {};
	}
	err.clear();
	unique_handle thread_handle{result.hThread};
	return unique_handle{result.hProcess};
}
#endif
