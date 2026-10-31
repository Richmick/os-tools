module os.winapi;

#ifdef _WIN32
import std;

winapi::ipc::pipe::pipe():
	pipe(0)
{}
winapi::ipc::pipe::pipe(std::size_t desired_size)
{
	std::error_code err;
	*this = pipe{desired_size, err};
	throw_on_error(err, "failed to create anonymous pipe");
}
winapi::ipc::pipe::pipe(inheritmode inherit, std::size_t desired_size)
{
	std::error_code err;
	*this = pipe{inherit, desired_size, err};
	throw_on_error(err, "failed to create anonymous pipe");
}
winapi::ipc::pipe::pipe(std::error_code& err) noexcept:
	pipe(0, err)
{}
winapi::ipc::pipe::pipe(std::size_t desired_size, std::error_code& err) noexcept
{
	handle_t read, write;
	if (!winapi::calls::CreatePipe(&read, &write, nullptr, static_cast< dword_t >(desired_size)))
	{
		err = winapi::last_error_code();
		return;
	}
	err.clear();
	read_.reset(read);
	write_.reset(write);
}
winapi::ipc::pipe::pipe(inheritmode inherit, std::size_t desired_size, std::error_code& err) noexcept
{
	security_attributes_t attributes{
				.nLength = sizeof(security_attributes_t),
				.lpSecurityDescriptor = nullptr,
				.bInheritHandle = static_cast< bool >(inherit)
			};
	handle_t read, write;
	if (!winapi::calls::CreatePipe(&read, &write, &attributes, static_cast< dword_t >(desired_size)))
	{
		err = winapi::last_error_code();
		return;
	}
	err.clear();
	read_.reset(read);
	write_.reset(write);
}
winapi::ipc::pipe::pipe(native_handle_type read, native_handle_type write) noexcept:
	read_(read),
	write_(write)
{}

winapi::handle_t winapi::ipc::pipe::native_read_handle() const noexcept
{
	return read_.get();
}
winapi::handle_t winapi::ipc::pipe::native_write_handle() const noexcept
{
	return write_.get();
}
winapi::handle_t winapi::ipc::pipe::release_read_handle() noexcept
{
	return read_.release();
}
winapi::handle_t winapi::ipc::pipe::release_write_handle() noexcept
{
	return write_.release();
}
void winapi::ipc::pipe::close_read_handle() noexcept
{
	read_.reset();
}
void winapi::ipc::pipe::close_write_handle() noexcept
{
	write_.reset();
}
winapi::openmode winapi::ipc::pipe::mode() const noexcept
{
	if (read_)
	{
		if (write_) return openmode::inout;
		return openmode::in;
	}
	if (write_) return openmode::out;
	return openmode::closed;
}
bool winapi::ipc::pipe::can_read() const noexcept
{
	return read_;
}
bool winapi::ipc::pipe::can_write() const noexcept
{
	return write_;
}

winapi::ipc::pipe winapi::ipc::pipe::release_read() noexcept
{
	return pipe{read_.release(), nullptr};
}
winapi::ipc::pipe winapi::ipc::pipe::release_write() noexcept
{
	return pipe{nullptr, write_.release()};
}

void winapi::ipc::pipe::write(std::string_view data)
{
	write(std::as_bytes(std::span< const char >(data.data(), data.size())));
}
std::size_t winapi::ipc::pipe::try_write(std::string_view data)
{
	return try_write(std::as_bytes(std::span< const char >(data.data(), data.size())));;
}
#endif
