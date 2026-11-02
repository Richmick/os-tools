module;
	#include <cassert>
module os.winapi;

#ifdef _WIN32
import std;

[[noreturn]]
void winapi::this_process::_exit(int code)
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
winapi::process::group_id winapi::this_process::get_group_id()
{
	return get_id();
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
void winapi::this_process::signal(ipc::signal::type type)
{
	ipc::signal::send(get_group_id(), type);
}
winapi::path winapi::this_process::executable()
{
	return executable_aw< path::value_type >();
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
winapi::handle_t winapi::this_process::stdin()
{
	return std_handle(io::in);
}
winapi::handle_t winapi::this_process::stdout()
{
	return std_handle(io::out);
}
winapi::handle_t winapi::this_process::stderr()
{
	return std_handle(io::err);
}
winapi::handle_t winapi::this_process::stdin(std::error_code& err) noexcept
{
	return std_handle(io::in, err);
}
winapi::handle_t winapi::this_process::stdout(std::error_code& err) noexcept
{
	return std_handle(io::out, err);
}
winapi::handle_t winapi::this_process::stderr(std::error_code& err) noexcept
{
	return std_handle(io::err, err);
}
winapi::handle_t winapi::this_process::std_handle(io type)
{
	std::error_code err;
	handle_t res = std_handle(type, err);
	throw_on_error(err, "failed to get std handle");
	return res;
}
winapi::handle_t winapi::this_process::std_handle(io type, std::error_code& err) noexcept
{
	handle_t res = winapi::calls::GetStdHandle(static_cast< dword_t >(type));
	if (winapi::is_blank(res)) err = winapi::last_error_code();
	else err.clear();
	return res;
}

winapi::process::~process()
{
	if (joinable()) std::terminate();
}
winapi::process& winapi::process::operator=(process&& rhs) noexcept
{
	if (joinable()) std::terminate();
	native_ = std::move(rhs.native_);
	return *this;
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
winapi::process::group_id winapi::process::get_group_id() const
{
	return get_id();
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
void winapi::process::signal(ipc::signal::type type)
{
	ipc::signal::send(get_group_id(), type);
}
void winapi::process::terminate()
{
	signal(ipc::signal::terminate);
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
