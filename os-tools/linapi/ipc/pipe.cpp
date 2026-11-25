module;
	#ifdef __linux__
		#include <cassert>
		#include <cerrno>
		#include <fcntl.h>
		#include <os-tools/os_macro.hpp>
	#endif
module os.linapi;

#ifdef __linux__
import std;

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

void linapi::ipc::pipe::prevent_signal_emiting()
{
	THROW_IF_ERR0(prevent_signal_emiting, "failed to ignore sigpipe");
}
bool linapi::ipc::pipe::prevent_signal_emiting(std::error_code & err) noexcept
{
	return signal::ignore(signal::broken_pipe, err);
}

void linapi::ipc::pipe::set_traits(bool packet, bool nonblock)
{
	THROW_IF_ERR(set_traits, "failed to set pipe traits", packet, nonblock);
}
bool linapi::ipc::pipe::packet()
{
	THROW_IF_ERR_PROXY0(packet, "failed to get pipe traits");
}
void linapi::ipc::pipe::set_packet(bool packet)
{
	set_mode(native_.get(), handle_mode::packet, (packet ? handle_mode::packet : 0));
}
bool linapi::ipc::pipe::nonblock()
{
	THROW_IF_ERR_PROXY0(nonblock, "failed to get pipe traits");
}
void linapi::ipc::pipe::set_nonblock(bool nonblock)
{
	set_mode(native_.get(), handle_mode::nonblock, (nonblock ? handle_mode::nonblock : 0));
}

bool linapi::ipc::pipe::set_traits(bool packet, bool nonblock, std::error_code& err) noexcept
{
	return set_mode(native_.get(), handle_mode::packet | handle_mode::nonblock,
				(packet ? handle_mode::packet : 0)
				| (nonblock ? handle_mode::nonblock : 0), err);
}
bool linapi::ipc::pipe::packet(std::error_code& err) noexcept
{
	return get_mode(native_.get(), err) & handle_mode::packet;
}
bool linapi::ipc::pipe::set_packet(bool packet, std::error_code& err) noexcept
{
	return set_mode(native_.get(), handle_mode::packet, (packet ? handle_mode::packet : 0), err);
}
bool linapi::ipc::pipe::nonblock(std::error_code& err) noexcept
{
	return get_mode(native_.get(), err) & handle_mode::nonblock;
}
bool linapi::ipc::pipe::set_nonblock(bool nonblock, std::error_code& err) noexcept
{
	return set_mode(native_.get(), handle_mode::nonblock, (nonblock ? handle_mode::nonblock : 0), err);
}

std::string linapi::ipc::pipe::read()
{
	THROW_IF_ERR_PROXY0(read, "failed to read msg from pipe");
}
std::string linapi::ipc::pipe::read(std::error_code& err)
{
	std::string res;
	while (true)
	{
		res.push_back('\0');
		res.pop_back();
		res.resize_and_overwrite(res.capacity(), [this, &err, pre = res.size()](char* str, std::size_t sz) {
						std::span span{str + pre, sz - pre};
						return pre + read(std::as_writable_bytes(span), err);
					});
		if (err == std::errc::broken_pipe)
		{
			if (!res.empty()) err.clear();
			return res;
		}
		if (!err) return res;
		break;
	}
	return res;
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
void linapi::ipc::pipe::write(std::span< const std::byte > data)
{
	THROW_IF_ERR(write, "failed to write to pipe", data);
}
std::size_t linapi::ipc::pipe::write_once(std::span< const std::byte > data)
{
	THROW_IF_ERR_PROXY(write_once, "failed to write to pipe", data);
}
std::size_t linapi::ipc::pipe::read(std::span< std::byte > data)
{
	THROW_IF_ERR_PROXY(read, "failed to read from pipe", data);
}
std::size_t linapi::ipc::pipe::read_once(std::span< std::byte > data)
{
	THROW_IF_ERR_PROXY(read_once, "failed to read from pipe", data);
}

std::size_t linapi::ipc::pipe::write(std::span< const std::byte > data, std::error_code& err) noexcept
{
	std::size_t offset = 0;
	while (offset < data.size())
	{
		offset += write_once(data.subspan(offset), err);
		if (err) return offset;
	}
	return offset;
}
std::size_t linapi::ipc::pipe::write_once(std::span< const std::byte > data, std::error_code& err) noexcept
{
	assert(native_);
	std::ptrdiff_t written = linapi::calls::write(native_.get(), data.data(), data.size());
	process_error(written >= 0, err);
	return static_cast< std::size_t >(std::max(written, static_cast< std::ptrdiff_t >(0)));
}
std::size_t linapi::ipc::pipe::read(std::span< std::byte > data, std::error_code& err) noexcept
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
std::size_t linapi::ipc::pipe::read_once(std::span< std::byte > data, std::error_code& err) noexcept
{
	assert(native_);
	std::ptrdiff_t read_count = linapi::calls::read(native_.get(), data.data(), data.size());
	process_error(read_count >= 0, err);
	if (read_count == 0) err.assign(EPIPE, std::system_category());
	return static_cast< std::size_t >(std::max(read_count, static_cast< std::ptrdiff_t >(0)));
}
#endif
