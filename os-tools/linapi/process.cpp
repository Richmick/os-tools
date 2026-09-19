module;
	#ifdef __linux__
		#include <sys/wait.h>
	#endif
module os.linapi;

#ifdef __linux__
import std;

linapi::ready_args_t linapi::prepare_cmd_args(const char* app, std::span< const char*const > args)
{
	ready_args_t result;
	result.first.reserve(args.size() + 1);
	result.second.reserve(args.size() + 1);
	result.second.emplace_back(result.first.emplace_back(app).data());
	for (const char* arg: args)
	{
		result.second.emplace_back(result.first.emplace_back(arg).data());
	}
	return result;
}
linapi::ready_args_t linapi::prepare_cmd_args(const std::string& app, std::span< const char*const > args)
{
	ready_args_t result;
	result.first.reserve(args.size() + 1);
	result.second.reserve(args.size() + 1);
	result.second.emplace_back(result.first.emplace_back(app.c_str()).data());
	for (const char* arg: args)
	{
		result.second.emplace_back(result.first.emplace_back(arg).data());
	}
	return result;
}
linapi::ready_args_t linapi::prepare_cmd_args(const std::string& app, std::span< const std::string > args)
{
	ready_args_t result;
	result.first.reserve(args.size() + 1);
	result.second.reserve(args.size() + 1);
	result.second.emplace_back(result.first.emplace_back(app.c_str()).data());
	for (const std::string& arg: args)
	{
		result.second.emplace_back(result.first.emplace_back(arg.c_str()).data());
	}
	return result;
}
linapi::process::native_handle_type linapi::process::create(const char* app, char*const* argv)
{
	process_handle_t h = linapi::calls::vfork();
	if (h == 0)
	{
		linapi::calls::execv(app, argv);
		linapi::calls::_exit(-1);
	}
	else if (h == -1)
	{
		throw linapi::last_error("failed to vfork process");
	}
	return h;
}

[[noreturn]]
void linapi::this_process::exit(int code)
{
	std::exit(code);
}
linapi::process::native_handle_type linapi::this_process::native()
{
	return linapi::calls::getpid();
}
linapi::process::native_handle_type linapi::this_process::native_handle()
{
	return linapi::calls::getpid();
}
linapi::process::id linapi::this_process::get_id()
{
	return this_process::native();
}
linapi::process::native_handle_type linapi::this_process::parent_native()
{
	return linapi::calls::getppid();
}
linapi::process::native_handle_type linapi::this_process::parent_native_handle()
{
	return linapi::calls::getppid();
}
linapi::process::id linapi::this_process::get_parent_id()
{
	return this_process::parent_native();
}
linapi::process::priority linapi::this_process::get_priority()
{
	int res = linapi::calls::sched_getscheduler(this_process::native());
	if (res == -1)
	{
		throw linapi::last_error("failed to get process priority");
	}
	return static_cast< process::priority >(res);
}
void linapi::this_process::set_priority(process::priority p)
{
	scheduler_settings_t param{};
	int res = linapi::calls::sched_setscheduler(this_process::native(), static_cast< int >(p), &param);
	if (res == -1)
	{
		throw linapi::last_error("failed to set process priority");
	}
}
std::string linapi::this_process::executable()
{
	const char* symlink = "/proc/self/exe";
	constexpr std::size_t length = 256;

	{
		char buf[length];
		std::ptrdiff_t res_len = linapi::calls::readlink(symlink, buf, length);
		if (res_len < 0) throw linapi::last_error("failed to retrieve executable");
		buf[res_len] = '\0';
		if (res_len < length) return {buf, static_cast< std::size_t >(res_len)};
	}
	std::string buf;
	buf.reserve(length * 2);
	std::size_t res_len = linapi::calls::readlink(symlink, buf.data(), buf.size());
	while (res_len == buf.size())
	{
		buf.clear();
		buf.reserve(res_len * 2);
		res_len = linapi::calls::readlink(symlink, buf.data(), buf.size());
	}
	if (res_len == 0) throw linapi::last_error("failed to retrieve executable");
	return {buf, static_cast< std::size_t >(res_len)};
}
std::optional< std::string > linapi::this_process::get_env(const std::string& name)
{
	const char* res = linapi::calls::getenv(name.c_str());
	if (res == nullptr) return std::nullopt;
	return std::optional< std::string >{std::in_place, res};
}
void linapi::this_process::set_env(const std::string& name, const std::string& value)
{
	if (linapi::calls::setenv(name.c_str(), value.c_str(), true) != 0)
	{
		throw linapi::last_error("failed to delete env var");
	}
}
void linapi::this_process::delete_env(const std::string& name)
{
	if (linapi::calls::unsetenv(name.c_str()) != 0)
	{
		throw linapi::last_error("failed to delete env var");
	}
}
std::unordered_map< std::string, std::string > linapi::this_process::environment()
{
	std::unordered_map< std::string, std::string > result;
	const char*const* caret = linapi::environ;
	if (caret == nullptr) return result;
	for (; *caret != nullptr; caret++)
	{
		std::string_view line{*caret};
		std::string_view::size_type eq_sign = line.find_first_of('=');
		if (eq_sign == std::string_view::npos)
		{
			throw std::runtime_error{"no equal sign in env variable"};
		}
		std::string_view key = line.substr(0, eq_sign), value = line.substr(eq_sign + 1);
		result.emplace(key, value);
	}
	return result;
}

