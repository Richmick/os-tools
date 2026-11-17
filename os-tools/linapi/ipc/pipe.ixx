module;
	#ifdef __linux__
		#include <cassert>
		#include <cerrno>
		#include <cstddef>
		#include <span>
		#include <string_view>
		#include <system_error>
	#endif
export module os.linapi:pipe;

#ifdef __linux__
import :aliases;
import :handle;

export namespace linapi::ipc
{
	class pipe: public handle_container_base
	{
	public:
		struct initial_params;

		using handle_container_base::handle_container_base;

		static void prevent_signal_emiting();
		static bool prevent_signal_emiting(std::error_code& err) noexcept;

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
		path name;
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
}

struct linapi::ipc::pipe::initial_params
{
	inheritmode inherit = inheritmode::none;
	bool nonblock = false;
	bool packet = false;
	[[maybe_unused]] std::size_t size_hint = 0;
};
struct linapi::ipc::named_pipe::initial_params
{
	openmode mode = openmode::inout;
	inheritmode inherit = inheritmode::none;
	bool nonblock = false;
	bool packet = false;
	bool prevent_open_existing = true;
	[[maybe_unused]] std::size_t read_size_hint = 0;
	[[maybe_unused]] std::size_t write_size_hint = 0;
};
struct linapi::ipc::named_pipe::open_params
{
	openmode mode = openmode::inout;
	inheritmode inherit = inheritmode::none;
	bool nonblock = false;
	bool packet = false;
};


template< std::size_t N >
void linapi::ipc::pipe::write(std::span< const std::byte, N > data)
{
	std::error_code err;
	write(data, err);
	throw_on_error(err, "failed to write to pipe");
}
template< std::size_t N >
std::size_t linapi::ipc::pipe::write_once(std::span< const std::byte, N > data)
{
	std::error_code err;
	std::size_t result = write_once(data, err);
	throw_on_error(err, "failed to write to pipe");
	return result;
}
template< std::size_t N >
std::size_t linapi::ipc::pipe::read(std::span< std::byte, N > data)
{
	std::error_code err;
	std::size_t result = read(data, err);
	throw_on_error(err, "failed to read from pipe");
	return result;
}
template< std::size_t N >
std::size_t linapi::ipc::pipe::read_once(std::span< std::byte, N > data)
{
	std::error_code err;
	std::size_t result = read_once(data, err);
	throw_on_error(err, "failed to read from pipe");
	return result;
}

template< std::size_t N >
std::size_t linapi::ipc::pipe::write(std::span< const std::byte, N > data, std::error_code& err) noexcept
{
	std::size_t offset = 0;
	while (offset < data.size())
	{
		offset += write_once(data.subspan(offset));
		if (err == std::errc::broken_pipe) return offset;
	}
	return offset;
}
template< std::size_t N >
std::size_t linapi::ipc::pipe::write_once(std::span< const std::byte, N > data, std::error_code& err) noexcept
{
	assert(native_);
	std::ptrdiff_t written = linapi::calls::write(native_.get(), data.data(), data.size());
	process_error(written >= 0, err);
	return static_cast< std::size_t >(std::max(written, static_cast< std::ptrdiff_t >(0)));
}
template< std::size_t N >
std::size_t linapi::ipc::pipe::read(std::span< std::byte, N > data, std::error_code& err) noexcept
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
std::size_t linapi::ipc::pipe::read_once(std::span< std::byte, N > data, std::error_code& err) noexcept
{
	assert(native_);
	std::ptrdiff_t read_count = linapi::calls::read(native_.get(), data.data(), data.size());
	process_error(read_count >= 0, err);
	if (read_count == 0) err.assign(EPIPE, std::system_category());
	return static_cast< std::size_t >(std::max(read_count, static_cast< std::ptrdiff_t >(0)));
}
#endif
