module;
	#include <cassert>
module os.linapi;

#ifdef __linux__
import std;

[[noreturn]]
void linapi::this_thread::exit(int code)
{
	linapi::calls::pthread_exit(reinterpret_cast< void* >(code));
}
[[noreturn]]
void linapi::this_thread::exit(void* data)
{
	linapi::calls::pthread_exit(data);
}

linapi::thread_handle_t linapi::this_thread::native_handle()
{
	return linapi::calls::pthread_self();
}
linapi::thread::id linapi::this_thread::get_id()
{
	return this_thread::native_handle();
}
linapi::thread::priority linapi::this_thread::get_priority()
{
	int prior;
	scheduler_settings_t param;
	if (linapi::calls::pthread_getschedparam(this_thread::native_handle(), &prior, &param) != 0)
	{
		throw linapi::last_error("failed to get thread priority");
	}
	return static_cast< thread::priority >(prior);
}
void linapi::this_thread::set_priority(thread::priority p)
{
	assert(p != thread::priority::unset);
	if (linapi::calls::pthread_setschedprio(this_thread::native_handle(), static_cast< int >(p)) != 0)
	{
		throw linapi::last_error("failed to set thread priority");
	}
}

linapi::thread::thread(native_handle_type h) noexcept:
	native_(h)
{}
linapi::thread::thread(thread&& rhs) noexcept:
	native_(std::exchange(rhs.native_, {})),
	erasor_(std::exchange(rhs.erasor_, nullptr))
{}
linapi::thread::thread(thread_function_t f, void* data)
{
	if (linapi::calls::pthread_create(&native_, nullptr, f, data) != 0)
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
		if (linapi::calls::pthread_attr_init(&attributes_impl) != 0)
			throw linapi::last_error("failed to initialize pthread_attr_t");
		attributes = &attributes_impl;
	}
	if ((params.stack_size != 0) && linapi::calls::pthread_attr_setstacksize(attributes, params.stack_size) != 0)
		throw linapi::last_error("failed to set thread stack size");
	bool post_set_priority = (params.priority != priority::normal)
			&& (params.priority != priority::realtime)
			&& (params.priority != priority::greater_realtime);
	if (params.priority != priority::unset)
	{
		if (linapi::calls::pthread_attr_setinheritsched(attributes, explicit_priority) != 0)
			throw linapi::last_error("failed to set thread explicit shed");
		if (!post_set_priority && linapi::calls::pthread_attr_setschedpolicy(attributes,
						static_cast< int >(params.priority)) != 0)
			throw linapi::last_error("failed to set thread priority");
	}
	else if (linapi::calls::pthread_attr_setinheritsched(attributes, implicit_priority) != 0)
		throw linapi::last_error("failed to set thread implicit shed");

	if (linapi::calls::pthread_create(&native_, attributes, f, data) != 0)
	{
		if (params.attributes == nullptr) linapi::calls::pthread_attr_destroy(&attributes_impl);
		throw linapi::last_error("failed to start thread");
	}
	if ((params.attributes == nullptr) && (linapi::calls::pthread_attr_destroy(&attributes_impl) != 0))
		throw linapi::last_error("failed to destroy pthread_attr_t");
	if ((params.priority != priority::unset) && post_set_priority)
		set_priority(params.priority);
}
linapi::thread::~thread()
{
	if (joinable()) std::terminate();
}
linapi::thread& linapi::thread::operator=(thread&& rhs) noexcept
{
	thread dummy{std::move(*this)};
	erasor_ = std::exchange(rhs.erasor_, nullptr);
	native_ = std::exchange(rhs.native_, {});
	return *this;
}

unsigned int linapi::thread::hardware_concurrency() noexcept
{
	return std::thread::hardware_concurrency();
}

linapi::thread_handle_t linapi::thread::native_handle() const noexcept
{
	return native_;
}
linapi::thread_handle_t linapi::thread::release_handle() noexcept
{
	if ((erasor_ != nullptr) && erasor_->oneside_released.test_and_set())
	{
		delete erasor_;
	}
	erasor_ = nullptr;
	return std::exchange(native_, {});
}
void linapi::thread::swap(thread& rhs) noexcept
{
	std::swap(native_, rhs.native_);
	std::swap(erasor_, rhs.erasor_);
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
	if (linapi::calls::pthread_join(native_, &result) != 0)
	{
		throw linapi::last_error("failed to join thread");
	}
	native_ = id{};
	delete erasor_;
	erasor_ = nullptr;
	return result;
}
void linapi::thread::detach()
{
	if (linapi::calls::pthread_detach(native_) != 0)
	{
		throw linapi::last_error("failed to detach thread");
	}
	release_handle();
}
void linapi::thread::kill() const
{
	if (linapi::calls::pthread_kill(native_, static_cast< int >(linapi::signal::kill)) != 0)
	{
		throw linapi::last_error("failed to kill thread");
	}
}
linapi::thread::priority linapi::thread::get_priority() const
{
	int prior;
	scheduler_settings_t param;
	if (linapi::calls::pthread_getschedparam(native_, &prior, &param) != 0)
	{
		throw linapi::last_error("failed to get thread priority");
	}
	return static_cast< priority >(prior);
}
void linapi::thread::set_priority(priority p) const
{
	assert(p != priority::unset);
	scheduler_settings_t param{};
	if (linapi::calls::pthread_setschedparam(native_, static_cast< int >(p), &param) != 0)
	{
		throw linapi::last_error("failed to set thread priority");
	}
}

void std::swap(linapi::thread& lhs, linapi::thread& rhs) noexcept
{
	lhs.swap(rhs);
}

#endif
