module;
	#ifdef _WIN32
		#include <os-tools/os_macro.hpp>
		#include <os-tools/winapi/deps.hpp>
		#include <cassert>
	#endif
module os.winapi;

#ifdef _WIN32
import std;

winapi::net::socket::socket(socket&& rhs) noexcept:
	native_(rhs.release_handle())
{}
winapi::net::socket::socket(native_handle_type h) noexcept:
	native_(h)
{}
winapi::net::socket::socket(address_type addr, type t, protocol proto)
{
	THROW_IF_ERR_CTOR(socket, "failed to create socket", addr, t, proto);
}
winapi::net::socket::socket(address_type addr, type t, protocol proto, std::error_code& err) noexcept:
	socket(addr, t, proto, inheritmode::none, err)
{}
winapi::net::socket::socket(address_type addr, type t, protocol proto, inheritmode inherit)
{
	THROW_IF_ERR_CTOR(socket, "failed to create socket", addr, t, proto, inherit);
}
winapi::net::socket::socket(address_type addr, type t, protocol proto, inheritmode inherit, std::error_code& err) noexcept:
	native_(winapi::calls::WSASocketA(static_cast< int >(addr), static_cast< int >(t),
				static_cast< int >(proto), nullptr, 0, WSA_FLAG_NO_HANDLE_INHERIT))
{
	net::process_error(operator bool(), err);
}
winapi::net::socket& winapi::net::socket::operator=(socket&& rhs) noexcept
{
	native_ = rhs.release_handle();
	return *this;
}

void winapi::net::socket::bind(const_address_link addr) const
{
	THROW_IF_ERR(bind, "failed to bind socket", addr);
}
bool winapi::net::socket::bind(const_address_link addr, std::error_code& err) const noexcept
{
	return net::process_error(winapi::calls::bind(native_, addr.first, addr.second) == 0, err);
}
void winapi::net::socket::listen(int max_conns) const
{
	THROW_IF_ERR(listen, "failed to listen socket", max_conns);
}
bool winapi::net::socket::listen(int max_conns, std::error_code & err) const noexcept
{
	return net::process_error(winapi::calls::listen(native_, max_conns) == 0, err);
}
winapi::net::socket winapi::net::socket::accept() const
{
	THROW_IF_ERR_PROXY0(accept, "failed to accept socket connection");
}
winapi::net::socket winapi::net::socket::accept(std::error_code& err) const noexcept
{
	socket result = socket{winapi::calls::accept(native_, nullptr, nullptr)};
	net::process_error(result, err);
	return result;
}
void winapi::net::socket::connect(const_address_link addr) const
{
	THROW_IF_ERR(connect, "failed to connect socket", addr);
}
bool winapi::net::socket::connect(const_address_link addr, std::error_code& err) const noexcept
{
	return net::process_error(winapi::calls::connect(native_, addr.first, addr.second) == 0, err);
}
void winapi::net::socket::shutdown(bool close_read, bool close_write) const
{
	THROW_IF_ERR(shutdown, "failed to shutdown socket operations", close_read, close_write);
}
bool winapi::net::socket::shutdown(bool close_read, bool close_write, std::error_code& err) const noexcept
{
	int mode = 0;
	if (close_read)
		if (close_write) mode = SD_BOTH;
		else mode = SD_RECEIVE;
	else if (close_write) mode = SD_SEND;
	else return true;

	return net::process_error(winapi::calls::shutdown(native_, mode) == 0, err);
}

winapi::net::address_any winapi::net::socket::address() const
{
	THROW_IF_ERR_PROXY0(address, "failed to obtain socket address (name)");
}
winapi::net::address_any winapi::net::socket::address(std::error_code& err) const noexcept
{
	address_any result;
	address_link link = result;
	net::process_error(winapi::calls::getsockname(native_, link.first, &link.second) == 0, err);
	return result;
}
winapi::net::address_any winapi::net::socket::peer_address() const
{
	THROW_IF_ERR_PROXY0(address, "failed to obtain socket peer address (name)");
}
winapi::net::address_any winapi::net::socket::peer_address(std::error_code& err) const noexcept
{
	address_any result;
	address_link link = result;
	net::process_error(winapi::calls::getpeername(native_, link.first, &link.second) == 0, err);
	return result;
}

