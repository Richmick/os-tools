module;
	#include <os-tools/winapi/deps.hpp>
	#include <os-tools/os_macro.hpp>
module os.winapi;

#ifdef _WIN32

bool winapi::is_blank(handle_t h) noexcept
{
	return (h == nullptr) || (h == INVALID_HANDLE_VALUE);
}
void winapi::set_inheritance(inheritmode inherit, handle_t h)
{
	THROW_IF_ERR(set_inheritance, "failed to set handle inheritance", inherit, h);
}
bool winapi::set_inheritance(inheritmode inherit, handle_t h, std::error_code& err) noexcept
{
	constexpr dword_t flag = HANDLE_FLAG_INHERIT;
	process_error(winapi::calls::SetHandleInformation(h, flag, (inherit == inheritmode::all ? flag : 0)), err);
	return static_cast< bool >(err);
}
winapi::inheritmode winapi::get_inheritance(handle_t h)
{
	THROW_IF_ERR_PROXY(get_inheritance, "failed to get handle inheritance", h);
}
winapi::inheritmode winapi::get_inheritance(handle_t h, std::error_code& err) noexcept
{
	constexpr dword_t flag = HANDLE_FLAG_INHERIT;
	dword_t result;
	process_error(winapi::calls::GetHandleInformation(h, &result), err);
	return static_cast< inheritmode >(static_cast< bool >(result & flag));
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
void winapi::unique_handle::enable_inheritance() const
{
	set_inheritance(inheritmode::all, native_);
}
void winapi::unique_handle::disable_inheritance() const
{
	set_inheritance(inheritmode::none, native_);
}
bool winapi::unique_handle::enable_inheritance(std::error_code& err) const noexcept
{
	set_inheritance(inheritmode::all, native_, err);
	return static_cast< bool >(err);
}
bool winapi::unique_handle::disable_inheritance(std::error_code& err) const noexcept
{
	set_inheritance(inheritmode::none, native_, err);
	return static_cast< bool >(err);
}
winapi::inheritmode winapi::unique_handle::inheritance() const
{
	return winapi::get_inheritance(native_);
}
winapi::inheritmode winapi::unique_handle::inheritance(std::error_code& err) const noexcept
{
	return winapi::get_inheritance(native_, err);
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

winapi::handle_container_base::handle_container_base(unique_handle&& rhs) noexcept:
	native_(std::move(rhs))
{}
winapi::handle_container_base::handle_container_base(native_handle_type rhs) noexcept:
	native_(rhs)
{}
winapi::handle_t winapi::handle_container_base::native_handle() const noexcept
{
	return native_.get();
}
winapi::handle_t winapi::handle_container_base::release_handle() noexcept
{
	return native_.release();
}
void winapi::handle_container_base::close_handle() noexcept
{
	native_.close();
}
winapi::handle_container_base::operator bool() const noexcept
{
	return native_;
}
void winapi::handle_container_base::enable_handle_inheritance() const
{
	native_.enable_inheritance();
}
void winapi::handle_container_base::disable_handle_inheritance() const
{
	native_.disable_inheritance();
}
bool winapi::handle_container_base::enable_handle_inheritance(std::error_code& err) const noexcept
{
	return native_.enable_inheritance(err);
}
bool winapi::handle_container_base::disable_handle_inheritance(std::error_code& err) const noexcept
{
	return native_.disable_inheritance(err);
}
winapi::inheritmode winapi::handle_container_base::handle_inheritance() const
{
	return native_.inheritance();
}
winapi::inheritmode winapi::handle_container_base::handle_inheritance(std::error_code& err) const noexcept
{
	return native_.inheritance(err);
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
