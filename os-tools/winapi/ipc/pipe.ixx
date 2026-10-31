module;
	#include <cassert>
export module os.winapi:pipe;

#ifdef _WIN32
import std;
import :aliases;
import :handle;

export namespace winapi::ipc
{
	class pipe;
}

class winapi::ipc::pipe
{
public:
	using native_handle_type = handle_t;

	explicit pipe();
	explicit pipe(std::size_t desired_size);
	explicit pipe(inheritmode inherit, std::size_t desired_size = 0);

	explicit pipe(std::error_code& err) noexcept;
	explicit pipe(std::size_t desired_size, std::error_code& err) noexcept;
	explicit pipe(inheritmode inherit, std::size_t desired_size, std::error_code& err) noexcept;
	explicit pipe(native_handle_type read, native_handle_type write) noexcept;

	native_handle_type native_read_handle() const noexcept;
	native_handle_type native_write_handle() const noexcept;
	native_handle_type release_read_handle() noexcept;
	native_handle_type release_write_handle() noexcept;
	void close_read_handle() noexcept;
	void close_write_handle() noexcept;

	openmode mode() const noexcept;
	bool can_read() const noexcept;
	bool can_write() const noexcept;

	pipe release_read() noexcept;
	pipe release_write() noexcept;

	
	template< std::size_t N = std::dynamic_extent >
	void write(std::span< const std::byte, N > data);
	void write(std::string_view data);
	template< std::size_t N = std::dynamic_extent >
	std::size_t try_write(std::span< const std::byte, N > data);
	std::size_t try_write(std::string_view data);

	template< std::size_t N = std::dynamic_extent >
	void read(std::span< std::byte, N > data);
	template< std::size_t N = std::dynamic_extent >
	std::size_t try_read(std::span< std::byte, N > data);

private:
	unique_handle read_, write_;
};

template< std::size_t N >
void winapi::ipc::pipe::write(std::span< const std::byte, N > data)
{
	std::size_t offset = 0;
	while (offset < data.size())
	{
		offset += try_write(data.subspan(offset));
	}
}
template< std::size_t N >
std::size_t winapi::ipc::pipe::try_write(std::span< const std::byte, N > data)
{
	constexpr dword_t max = std::numeric_limits< dword_t >::max();

	dword_t written;
	const dword_t size = (data.size() < max ? static_cast< dword_t >(data.size()) : max);
	assert(write_);
	if (!winapi::calls::WriteFile(write_.get(), data.data(), size, &written, nullptr))
	{
		throw winapi::last_error("failed to write to pipe");
	}
	return static_cast< std::size_t >(written);
}
template< std::size_t N >
void winapi::ipc::pipe::read(std::span< std::byte, N > data)
{
	std::size_t offset = 0;
	while (offset < data.size())
	{
		offset += try_read(data.subspan(offset));
	}
}
template< std::size_t N >
std::size_t winapi::ipc::pipe::try_read(std::span< std::byte, N > data)
{
	constexpr dword_t max = std::numeric_limits< dword_t >::max();

	dword_t read_count;
	const dword_t size = (data.size() < max ? static_cast< dword_t >(data.size()) : max);
	assert(read_);
	if (!winapi::calls::ReadFile(read_.get(), data.data(), size, &read_count, nullptr))
	{
		throw winapi::last_error("failed to write to pipe");
	}
	return static_cast< std::size_t >(read_count);
}
#endif
