module os.linapi;

#ifdef __linux__

bool winapi::is_blank(handle_t h) noexcept
{
	return h < 0;
}

linapi::unique_handle::unique_handle(unique_handle&& rhs) noexcept:
	native_(rhs.release())
{}
linapi::unique_handle::unique_handle(handle_t handle) noexcept:
	native_(handle)
{}
linapi::unique_handle::unique_handle(std::nullptr_t) noexcept:
	native_(-1)
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
	return std::exchange(native_, -1);
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
		::CloseHandle(release());
	}
}

linapi::handle_t* linapi::unique_handle::operator&() noexcept
{
	close();
	return &native_;
}
linapi::unique_handle::operator bool() const noexcept
{
	return is_blank(native_);
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

void std::swap(linapi::unique_handle& lhs, linapi::unique_handle& rhs) noexcept
{
	lhs.swap(rhs);
}
std::size_t std::hash< linapi::unique_handle >::operator()(const linapi::unique_handle& rhs)
{
	return hash< linapi::handle_t >::operator()(rhs.get());
}

#endif
