module;
	#ifdef _WIN32
		#include <cassert>
	#endif
export module os.winapi:pipe;

#ifdef _WIN32
import std;
import :aliases;
import :handle;

export namespace winapi::ipc
{
	class pipe: public handle_container_base
	{
	public:
		struct initial_params;

		using handle_container_base::handle_container_base;

		template< std::size_t N = std::dynamic_extent >
		void write(std::span< const std::byte, N > data);
		void write(std::string_view data);
		template< std::size_t N = std::dynamic_extent >
		std::size_t write_once(std::span< const std::byte, N > data);
		std::size_t write_once(std::string_view data);

		template< std::size_t N = std::dynamic_extent >
		std::size_t write(std::span< const std::byte, N > data, std::error_code& err) noexcept;
		std::size_t write(std::string_view data, std::error_code& err) noexcept;
		template< std::size_t N = std::dynamic_extent >
		std::size_t write_once(std::span< const std::byte, N > data, std::error_code& err) noexcept;
		std::size_t write_once(std::string_view data, std::error_code& err) noexcept;

		template< std::size_t N = std::dynamic_extent >
		std::size_t read(std::span< std::byte, N > data);
		template< std::size_t N = std::dynamic_extent >
		std::size_t read_once(std::span< std::byte, N > data);

		template< std::size_t N = std::dynamic_extent >
		std::size_t read(std::span< std::byte, N > data, std::error_code& err) noexcept;
		template< std::size_t N = std::dynamic_extent >
		std::size_t read_once(std::span< std::byte, N > data, std::error_code& err) noexcept;
	};
	std::pair< pipe, pipe > create_pipe();
	std::pair< pipe, pipe > create_pipe(std::error_code& err) noexcept;
	std::pair< pipe, pipe > create_pipe(inheritmode inherit);
	std::pair< pipe, pipe > create_pipe(inheritmode inherit, std::error_code& err) noexcept;
	std::pair< pipe, pipe > create_pipe(const pipe::initial_params& params);
	std::pair< pipe, pipe > create_pipe(const pipe::initial_params& params, std::error_code& err) noexcept;

	class named_pipe: public pipe
	{
	public:
		struct initial_params;
		struct open_params;

		using pipe::pipe; // handle_container_base constructors

		void flush();
		bool flush(std::error_code& err) noexcept;
		void connect();
		bool connect(std::error_code& err) noexcept;
	};
	named_pipe create_server_pipe(string name, openmode mode);
	named_pipe create_server_pipe(string name, openmode mode, std::error_code& err) noexcept;
	named_pipe create_server_pipe(string name, openmode mode, inheritmode inherit);
	named_pipe create_server_pipe(string name, openmode mode, inheritmode inherit,
				std::error_code& err) noexcept;
	named_pipe create_server_pipe(const named_pipe::initial_params& params, string name);
	named_pipe create_server_pipe(const named_pipe::initial_params& params, string name,
				std::error_code& err) noexcept;

	named_pipe open_client_pipe(string name, openmode mode);
	named_pipe open_client_pipe(string name, openmode mode, std::error_code& err) noexcept;
	named_pipe open_client_pipe(string name, openmode mode, inheritmode inherit);
	named_pipe open_client_pipe(string name, openmode mode, inheritmode inherit,
				std::error_code& err) noexcept;
	named_pipe open_client_pipe(const named_pipe::open_params& params, string name);
	named_pipe open_client_pipe(const named_pipe::open_params& params, string name,
				std::error_code& err) noexcept;

	// Winapi specific
	named_pipe create_server_pipe(std::string name, openmode mode);
	named_pipe create_server_pipe(std::string name, openmode mode, std::error_code& err) noexcept;
	named_pipe create_server_pipe(std::string name, openmode mode, inheritmode inherit);
	named_pipe create_server_pipe(std::string name, openmode mode, inheritmode inherit,
				std::error_code& err) noexcept;
	named_pipe create_server_pipe(const named_pipe::initial_params& params, std::string name);
	named_pipe create_server_pipe(const named_pipe::initial_params& params, std::string name,
				std::error_code& err) noexcept;

