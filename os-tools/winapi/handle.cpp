module;
	#ifdef _WIN32
		#define WIN32_LEAN_AND_MEAN
		#include <windows.h>
	#endif
module os.winapi;

#ifdef _WIN32

bool winapi::is_blank(handle_t h) noexcept
{
	return (h == nullptr) || (h == INVALID_HANDLE_VALUE);
}

winapi::unique_handle::unique_handle(unique_handle&& rhs) noexcept:
	native_(rhs.release())
{}
winapi::unique_handle::unique_handle(handle_t handle) noexcept:
	native_(handle)
{}
winapi::unique_handle::unique_handle(std::nullptr_t) noexcept:
	native_(nullptr)
{}
winapi::unique_handle::~unique_handle()
{
	close();
}
winapi::unique_handle& winapi::unique_handle::operator=(unique_handle&& rhs) noexcept
{
	reset(rhs.release());
	return *this;
}

winapi::handle_t winapi::unique_handle::get() const noexcept
{
	return native_;
}
winapi::handle_t winapi::unique_handle::release() noexcept
{
	return std::exchange(native_, nullptr);
}
void winapi::unique_handle::reset(handle_t new_handle) noexcept
{
	close();
	native_ = new_handle;
}
void winapi::unique_handle::swap(unique_handle& rhs) noexcept
{
	std::swap(native_, rhs.native_);
}
void winapi::unique_handle::close() noexcept
{
	if (operator bool())
	{
		winapi::calls::CloseHandle(release());
	}
}

winapi::handle_t* winapi::unique_handle::operator&() noexcept
{
	close();
	return &native_;
}
winapi::unique_handle::operator bool() const noexcept
{
	return !is_blank(native_);
}
std::strong_ordering winapi::unique_handle::operator<=>(const unique_handle& rhs) const noexcept
{
	return native_ <=> rhs.native_;
}
std::strong_ordering winapi::unique_handle::operator<=>(handle_t rhs) const noexcept
{
	return native_ <=> rhs;
}
bool winapi::unique_handle::operator==(const unique_handle& rhs) const noexcept
{
	return native_ == rhs.native_;
}
bool winapi::unique_handle::operator==(handle_t rhs) const noexcept
{
	return native_ == rhs;
}
bool winapi::unique_handle::operator==(std::nullptr_t) const noexcept
{
	return !operator bool();
}

void std::swap(winapi::unique_handle& lhs, winapi::unique_handle& rhs) noexcept
{
	lhs.swap(rhs);
}
std::size_t std::hash< winapi::unique_handle >::operator()(const winapi::unique_handle& rhs)
{
	return hash< winapi::handle_t >::operator()(rhs.get());
}

#endif
