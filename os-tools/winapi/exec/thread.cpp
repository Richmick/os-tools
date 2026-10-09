module;
	#include <cassert>
module os.winapi;

#ifdef _WIN32
import std;

[[noreturn]]
void winapi::this_thread::exit(int code)
{
	winapi::calls::ExitThread(code);
}
winapi::thread::native_handle_type winapi::this_thread::native_handle()
{
	return winapi::calls::GetCurrentThread();
}
winapi::thread::id winapi::this_thread::get_id()
{
	return winapi::calls::GetCurrentThreadId();
}
winapi::process::id winapi::this_thread::get_process_id()
{
	return winapi::this_process::get_id();
}
winapi::thread::priority winapi::this_thread::get_priority()
{
	int res = winapi::calls::GetThreadPriority(native_handle());
	if (res == static_cast< int >(winapi::thread::priority::unset))
	{
		throw winapi::last_error("failed to set thread priority");
	}
	return static_cast< thread::priority >(res);
}
void winapi::this_thread::set_priority(thread::priority p)
{
	assert(p != thread::priority::unset);
	if (!winapi::calls::SetThreadPriority(native_handle(), static_cast< int >(p)))
	{
		throw winapi::last_error("failed to set thread priority");
	}
}

winapi::thread::thread(native_handle_type h) noexcept:
	native_(h)
{}
winapi::thread::thread(unique_handle&& h) noexcept:
	native_(std::move(h))
{}
winapi::thread::thread(thread&& rhs) noexcept:
	native_(std::move(rhs.native_)),
	erasor_(std::exchange(rhs.erasor_, nullptr))
{}
winapi::thread::thread(thread_function_t f, void* data)
{
	native_ = unique_handle{winapi::calls::CreateThread(nullptr, 0, f, data, 0, nullptr)};
	if (!native_)
	{
		throw winapi::last_error("failed to start thread");
	}
}
winapi::thread::thread(const initial_params& params, thread_function_t f, void* data)
{
	security_attributes_t attributes = get_attributes(params);
	native_ = unique_handle{winapi::calls::CreateThread(&attributes, params.stack_size,
					f, data, get_flags(params), nullptr)};
	if (!native_)
	{
		throw winapi::last_error("failed to start thread");
	}
	if (params.priority != priority::unset)
	{
		set_priority(params.priority);
	}
}
winapi::thread::thread(const process& proc, thread_function_t f, void* data)
{
	native_ = unique_handle{winapi::calls::CreateRemoteThread(proc.native_handle(), nullptr, 0, f, data, 0, nullptr)};
	if (!native_)
	{
		throw winapi::last_error("failed to start thread");
	}
}
winapi::thread::thread(const process& proc, const initial_params& params, thread_function_t f, void* data)
{
	security_attributes_t attributes = get_attributes(params);
	native_ = unique_handle{winapi::calls::CreateRemoteThread(proc.native_handle(), &attributes, params.stack_size,
					f, data, get_flags(params), nullptr)};
	if (!native_)
	{
		throw winapi::last_error("failed to start thread");
	}
	if (params.priority != priority::unset)
	{
		set_priority(params.priority);
	}
}
winapi::thread::~thread()
{
	if (joinable()) std::terminate();
}
winapi::thread& winapi::thread::operator=(thread&& rhs) noexcept
{
	thread dummy{std::move(*this)};
	erasor_ = std::exchange(rhs.erasor_, nullptr);
	native_ = std::move(rhs.native_);
	return *this;
}

unsigned int winapi::thread::hardware_concurrency() noexcept
{
	return std::thread::hardware_concurrency();
}
winapi::security_attributes_t winapi::thread::get_attributes(const initial_params& params)
{
	security_attributes_t attributes = (params.attributes != nullptr ? *params.attributes : security_attributes_t{});
	if (params.inherit_handles) attributes.bInheritHandle = true;
	if (attributes.nLength == 0) attributes.nLength = sizeof(security_attributes_t);
	return attributes;
}
winapi::dword_t winapi::thread::get_flags(const initial_params& params)
{
	dword_t flags = 0;
	if (params.create_suspended) flags |= create_suspended;
	if (params.defer_stack_commit) flags |= stack_size_param_is_a_reservation;
	return flags;
}

