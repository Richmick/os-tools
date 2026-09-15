module os.linapi;

#ifdef __linux__
import std;

[[noreturn]]
void linapi::this_thread::exit(int code)
{
	linapi::pthread_exit(reinterpret_cast< void* >(code));
}
[[noreturn]]
void linapi::this_thread::exit(void* data)
{
	linapi::pthread_exit(data);
}

linapi::thread_handle_t linapi::this_thread::native()
{
	return linapi::pthread_self();
}
linapi::thread_handle_t linapi::this_thread::native_handle()
{
	return linapi::pthread_self();
}
linapi::thread::id linapi::this_thread::get_id()
{
	return this_thread::native();
}
linapi::thread::priority linapi::this_thread::get_priority()
{
	int prior;
	scheduler_settings_t param;
	if (linapi::pthread_getschedparam(this_thread::native(), &prior, &param) != 0)
	{
		throw linapi::last_error("failed to get thread priority");
	}
	return static_cast< thread::priority >(prior);
}
void linapi::this_thread::set_priority(thread::priority p)
{
	if (linapi::pthread_setschedprio(this_thread::native(), static_cast< int >(p)) != 0)
	{
		throw linapi::last_error("failed to set thread priority");
	}
}

linapi::thread::thread(native_handle_type h) noexcept:
	native_(h)
{}
linapi::thread::thread(thread&& rhs) noexcept:
	native_(rhs.release())
{}
linapi::thread::thread(thread_function_t f, void* data)
{
	if (linapi::pthread_create(&native_, nullptr, f, data) != 0)
	{
		throw linapi::last_error("failed to start thread");
	}
}
linapi::thread::thread(const initial_params& params, thread_function_t f, void* data)
{
	thread_attributes_t attributes_impl;
	thread_attributes_t* attributes = params.attributes;
	if (attributes == nullptr)
	{
		if (linapi::pthread_attr_init(&attributes_impl) != 0)
			throw linapi::last_error("failed to initialize pthread_attr_t");
		attributes = &attributes_impl;
	}
	if (linapi::pthread_attr_setstacksize(attributes, params.stack_size) != 0)
		throw linapi::last_error("failed to set thread stack size");
	if (params.use_priority)
	{
		if (linapi::pthread_attr_setinheritsched(attributes, explicit_priority) != 0)
			throw linapi::last_error("failed to set thread explicit shed");
		if (linapi::pthread_attr_setschedpolicy(attributes, static_cast< int >(params.explicit_priority)) != 0)
			throw linapi::last_error("failed to set thread priority");
	}
	else if (linapi::pthread_attr_setinheritsched(attributes, implicit_priority) != 0)
		throw linapi::last_error("failed to set thread implicit shed");

	if (linapi::pthread_create(&native_, attributes, f, data) != 0)
	{
		if (params.attributes == nullptr) linapi::pthread_attr_destroy(&attributes_impl);
		throw linapi::last_error("failed to start thread");
	}
	if ((params.attributes == nullptr) && (linapi::pthread_attr_destroy(&attributes_impl) != 0))
		throw linapi::last_error("failed to destroy pthread_attr_t");
}
linapi::thread::~thread()
{
	if (joinable()) std::terminate();
}
linapi::thread& linapi::thread::operator=(thread&& rhs) noexcept
{
	if (joinable()) std::terminate();
	native_ = rhs.release();
	return *this;
}

unsigned int linapi::thread::hardware_concurrency() noexcept
{
	return std::thread::hardware_concurrency();
}

linapi::thread_handle_t linapi::thread::native() const noexcept
{
	return native_;
}
linapi::thread_handle_t linapi::thread::native_handle() const noexcept
{
	return native_;
}
linapi::thread_handle_t linapi::thread::release() noexcept
{
	return std::exchange(native_, {});
}
void linapi::thread::swap(thread& rhs) noexcept
{
	std::swap(native_, rhs.native_);
}
linapi::thread::id linapi::thread::get_id() const
{
	return native_;
}
int linapi::thread::get_process_id() const
{
	return this_process::get_id();
}
bool linapi::thread::joinable() const noexcept
{
	return native_ != id{};
}
int linapi::thread::join()
{
	return static_cast< int >(reinterpret_cast< std::size_t >(join_val()));
}
void* linapi::thread::join_val()
{
	if (get_id() == this_thread::get_id())
		throw std::system_error{std::make_error_code(std::errc::resource_deadlock_would_occur),
					"join thread to self"};
	if (!joinable())
		throw std::invalid_argument{"join for not joinable thread"};
	void* result;
	if (linapi::pthread_join(native_, &result) != 0)
	{
		throw linapi::last_error("failed to join thread");
	}
	release();
	return result;
}
void linapi::thread::detach()
{
	if (linapi::pthread_detach(native_) != 0)
	{
		throw linapi::last_error("failed to detach thread");
	}
	release();
}
void linapi::thread::kill() const
{
	if (linapi::pthread_kill(native_, static_cast< int >(linapi::signal::kill)) != 0)
	{
		throw linapi::last_error("failed to kill thread");
	}
}
linapi::thread::priority linapi::thread::get_priority() const
{
	int prior;
	scheduler_settings_t param;
	if (linapi::pthread_getschedparam(native_, &prior, &param) != 0)
	{
		throw linapi::last_error("failed to get thread priority");
	}
	return static_cast< priority >(prior);
}
void linapi::thread::set_priority(priority p) const
{
	if (linapi::pthread_setschedprio(native_, static_cast< int >(p)) != 0)
	{
		throw linapi::last_error("failed to set thread priority");
	}
}

void std::swap(linapi::thread& lhs, linapi::thread& rhs) noexcept
{
	lhs.swap(rhs);
}

#endif
