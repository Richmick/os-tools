module;
	#ifdef __linux__
		#include <cstddef>
		#include <span>
		#include <string_view>
		#include <system_error>
		#include <utility>
		#include <fcntl.h>
	#endif
module os.linapi;

#ifdef __linux__
std::pair< linapi::ipc::pipe, linapi::ipc::pipe > linapi::ipc::create_pipe()
{
	return create_pipe(inheritmode::none);
}
std::pair< linapi::ipc::pipe, linapi::ipc::pipe >
		linapi::ipc::create_pipe(std::error_code& err) noexcept
{
	return create_pipe(inheritmode::none, err);
}
std::pair< linapi::ipc::pipe, linapi::ipc::pipe >
		linapi::ipc::create_pipe(inheritmode inherit)
{
	pipe::initial_params params{.inherit = inherit};
	return create_pipe(params);
}
std::pair< linapi::ipc::pipe, linapi::ipc::pipe >
		linapi::ipc::create_pipe(inheritmode inherit, std::error_code& err) noexcept
{
	pipe::initial_params params{.inherit = inherit};
	return create_pipe(params, err);
}
std::pair< linapi::ipc::pipe, linapi::ipc::pipe >
		linapi::ipc::create_pipe(const pipe::initial_params& params)
{
	std::error_code err;
	std::pair< pipe, pipe > result = create_pipe(params, err);
	throw_on_error(err, "failed to create anonymous pipe");
	return result;
}
std::pair< linapi::ipc::pipe, linapi::ipc::pipe >
		linapi::ipc::create_pipe(const pipe::initial_params& params, std::error_code& err) noexcept
{
	handle_t read_write[2];
	int flags = 0;
	if (params.inherit != inheritmode::all) flags |= O_CLOEXEC;
	if (params.nonblock) flags |= O_NONBLOCK;
	if (params.packet) flags |= O_DIRECT;
	if (linapi::calls::pipe2(read_write, flags) != 0)
	{
		err = linapi::last_error_code();
		return {};
	}
	err.clear();
	return {pipe{read_write[0]}, pipe{read_write[1]}};
}

void linapi::ipc::pipe::write(std::string_view data)
{
	write(std::as_bytes(std::span< const char >(data.data(), data.size())));
}
std::size_t linapi::ipc::pipe::write_once(std::string_view data)
{
	return write_once(std::as_bytes(std::span< const char >(data.data(), data.size())));;
}
std::size_t linapi::ipc::pipe::write(std::string_view data, std::error_code& err) noexcept
{
	return write(std::as_bytes(std::span< const char >(data.data(), data.size())), err);
}
std::size_t linapi::ipc::pipe::write_once(std::string_view data, std::error_code& err) noexcept
{
	return write_once(std::as_bytes(std::span< const char >(data.data(), data.size())), err);;
}
#endif
