module;
	#ifdef __linux__
		#include <fcntl.h>
	#endif
module os.linapi;

#ifdef __linux__
import std;

bool linapi::is_blank(handle_t h) noexcept
{
	return h < 0;
}
void linapi::set_inheritance(inheritmode inherit, handle_t h)
{
	std::error_code err;
	set_inheritance(inherit, h, err);
	throw_on_error(err, "failed to set handle inheritance");
}
bool linapi::set_inheritance(inheritmode inherit, handle_t h, std::error_code& err) noexcept
{
	constexpr int flag = FD_CLOEXEC;
	process_error(linapi::calls::fcntl(h, F_SETFD, (inherit == inheritmode::all ? 0 : flag)) != -1, err);
	return !static_cast< bool >(err);
}

linapi::unique_handle::unique_handle(unique_handle&& rhs) noexcept:
	native_(rhs.release())
{}
linapi::unique_handle::unique_handle(handle_t handle) noexcept:
	native_(handle)
{}
linapi::unique_handle::unique_handle(std::nullptr_t) noexcept:
	native_(empty_handle)
{}
linapi::unique_handle::~unique_handle()
{
	close();
}
linapi::unique_handle& linapi::unique_handle::operator=(unique_handle&& rhs) noexcept
{
	reset(rhs.release());
	return *this;
}

linapi::handle_t linapi::unique_handle::get() const noexcept
{
	return native_;
}
linapi::handle_t linapi::unique_handle::release() noexcept
{
	return std::exchange(native_, empty_handle);
}
void linapi::unique_handle::reset(handle_t new_handle) noexcept
{
	close();
	native_ = new_handle;
}
void linapi::unique_handle::swap(unique_handle& rhs) noexcept
{
	std::swap(native_, rhs.native_);
}
void linapi::unique_handle::close() noexcept
{
	if (operator bool())
	{
		linapi::calls::close(release());
	}
}
void linapi::unique_handle::enable_inheritance() const
{
	set_inheritance(inheritmode::all, native_);
}
void linapi::unique_handle::disable_inheritance() const
{
	set_inheritance(inheritmode::none, native_);
}
bool linapi::unique_handle::enable_inheritance(std::error_code& err) const noexcept
{
	set_inheritance(inheritmode::all, native_, err);
	return static_cast< bool >(err);
}
bool linapi::unique_handle::disable_inheritance(std::error_code& err) const noexcept
{
	set_inheritance(inheritmode::none, native_, err);
	return static_cast< bool >(err);
}

linapi::unique_handle::operator bool() const noexcept
{
	return !is_blank(native_);
}
std::strong_ordering linapi::unique_handle::operator<=>(const unique_handle& rhs) noexcept
{
	return native_ <=> rhs.native_;
}
std::strong_ordering linapi::unique_handle::operator<=>(handle_t rhs) noexcept
{
	return native_ <=> rhs;
}
bool linapi::unique_handle::operator==(std::nullptr_t) noexcept
{
	return !operator bool();
}

linapi::handle_container_base::handle_container_base(unique_handle&& rhs) noexcept:
	native_(std::move(rhs))
{}
linapi::handle_container_base::handle_container_base(native_handle_type rhs) noexcept:
	native_(rhs)
{}
linapi::handle_t linapi::handle_container_base::native_handle() const noexcept
{
	return native_.get();
}
linapi::handle_t linapi::handle_container_base::release_handle() noexcept
{
	return native_.release();
}
void linapi::handle_container_base::close_handle() noexcept
{
	native_.close();
}
linapi::handle_container_base::operator bool() const noexcept
{
	return native_;
}
void linapi::handle_container_base::enable_handle_inheritance() const
{
	native_.enable_inheritance();
}
void linapi::handle_container_base::disable_handle_inheritance() const
{
	native_.disable_inheritance();
}
bool linapi::handle_container_base::enable_handle_inheritance(std::error_code& err) const noexcept
{
	return native_.enable_inheritance(err);
}
bool linapi::handle_container_base::disable_handle_inheritance(std::error_code& err) const noexcept
{
	return native_.disable_inheritance(err);
}

void std::swap(linapi::unique_handle& lhs, linapi::unique_handle& rhs) noexcept
{
	lhs.swap(rhs);
}
std::size_t std::hash< linapi::unique_handle >::operator()(const linapi::unique_handle& rhs)
{
	return hash< linapi::handle_t >::operator()(rhs.get());
}

#endif