linapi::process::process(native_handle_type h) noexcept:
	native_(h)
{}
linapi::process::process(process&& rhs) noexcept:
	native_(rhs.release())
{}
linapi::process::process(call_self_t)
{
	std::string app = this_process::executable();
	char*const argv[] = {app.data(), nullptr};
	native_ = create(app.data(), argv);
}
linapi::process::process(call_self_t, std::span< const char*const > args):
	process(prepare_cmd_args(this_process::executable(), args))
{}
linapi::process::process(call_self_t, std::span< const std::string > args):
	process(prepare_cmd_args(this_process::executable(), args))
{}
linapi::process::process(call_self_t, char*const* args)
{
	std::string app = this_process::executable();
	native_ = create(app.data(), args);
}
linapi::process::process(const std::string& app, std::span< const std::string > args):
	process(prepare_cmd_args(app, args))
{}
linapi::process::process(const char* app, std::span< const std::string > args):
	process(prepare_cmd_args(app, args))
{}
linapi::process::process(const char* app, std::span< const char*const > args):
	process(prepare_cmd_args(app, args))
{}
linapi::process::process(const char* app, char*const* args)
{
	native_ = create(app, args);
}
linapi::process::process(ready_args_t argv)
{
	std::string app = argv.first.front();
	native_ = create(app.data(), argv.second.data());
}

linapi::process& linapi::process::operator=(process&& rhs) noexcept
{
	if (joinable()) std::terminate();
	native_ = rhs.release();
	return *this;
}
linapi::process::~process()
{
	if (joinable()) std::terminate();
}
linapi::process::native_handle_type linapi::process::native() const noexcept
{
	return native_;
}
linapi::process::native_handle_type linapi::process::native_handle() const noexcept
{
	return native_;
}
linapi::process::native_handle_type linapi::process::release()
{
	return std::exchange(native_, {});
}
void linapi::process::swap(process& rhs) noexcept
{
	std::swap(native_, rhs.native_);
}
linapi::process::id linapi::process::get_id() const
{
	return native_;
}
bool linapi::process::joinable() const noexcept
{
	return native_ != id{};
}
bool linapi::process::alive() const
{
	if (get_id() == this_process::get_id())
		throw std::system_error{std::make_error_code(std::errc::resource_deadlock_would_occur),
					"join process to self"};
	if (!joinable()) throw std::invalid_argument{"wait for not joinable process"};

	siginfo_t result;
	if (linapi::calls::waitid(pid_entity, native_, &result, wait_for_exit | wait_only | wait_immediate) < 0)
	{
		throw linapi::last_error("failed to wait process");
	}
	return result.si_pid != native_;
}
int linapi::process::wait() const
{
	if (get_id() == this_process::get_id())
		throw std::system_error{std::make_error_code(std::errc::resource_deadlock_would_occur),
					"join process to self"};
	if (!joinable()) throw std::invalid_argument{"wait for not joinable process"};

	siginfo_t result;
	if (linapi::calls::waitid(pid_entity, native_, &result, wait_for_exit | wait_only) < 0)
	{
		throw linapi::last_error("failed to wait process");
	}
	return result.si_status;
}
int linapi::process::join()
{
	if (get_id() == this_process::get_id())
		throw std::system_error{std::make_error_code(std::errc::resource_deadlock_would_occur),
					"join process to self"};
	if (!joinable()) throw std::invalid_argument{"wait for not joinable process"};

	int result;
	if (linapi::calls::waitpid(native_, &result, 0) < 0)
	{
		throw linapi::last_error("failed to join process");
	}
	release();
	return WEXITSTATUS(result);
}
linapi::process::priority linapi::process::get_priority() const
{
	int res = linapi::calls::sched_getscheduler(native_);
	if (res == -1)
	{
		throw linapi::last_error("failed to get process priority");
	}
	return static_cast< priority >(res);
}
void linapi::process::set_priority(priority p) const
{
	scheduler_settings_t param{};
	int res = linapi::calls::sched_setscheduler(native_, static_cast< int >(p), &param);
	if (res == -1)
	{
		throw linapi::last_error("failed to set process priority");
	}
}
int linapi::process::exit_code() const
{
	siginfo_t result;
	if (linapi::calls::waitid(pid_entity, native_, &result, wait_for_exit | wait_only | wait_immediate) < 0)
	{
		throw linapi::last_error("failed to wait process");
	}
	if (result.si_pid != native_)
	{
		return still_active;
	}
	return static_cast< int >(result.si_status);
}
void linapi::process::kill()
{
	if (linapi::calls::kill(native_, static_cast< int >(linapi::signal::kill)) != 0)
	{
		throw linapi::last_error("failed to kill process");
	}
}

void std::swap(linapi::process& lhs, linapi::process& rhs) noexcept
{
	lhs.swap(rhs);
}

#endif
