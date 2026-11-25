module;
	#ifdef __linux__
		#include <cassert>
		#include <sys/socket.h>
		#include <os-tools/os_macro.hpp>
	#endif
module os.linapi;

#ifdef __linux__
import std;

linapi::net::socket::socket(address_type addr, type t, protocol proto)
{
	THROW_IF_ERR_CTOR(socket, "failed to create socket", addr, t, proto);
}
linapi::net::socket::socket(address_type addr, type t, protocol proto, std::error_code& err) noexcept:
	socket(addr, t, proto, inheritmode::none, err)
{}
linapi::net::socket::socket(address_type addr, type t, protocol proto, inheritmode inherit)
{
	THROW_IF_ERR_CTOR(socket, "failed to create socket", addr, t, proto, inherit);
}
linapi::net::socket::socket(address_type addr, type t, protocol proto, inheritmode inherit, std::error_code& err) noexcept:
	handle_container_base(linapi::calls::socket(static_cast< int >(addr),
				static_cast< int >(t) | (inherit == inheritmode::all ? 0 : SOCK_CLOEXEC),
				static_cast< int >(proto)))
{
	process_error(*this, err);
}

void linapi::net::socket::bind(const_address_link addr) const
{
	THROW_IF_ERR(bind, "failed to bind socket", addr);
}
bool linapi::net::socket::bind(const_address_link addr, std::error_code& err) const noexcept
{
	return process_error(linapi::calls::bind(native_.get(), addr.first, addr.second) == 0, err);
}
void linapi::net::socket::listen(int max_conns) const
{
	THROW_IF_ERR(listen, "failed to listen socket", max_conns);
}
bool linapi::net::socket::listen(int max_conns, std::error_code & err) const noexcept
{
	return process_error(linapi::calls::listen(native_.get(), max_conns) == 0, err);
}
linapi::net::socket linapi::net::socket::accept() const
{
	THROW_IF_ERR_PROXY0(accept, "failed to accept socket connection");
}
linapi::net::socket linapi::net::socket::accept(std::error_code& err) const noexcept
{
	socket result = socket{linapi::calls::accept(native_.get(), nullptr, nullptr)};
	process_error(result, err);
	return result;
}
void linapi::net::socket::connect(const_address_link addr) const
{
	THROW_IF_ERR(connect, "failed to connect socket", addr);
}
bool linapi::net::socket::connect(const_address_link addr, std::error_code& err) const noexcept
{
	return process_error(linapi::calls::connect(native_.get(), addr.first, addr.second) == 0, err);
}
void linapi::net::socket::shutdown(bool close_read, bool close_write) const
{
	THROW_IF_ERR(shutdown, "failed to shutdown socket operations", close_read, close_write);
}
bool linapi::net::socket::shutdown(bool close_read, bool close_write, std::error_code& err) const noexcept
{
	int mode = 0;
	if (close_read)
		if (close_write) mode = SHUT_RDWR;
		else mode = SHUT_RD;
	else if (close_write) mode = SHUT_WR;
	else return true;

	return process_error(linapi::calls::shutdown(native_.get(), mode) == 0, err);
}

linapi::net::address_any linapi::net::socket::address() const
{
	THROW_IF_ERR_PROXY0(address, "failed to obtain socket address (name)");
}
linapi::net::address_any linapi::net::socket::address(std::error_code& err) const noexcept
{
	address_any result;
	address_link link = result;
	process_error(linapi::calls::getsockname(native_.get(), link.first, &link.second) == 0, err);
	return result;
}
linapi::net::address_any linapi::net::socket::peer_address() const
{
	THROW_IF_ERR_PROXY0(address, "failed to obtain socket peer address (name)");
}
linapi::net::address_any linapi::net::socket::peer_address(std::error_code& err) const noexcept
{
	address_any result;
	address_link link = result;
	process_error(linapi::calls::getpeername(native_.get(), link.first, &link.second) == 0, err);
	return result;
}

std::string linapi::net::socket::read()
{
	THROW_IF_ERR_PROXY0(read, "failed to read msg from pipe");
}
std::string linapi::net::socket::read(std::error_code& err)
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
		if (!err) return res;
		break;
	}
	return res;
}
void linapi::net::socket::write(std::string_view data)
{
	write(std::as_bytes(std::span< const char >(data.data(), data.size())));
}
std::size_t linapi::net::socket::write_once(std::string_view data)
{
	return write_once(std::as_bytes(std::span< const char >(data.data(), data.size())));;
}
std::size_t linapi::net::socket::write(std::string_view data, std::error_code& err) noexcept
{
	return write(std::as_bytes(std::span< const char >(data.data(), data.size())), err);
}
std::size_t linapi::net::socket::write_once(std::string_view data, std::error_code& err) noexcept
{
	return write_once(std::as_bytes(std::span< const char >(data.data(), data.size())), err);;
}
void linapi::net::socket::write(std::span< const std::byte > data)
{
	THROW_IF_ERR(write, "failed to write to pipe", data);
}
std::size_t linapi::net::socket::write_once(std::span< const std::byte > data)
{
	THROW_IF_ERR_PROXY(write_once, "failed to write to pipe", data);
}
std::size_t linapi::net::socket::read(std::span< std::byte > data)
{
	THROW_IF_ERR_PROXY(read, "failed to read from pipe", data);
}
std::size_t linapi::net::socket::read_once(std::span< std::byte > data)
{
	THROW_IF_ERR_PROXY(read_once, "failed to read from pipe", data);
}

std::size_t linapi::net::socket::write(std::span< const std::byte > data, std::error_code& err) noexcept
{
	std::size_t offset = 0;
	while (offset < data.size())
	{
		offset += write_once(data.subspan(offset), err);
		if (err == std::errc::broken_pipe) return offset;
	}
	return offset;
}
std::size_t linapi::net::socket::write_once(std::span< const std::byte > data, std::error_code& err) noexcept
{
	constexpr int max = std::numeric_limits< int >::max();

	const int size = (data.size() < max ? static_cast< int >(data.size()) : max);
	assert(native_);
	int written = linapi::calls::send(native_.get(), reinterpret_cast< const char* >(data.data()), size, MSG_NOSIGNAL);
	process_error(written < 0, err);
	return static_cast< std::size_t >(written);
}
std::size_t linapi::net::socket::read(std::span< std::byte > data, std::error_code& err) noexcept
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
std::size_t linapi::net::socket::read_once(std::span< std::byte > data, std::error_code& err) noexcept
{
	constexpr int max = std::numeric_limits< int >::max();

	const int size = (data.size() < max ? static_cast< int >(data.size()) : max);
	assert(native_);
	int read_count = linapi::calls::recv(native_.get(), reinterpret_cast< char* >(data.data()), size, 0);
	process_error(read_count < 0, err);
	return static_cast< std::size_t >(read_count);
}
#endif
