module;
	#include <cassert>
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
winapi::unique_handle winapi::process::create(const char* app, char* console)
{
	startup_info_t startup{.cb = sizeof(startup_info_t)};
	process_info_t result;
	if (!create_process_aw(app, startup, result, console))
	{
		throw winapi::last_error("failed to start process (called self)");
	}
	unique_handle thread_handle{result.hThread};
	return unique_handle{result.hProcess};
}

[[noreturn]]
void winapi::this_process::exit(int code)
{
	winapi::calls::ExitProcess(code);
}
winapi::process::native_handle_type winapi::this_process::native_handle()
{
	return winapi::calls::GetCurrentProcess();
}
winapi::process::id winapi::this_process::get_id()
{
	return winapi::calls::GetCurrentProcessId();
}
winapi::process::priority winapi::this_process::get_priority()
{
	return static_cast< process::priority >(winapi::calls::GetPriorityClass(native_handle()));
}
void winapi::this_process::set_priority(process::priority p)
{
	assert(p != process::priority::unset);
	if (!winapi::calls::SetPriorityClass(native_handle(), static_cast< dword_t >(p)))
	{
		throw winapi::last_error("failed to set process priority");
	}
}
std::string winapi::this_process::executable()
{
	return executable_aw< char >();
}
std::optional< std::string > winapi::this_process::get_env(const std::string& name)
{
	return get_env_aw(name);
}
void winapi::this_process::set_env(const std::string& name, const std::string& value)
{
	set_env_aw(name, value);
}
void winapi::this_process::delete_env(const std::string& name)
{
	delete_env_aw(name);
}
std::unordered_map< std::string, std::string> winapi::this_process::environment()
{
	return environment_aw< char >();
}

winapi::process::process(unique_handle&& h) noexcept:
	native_(std::move(h))
{}
winapi::process::process(native_handle_type h) noexcept:
	native_(h)
{}
winapi::process::process(process&& rhs) noexcept:
	native_(std::move(rhs.native_))
{}
winapi::process::process(call_self_t)
{
	std::string executable = winapi::this_process::executable();
	native_ = create(executable.data(), nullptr);
}
winapi::process::process(call_self_t, std::span< const char*const > args):
	process(prepare_cmd_args(this_process::executable(), args))
{}
winapi::process::process(call_self_t, std::span< const std::string > args):
	process(prepare_cmd_args(this_process::executable(), args))
{}
winapi::process::process(call_self_t, std::string args)
{
	std::string executable = winapi::this_process::executable();
	native_ = create(executable.data(), args.data());
}
winapi::process::process(const std::string& app, std::span< const std::string > args):
	process(prepare_cmd_args(app, args))
{}
winapi::process::process(const char* app, std::span< const std::string > args):
	process(prepare_cmd_args(app, args))
{}
winapi::process::process(const char* app, std::span< const char*const > args):
	process(prepare_cmd_args(app, args))
{}
winapi::process::process(const std::string& app, std::string args)
{
	native_ = create(app.data(), args.data());
}
winapi::process::process(std::span< const char*const > cl_cmd):
	process(join_args(cl_cmd))
{}
winapi::process::process(std::string cl_cmd)
{
	native_ = create(nullptr, cl_cmd.data());
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
winapi::process::native_handle_type winapi::process::native_handle() const noexcept
{
	return native_.get();
}
winapi::handle_t winapi::process::release_handle() noexcept
{
	return native_.release();
}
void winapi::process::swap(process& rhs) noexcept
{
	native_.swap(rhs.native_);
}
winapi::process::id winapi::process::get_id() const
{
	dword_t result = winapi::calls::GetProcessId(native_.get());
	if (result == 0) throw winapi::last_error("failed to get process id");
	return result;
}
bool winapi::process::joinable() const noexcept
{
	return native_;
}
bool winapi::process::alive() const
{
	return !wait(std::chrono::milliseconds{0});
}
int winapi::process::wait() const
{
	if (get_id() == this_process::get_id())
		throw std::system_error{std::make_error_code(std::errc::resource_deadlock_would_occur),
					"join process to self"};
	if (!joinable()) throw std::invalid_argument{"wait for not joinable process"};
	winapi::wait(native_.get());
	return exit_code();
}
bool winapi::process::wait(std::chrono::milliseconds dur) const
{
	if (get_id() == this_process::get_id())
		throw std::system_error{std::make_error_code(std::errc::resource_deadlock_would_occur),
					"join process to self"};
	if (!joinable()) throw std::invalid_argument{"wait for not joinable process"};
	return winapi::wait(native_.get(), dur);
}
int winapi::process::join()
{
	int code = wait();
	native_.reset();
	return code;
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
winapi::process::priority winapi::process::get_priority() const
{
	return static_cast< priority >(winapi::calls::GetPriorityClass(native_.get()));
}
void winapi::process::set_priority(priority p) const
{
	assert(p != priority::unset);
	if (!winapi::calls::SetPriorityClass(native_.get(), static_cast< dword_t >(p)))
	{
		throw winapi::last_error("failed to set process priority");
	}
}
int winapi::process::exit_code() const
{
	dword_t result;
	if (!winapi::calls::GetExitCodeProcess(native_.get(), &result))
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
	if (!winapi::calls::TerminateProcess(native_.get(), exit_code))
	{
		throw winapi::last_error("failed to terminate process");
	}
}

void std::swap(winapi::process& lhs, winapi::process& rhs) noexcept
{
	lhs.swap(rhs);
}

#endif
