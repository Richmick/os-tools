module os.winapi;

#ifdef _WIN32
import std;

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
std::string winapi::prepare_cmd_args(const std::string& app, std::span< const char*const > args)
{
	std::string cmd = '"' + app + '"';
	if (args.size() != 0)
	{
		cmd += ' ';
		cmd += join_args(args);
	}
	return cmd;
}
std::string winapi::prepare_cmd_args(const std::string& app, std::span< const std::string > args)
{
	std::string cmd = '"' + app + '"';
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

[[noreturn]]
void winapi::this_process::exit(int code)
{
	winapi::ExitProcess(code);
}
winapi::handle_t winapi::this_process::native()
{
	return winapi::GetCurrentProcess();
}
int winapi::this_process::id()
{
	return winapi::GetCurrentProcessId();
}
winapi::priority winapi::this_process::priority()
{
	return static_cast< winapi::priority >(winapi::GetPriorityClass(native()));
}
void winapi::this_process::set_priority(winapi::priority p)
{
	if (!winapi::SetPriorityClass(native(), static_cast< dword_t >(p)))
	{
		throw winapi::last_error("failed to set process priority");
	}
}
winapi::tstring winapi::this_process::executable()
{
	constexpr std::size_t length = 256;
	{
		tchar_t buf[length];
		std::size_t res_len = winapi::get_module_file_name(nullptr, buf, length);
		if (res_len == 0) throw winapi::last_error("failed to retrieve executable");
		if (res_len < length) return {buf, res_len};
	}
	std::string buf;
	buf.reserve(length * 2);
	std::size_t res_len = winapi::get_module_file_name(nullptr, buf.data(), buf.size());
	while (res_len == buf.size())
	{
		buf.clear();
		buf.reserve(res_len * 2);
		res_len = winapi::get_module_file_name(nullptr, buf.data(), buf.size());
	}
	if (res_len == 0) throw winapi::last_error("failed to retrieve executable");
	return {buf, res_len};
}

winapi::process::process(unique_handle&& h) noexcept:
	native_(std::move(h))
{}
winapi::process::process(handle_t h) noexcept:
	native_(h)
{}
winapi::process::process(process&& rhs) noexcept:
	native_(std::move(rhs.native_))
{}
winapi::process::process(call_self_t)
{
	std::string executable = winapi::this_process::executable();
	startup_info_t startup{.cb = sizeof(startup_info_t)};
	process_info_t result;
	if (!create_process(executable.data(), startup, result))
	{
		throw winapi::last_error("failed to start process (called self)");
	}
	unique_handle thread_handle{result.hThread};
	native_.reset(result.hProcess);
}
winapi::process::process(call_self_t, std::span< const char*const > args)
{
	std::string cmd = prepare_cmd_args(this_process::executable(), args);
	startup_info_t startup{.cb = sizeof(startup_info_t)};
	process_info_t result;
	if (!create_process< char >(nullptr, startup, result, cmd.data()))
	{
		throw winapi::last_error("failed to start process (called self)");
	}
	unique_handle thread_handle{result.hThread};
	native_.reset(result.hProcess);
}
winapi::process::process(call_self_t, std::span< const std::string > args)
{
	std::string cmd = prepare_cmd_args(this_process::executable(), args);
	startup_info_t startup{.cb = sizeof(startup_info_t)};
	process_info_t result;
	if (!create_process< char >(nullptr, startup, result, cmd.data()))
	{
		throw winapi::last_error("failed to start process (called self)");
	}
	unique_handle thread_handle{result.hThread};
	native_.reset(result.hProcess);
}
winapi::process& winapi::process::operator=(process&& rhs) noexcept
{
	if (joinable()) std::terminate();
	native_ = std::move(rhs.native_);
	return *this;
}
winapi::process::~process()
{
	if (joinable()) std::terminate();
}
winapi::handle_t winapi::process::native() const noexcept
{
	return native_.get();
}
winapi::unique_handle winapi::process::release()
{
	return std::move(native_);
}
void winapi::process::swap(process& rhs) noexcept
{
	native_.swap(rhs.native_);
}
int winapi::process::id()
{
	dword_t result = winapi::GetProcessId(native_.get());
	if (result == 0) throw winapi::last_error("failed to get process id");
	return static_cast< int >(result);
}
bool winapi::process::joinable() const noexcept
{
	return native_;
}
void winapi::process::wait()
{
	if (id() == this_process::id())
		throw std::system_error{std::make_error_code(std::errc::resource_deadlock_would_occur),
					"join process to self"};
	if (!joinable()) throw std::invalid_argument{"wait for not joinable process"};
	winapi::wait(native_.get());
}
bool winapi::process::wait(std::chrono::milliseconds dur)
{
	if (id() == this_process::id())
		throw std::system_error{std::make_error_code(std::errc::resource_deadlock_would_occur),
					"join process to self"};
	if (!joinable()) throw std::invalid_argument{"wait for not joinable process"};
	return winapi::wait(native_.get(), dur);
}
void winapi::process::join()
{
	wait();
	native_.reset();
}
bool winapi::process::join(std::chrono::milliseconds dur)
{
	if (wait(dur))
	{
		native_.reset();
		return true;
	}
	return false;
}
void winapi::process::detach()
{
	native_.reset();
}
winapi::priority winapi::process::priority()
{
	return static_cast< winapi::priority >(winapi::GetPriorityClass(native_.get()));
}
void winapi::process::set_priority(winapi::priority p)
{
	if (!winapi::SetPriorityClass(native_.get(), static_cast<dword_t>(p)))
	{
		throw winapi::last_error("failed to set process priority");
	}
}
int winapi::process::exit_code()
{
	dword_t result;
	if (!winapi::GetExitCodeProcess(native_.get(), &result))
	{
		throw winapi::last_error("failed to retrieve process exit code");
	}
	return static_cast< int >(result);
}
void winapi::process::kill()
{
	kill(-1);
}
void winapi::process::kill(int exit_code)
{
	if (!winapi::TerminateProcess(native_.get(), exit_code))
	{
		throw winapi::last_error("failed to terminate process");
	}
}

void std::swap(winapi::process& lhs, winapi::process& rhs) noexcept
{
	lhs.swap(rhs);
}

#endif
