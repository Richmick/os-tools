module;
	#include <cassert>
export module os.linapi:pipe;

#ifdef __linux__
import std;
import :aliases;
import :handle;

export namespace linapi::ipc
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
	class resource_anchor
	{
	public:
		path name;

		resource_anchor() = default;
		resource_anchor(resource_anchor&&) = default;
		~resource_anchor();
	};

	std::pair< pipe, pipe > create_pipe();
	std::pair< pipe, pipe > create_pipe(std::error_code& err) noexcept;
	std::pair< pipe, pipe > create_pipe(inheritmode inherit);
	std::pair< pipe, pipe > create_pipe(inheritmode inherit, std::error_code& err) noexcept;
	std::pair< pipe, pipe > create_pipe(const pipe::initial_params& params);
	std::pair< pipe, pipe > create_pipe(const pipe::initial_params& params, std::error_code& err) noexcept;

	std::pair< pipe, resource_anchor > create_server_pipe(path name, openmode mode);
}

struct linapi::ipc::pipe::initial_params
{
	inheritmode inherit = inheritmode::none;
	bool nonblock = false;
	bool packet = false;
	[[maybe_unused]] std::size_t size_hint = 0;
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
	return static_cast< std::size_t >(written);
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
	return static_cast< std::size_t >(read_count);
}
#endif