winapi::socket_t winapi::net::socket::native_handle() const noexcept
{
	return native_;
}
winapi::socket_t winapi::net::socket::release_handle() noexcept
{
	return std::exchange(native_, empty_socket);
}
void winapi::net::socket::close_handle() noexcept
{
	if (operator bool())
	{
		winapi::calls::closesocket(release_handle());
	}
}
winapi::net::socket::operator bool() noexcept
{
	return native_ != empty_socket;
}

void winapi::net::socket::enable_handle_inheritance() const
{
	winapi::set_inheritance(inheritmode::all, reinterpret_cast< winapi::handle_t >(native_));
}
void winapi::net::socket::disable_handle_inheritance() const
{
	winapi::set_inheritance(inheritmode::none, reinterpret_cast< winapi::handle_t >(native_));
}
bool winapi::net::socket::enable_handle_inheritance(std::error_code& err) const noexcept
{
	return winapi::set_inheritance(inheritmode::all, reinterpret_cast< winapi::handle_t >(native_), err);
}
bool winapi::net::socket::disable_handle_inheritance(std::error_code& err) const noexcept
{
	return winapi::set_inheritance(inheritmode::none, reinterpret_cast< winapi::handle_t >(native_), err);
}
winapi::inheritmode winapi::net::socket::handle_inheritance() const
{
	return winapi::get_inheritance(reinterpret_cast< winapi::handle_t >(native_));
}
winapi::inheritmode winapi::net::socket::handle_inheritance(std::error_code& err) const noexcept
{
	return winapi::get_inheritance(reinterpret_cast< winapi::handle_t >(native_), err);
}

std::string winapi::net::socket::read()
{
	THROW_IF_ERR_PROXY0(read, "failed to read msg from pipe");
}
std::string winapi::net::socket::read(std::error_code& err)
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
void winapi::net::socket::write(std::string_view data)
{
	write(std::as_bytes(std::span< const char >(data.data(), data.size())));
}
std::size_t winapi::net::socket::write_once(std::string_view data)
{
	return write_once(std::as_bytes(std::span< const char >(data.data(), data.size())));;
}
std::size_t winapi::net::socket::write(std::string_view data, std::error_code& err) noexcept
{
	return write(std::as_bytes(std::span< const char >(data.data(), data.size())), err);
}
std::size_t winapi::net::socket::write_once(std::string_view data, std::error_code& err) noexcept
{
	return write_once(std::as_bytes(std::span< const char >(data.data(), data.size())), err);;
}
void winapi::net::socket::write(std::span< const std::byte > data)
{
	THROW_IF_ERR(write, "failed to write to pipe", data);
}
std::size_t winapi::net::socket::write_once(std::span< const std::byte > data)
{
	THROW_IF_ERR_PROXY(write_once, "failed to write to pipe", data);
}
std::size_t winapi::net::socket::read(std::span< std::byte > data)
{
	THROW_IF_ERR_PROXY(read, "failed to read from pipe", data);
}
std::size_t winapi::net::socket::read_once(std::span< std::byte > data)
{
	THROW_IF_ERR_PROXY(read_once, "failed to read from pipe", data);
}

std::size_t winapi::net::socket::write(std::span< const std::byte > data, std::error_code& err) noexcept
{
	std::size_t offset = 0;
	while (offset < data.size())
	{
		offset += write_once(data.subspan(offset), err);
		if (err == std::errc::broken_pipe) return offset;
	}
	return offset;
}
std::size_t winapi::net::socket::write_once(std::span< const std::byte > data, std::error_code& err) noexcept
{
	constexpr int max = std::numeric_limits< int >::max();

	const int size = (data.size() < max ? static_cast< int >(data.size()) : max);
	assert(native_ != empty_socket);
	int written = winapi::calls::send(native_, reinterpret_cast< const char* >(data.data()), size, 0);
	process_error(written < 0, err);
	return static_cast< std::size_t >(written);
}
std::size_t winapi::net::socket::read(std::span< std::byte > data, std::error_code& err) noexcept
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
std::size_t winapi::net::socket::read_once(std::span< std::byte > data, std::error_code& err) noexcept
{
	constexpr int max = std::numeric_limits< int >::max();

	const int size = (data.size() < max ? static_cast< int >(data.size()) : max);
	assert(native_ != empty_socket);
	int read_count = winapi::calls::recv(native_, reinterpret_cast< char* >(data.data()), size, 0);
	process_error(read_count < 0, err);
	return static_cast< std::size_t >(read_count);
}
#endif