winapi::handle_t winapi::thread::native_handle() const noexcept
{
	return native_.get();
}
winapi::unique_handle winapi::thread::release_handle() noexcept
{
	if ((erasor_ != nullptr) && erasor_->oneside_released.test_and_set())
	{
		delete erasor_;
	}
	erasor_ = nullptr;
	return std::move(native_);
}
void winapi::thread::swap(thread& rhs) noexcept
{
	std::swap(native_, rhs.native_);
	std::swap(erasor_, rhs.erasor_);
}
winapi::thread::id winapi::thread::get_id() const
{
	dword_t result = winapi::calls::GetThreadId(native_.get());
	if (result == 0) throw winapi::last_error("failed to get thread id");
	return result;
}
winapi::process::id winapi::thread::get_process_id() const
{
	dword_t result = winapi::calls::GetProcessIdOfThread(native_.get());
	if (result == 0) throw winapi::last_error("failed to get process id of thread");
	return result;
}
bool winapi::thread::joinable() const noexcept
{
	return native_;
}
int winapi::thread::wait() const
{
	if (get_id() == this_thread::get_id())
		throw std::system_error{std::make_error_code(std::errc::resource_deadlock_would_occur),
					"join thread to self"};
	if (!joinable())
		throw std::invalid_argument{"wait for not joinable thread"};
	winapi::wait(native_.get());
	return exit_code();
}
bool winapi::thread::wait(std::chrono::milliseconds dur) const
{
	if (get_id() == this_thread::get_id())
		throw std::system_error{std::make_error_code(std::errc::resource_deadlock_would_occur),
					"join thread to self"};
	if (!joinable())
		throw std::invalid_argument{"wait for not joinable thread"};
	return winapi::wait(native_.get(), dur);
}
int winapi::thread::join()
{
	int code = wait();
	native_.reset();
	delete erasor_;
	erasor_ = nullptr;
	return code;
}
bool winapi::thread::join(std::chrono::milliseconds dur)
{
	if (wait(dur))
	{
		native_.reset();
		delete erasor_;
		erasor_ = nullptr;
		return true;
	}
	return false;
}
void winapi::thread::detach()
{
	native_.reset();
	if ((erasor_ != nullptr) && erasor_->oneside_released.test_and_set())
	{
		delete erasor_;
	}
	erasor_ = nullptr;
}
int winapi::thread::exit_code() const
{
	dword_t result;
	if (!winapi::calls::GetExitCodeThread(native_.get(), &result))
	{
		throw winapi::last_error("failed to get process exit code");
	}
	return static_cast< int >(result);
}
void winapi::thread::kill() const
{
	kill(-1);
}
void winapi::thread::kill(int exit_code) const
{
	if (!winapi::calls::TerminateThread(native_.get(), exit_code))
	{
		throw winapi::last_error("failed to terminate thread");
	}
}
winapi::thread::priority winapi::thread::get_priority() const
{
	int res = winapi::calls::GetThreadPriority(native_.get());
	if (res == static_cast< int >(priority::unset))
	{
		throw winapi::last_error("failed to set thread priority");
	}
	return static_cast< priority >(res);
}
void winapi::thread::set_priority(priority p) const
{
	assert(p != priority::unset);
	if (!winapi::calls::SetThreadPriority(native_.get(), static_cast< int >(p)))
	{
		throw winapi::last_error("failed to set thread priority");
	}
}

void std::swap(winapi::thread& lhs, winapi::thread& rhs) noexcept
{
	lhs.swap(rhs);
}

#endif
