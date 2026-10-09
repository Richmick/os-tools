module;
	#ifdef _WIN32
		#define WIN32_LEAN_AND_MEAN
		#include <windows.h>
	#endif
export module os.winapi:semaphore;

#ifdef _WIN32
#undef max

import std;
import :aliases;
import :handle;

export namespace winapi
{
	template< std::ptrdiff_t MaxValue = std::numeric_limits< std::ptrdiff_t >::max() >
	class counting_semaphore;
}

template< std::ptrdiff_t MaxValue >
class winapi::counting_semaphore
{
	static_assert(MaxValue >= 0);
public:
	using native_handle_type = handle_t;
	static constexpr inline std::ptrdiff_t max = MaxValue;

	counting_semaphore(const counting_semaphore&) = delete;
	explicit counting_semaphore(native_handle_type handle) noexcept;
	explicit counting_semaphore(std::ptrdiff_t desired);
	explicit counting_semaphore(std::ptrdiff_t desired, const std::string& name);
	explicit counting_semaphore(open_by_name_t, const std::string& name);

	native_handle_type native_handle() const& noexcept;
	native_handle_type release_handle() noexcept;

	std::ptrdiff_t counter();
	void release(std::ptrdiff_t update = 1);
	void acquire();
	void try_acquire();

private:
	static constexpr dword_t all_access = EVENT_ALL_ACCESS;
	unique_handle native_;
};

template< std::ptrdiff_t MaxValue >
winapi::counting_semaphore< MaxValue >::counting_semaphore(native_handle_type handle) noexcept:
	native_(handle)
{}
template< std::ptrdiff_t MaxValue >
winapi::counting_semaphore< MaxValue >::counting_semaphore(std::ptrdiff_t desired):
	native_(winapi::calls::CreateSemaphoreA(nullptr, desired, MaxValue, nullptr))
{
	if (!native_) throw winapi::last_error("failed to create anonymous semaphore");
}
template< std::ptrdiff_t MaxValue >
winapi::counting_semaphore< MaxValue >::counting_semaphore(std::ptrdiff_t desired, const std::string& name):
	native_(winapi::calls::CreateSemaphoreA(nullptr, desired, MaxValue, name.c_str()))
{
	if (!native_) throw winapi::last_error("failed to create named semaphore");
}
template< std::ptrdiff_t MaxValue >
winapi::counting_semaphore< MaxValue >::counting_semaphore(open_by_name_t, const std::string& name):
	native_(winapi::calls::OpenSemaphoreA(all_access, false, name.c_str()))
{
	if (!native_) throw winapi::last_error("failed to open semaphore");
}

template< std::ptrdiff_t MaxValue >
winapi::counting_semaphore< MaxValue >::native_handle_type
		winapi::counting_semaphore< MaxValue >::native_handle() const& noexcept
{
	return native_.get();
}
template< std::ptrdiff_t MaxValue >
winapi::counting_semaphore< MaxValue >::native_handle_type
		winapi::counting_semaphore< MaxValue >::release_handle() noexcept
{
	return native_.release();
}
template< std::ptrdiff_t MaxValue >
std::ptrdiff_t winapi::counting_semaphore< MaxValue >::counter()
{
	long result;
	if (!winapi::calls::ReleaseSemaphore(native_.get(), 0, &result))
	{
		throw winapi::last_error("failed to release 0 to semaphore");
	}
	return result;
}
template< std::ptrdiff_t MaxValue >
void winapi::counting_semaphore< MaxValue >::release(std::ptrdiff_t update)
{
	if (!winapi::calls::ReleaseSemaphore(native_.get(), update, nullptr))
	{
		throw winapi::last_error("failed to release points to semaphore");
	}
}
template< std::ptrdiff_t MaxValue >
void winapi::counting_semaphore< MaxValue >::acquire()
{
	if (winapi::calls::WaitForSingleObject(native_.get(), std::numeric_limits< dword_t >::max())
				!= winapi::wait_success_0)
	{
		throw winapi::last_error("failed to acquire semaphore");
	}
}

#endif