	named_pipe open_client_pipe(std::string name, openmode mode);
	named_pipe open_client_pipe(std::string name, openmode mode, std::error_code& err) noexcept;
	named_pipe open_client_pipe(std::string name, openmode mode, inheritmode inherit);
	named_pipe open_client_pipe(std::string name, openmode mode, inheritmode inherit,
				std::error_code& err) noexcept;
	named_pipe open_client_pipe(const named_pipe::open_params& params, std::string name);
	named_pipe open_client_pipe(const named_pipe::open_params& params, std::string name,
				std::error_code& err) noexcept;
}

struct winapi::ipc::pipe::initial_params
{
	inheritmode inherit = inheritmode::none;
	[[maybe_unused]] bool nonblock = false;
	[[maybe_unused]] bool packet = false;
	std::size_t size_hint = 0;
	void* security_descriptor = nullptr;
};
struct winapi::ipc::named_pipe::initial_params
{
	openmode mode = openmode::inout;
	inheritmode inherit = inheritmode::none;
	bool nonblock = false;
	bool packet = false;
	bool read_bytes = false; ///< Force byte reading
	bool prevent_open_existing = true;
	bool overlapped = false;
	std::size_t read_size_hint = 0;
	std::size_t write_size_hint = 0;
	void* security_descriptor = nullptr;
	std::chrono::milliseconds default_timeout{0};
};
struct winapi::ipc::named_pipe::open_params
{
	openmode mode = openmode::inout;
	inheritmode inherit = inheritmode::none;
	bool nonblock = false;
	bool packet = false;
	bool overlapped = false;
	void* security_descriptor = nullptr;
};


template< std::size_t N >
void winapi::ipc::pipe::write(std::span< const std::byte, N > data)
{
	std::error_code err;
	write(data, err);
	throw_on_error(err, "failed to write to pipe");
}
template< std::size_t N >
std::size_t winapi::ipc::pipe::write_once(std::span< const std::byte, N > data)
{
	std::error_code err;
	std::size_t result = write_once(data, err);
	throw_on_error(err, "failed to write to pipe");
	return result;
}
template< std::size_t N >
std::size_t winapi::ipc::pipe::read(std::span< std::byte, N > data)
{
	std::error_code err;
	std::size_t result = read(data, err);
	throw_on_error(err, "failed to read from pipe");
	return result;
}
template< std::size_t N >
std::size_t winapi::ipc::pipe::read_once(std::span< std::byte, N > data)
{
	std::error_code err;
	std::size_t result = read_once(data, err);
	throw_on_error(err, "failed to read from pipe");
	return result;
}

template< std::size_t N >
std::size_t winapi::ipc::pipe::write(std::span< const std::byte, N > data, std::error_code& err) noexcept
{
	std::size_t offset = 0;
	while (offset < data.size())
	{
		offset += write_once(data.subspan(offset), err);
		if (err == std::errc::broken_pipe) return offset;
	}
	return offset;
}
template< std::size_t N >
std::size_t winapi::ipc::pipe::write_once(std::span< const std::byte, N > data, std::error_code& err) noexcept
{
	constexpr dword_t max = std::numeric_limits< dword_t >::max();

	dword_t written;
	const dword_t size = (data.size() < max ? static_cast< dword_t >(data.size()) : max);
	assert(native_);
	process_error(winapi::calls::WriteFile(native_.get(), data.data(), size, &written, nullptr), err);
	return static_cast< std::size_t >(written);
}
template< std::size_t N >
std::size_t winapi::ipc::pipe::read(std::span< std::byte, N > data, std::error_code& err) noexcept
{
	std::size_t offset = 0;
	while (offset < data.size())
	{
		std::size_t delta = read_once(data.subspan(offset), err);
		offset += delta;
		if ((delta == 0) || static_cast< bool >(err)) return offset;
	}
	return offset;
}
template< std::size_t N >
std::size_t winapi::ipc::pipe::read_once(std::span< std::byte, N > data, std::error_code& err) noexcept
{
	constexpr dword_t max = std::numeric_limits< dword_t >::max();

	dword_t read_count;
	const dword_t size = (data.size() < max ? static_cast< dword_t >(data.size()) : max);
	assert(native_);
	process_error(winapi::calls::ReadFile(native_.get(), data.data(), size, &read_count, nullptr), err);
	return static_cast< std::size_t >(read_count);
}
#endif
