module os.winapi;

#ifdef _WIN32

[[noreturn]]
void winapi::this_thread::exit(int code)
{
	winapi::ExitThread(code);
}
winapi::handle_t winapi::this_thread::native()
{
	return winapi::GetCurrentThread();
}
int winapi::this_thread::id()
{
	return winapi::GetCurrentThreadId();
}
winapi::priority winapi::this_thread::priority()
{
	return static_cast< winapi::priority >(winapi::GetThreadPriority(native()));
}
void winapi::this_thread::set_priority(winapi::priority p)
{
	if (!winapi::SetThreadPriority(native(), static_cast< dword_t >(p)))
	{
		throw winapi::last_error("failed to set thread priority");
	}
}

winapi::thread::thread(unique_handle&& h) noexcept:
	native_(std::move(h))
{}
winapi::thread::thread(handle_t h) noexcept:
	native_(h)
{}
winapi::thread::thread(thread&& rhs) noexcept:
	native_(std::move(rhs.native_))
{}
winapi::thread::thread(thread_function_t f, void* data)
{
	native_ = unique_handle{winapi::CreateThread(nullptr, 0, f, data, 0, nullptr)};
	if (!native_)
	{
		throw winapi::last_error("failed to start thread");
	}
}
winapi::thread::thread(const initial_params& params, thread_function_t f, void* data)
{
	security_attributes_t attributes = get_attributes(params);
	native_ = unique_handle{winapi::CreateThread(&attributes, params.stack_size,
					f, data, get_flags(params), nullptr)};
	if (!native_)
	{
		throw winapi::last_error("failed to start thread");
	}
}
winapi::thread::thread(const process& proc, thread_function_t f, void* data)
{
	native_ = unique_handle{winapi::CreateRemoteThread(proc.native(), nullptr, 0, f, data, 0, nullptr)};
	if (!native_)
	{
		throw winapi::last_error("failed to start thread");
	}
}
winapi::thread::thread(const process& proc, const initial_params& params, thread_function_t f, void* data)
{
	security_attributes_t attributes = get_attributes(params);
	native_ = unique_handle{winapi::CreateRemoteThread(proc.native(), &attributes, params.stack_size,
					f, data, get_flags(params), nullptr)};
	if (!native_)
	{
		throw winapi::last_error("failed to start thread");
	}
}
winapi::thread::~thread()
{
	if (joinable()) std::terminate();
}
winapi::thread& winapi::thread::operator=(thread&& rhs) noexcept
{
	if (joinable()) std::terminate();
	native_ = std::move(rhs.native_);
	return *this;
}

winapi::security_attributes_t winapi::thread::get_attributes(const initial_params& params)
{
	security_attributes_t attributes = params.attributes;
	if (params.inherit_handles) attributes.bInheritHandle = true;
	if (params.attributes.nLength == 0) attributes.nLength = sizeof(security_attributes_t);
	return attributes;
}
winapi::dword_t winapi::thread::get_flags(const initial_params& params)
{
	dword_t flags = 0;
	if (params.create_suspended) flags |= create_suspended;
	if (params.defer_stack_commit) flags |= stack_size_param_is_a_reservation;
	return flags;
}

winapi::handle_t winapi::thread::native() const noexcept
{
	return native_.get();
}
winapi::unique_handle winapi::thread::release()
{
	return std::move(native_);
}
void winapi::thread::swap(thread& rhs) noexcept
{
	native_.swap(rhs.native_);
}
int winapi::thread::id()
{
	dword_t result = winapi::GetThreadId(native_.get());
	if (result == 0) throw winapi::last_error("failed to get thread id");
	return static_cast< int >(result);
}
int winapi::thread::process_id()
{
	dword_t result = winapi::GetProcessIdOfThread(native_.get());
	if (result == 0) throw winapi::last_error("failed to get process id of thread");
	return static_cast< int >(result);
}
bool winapi::thread::joinable() const noexcept
{
	return native_;
}
void winapi::thread::wait()
{
	if (id() == this_thread::id())
		throw std::system_error{std::make_error_code(std::errc::resource_deadlock_would_occur),
					"join thread to self"};
	if (!joinable())
		throw std::invalid_argument{"wait for not joinable thread"};
	winapi::wait(native_.get());
}
bool winapi::thread::wait(std::chrono::milliseconds dur)
{
	if (id() == this_thread::id())
		throw std::system_error{std::make_error_code(std::errc::resource_deadlock_would_occur),
					"join thread to self"};
	if (!joinable())
		throw std::invalid_argument{"wait for not joinable thread"};
	return winapi::wait(native_.get(), dur);
}
void winapi::thread::join()
{
	wait();
	native_.reset();
}
bool winapi::thread::join(std::chrono::milliseconds dur)
{
	if (wait(dur))
	{
		native_.reset();
		return true;
	}
	return false;
}
void winapi::thread::detach()
{
	native_.reset();
}
int winapi::thread::exit_code()
{
	dword_t result;
	if (!winapi::GetExitCodeThread(native_.get(), &result))
	{
		throw winapi::last_error("failed to get process exit code");
	}
	return static_cast< int >(result);
}
void winapi::thread::kill()
{
	kill(-1);
}
void winapi::thread::kill(int exit_code)
{
	if (!winapi::TerminateThread(native_.get(), exit_code))
	{
		throw winapi::last_error("failed to terminate thread");
	}
}
winapi::priority winapi::thread::priority()
{
	return static_cast< winapi::priority >(winapi::GetThreadPriority(native_.get()));
}
void winapi::thread::set_priority(winapi::priority p)
{
	if (!winapi::SetThreadPriority(native_.get(), static_cast<dword_t>(p)))
	{
		throw winapi::last_error("failed to set thread priority");
	}
}

void std::swap(winapi::thread& lhs, winapi::thread& rhs) noexcept
{
	lhs.swap(rhs);
}

#endif
