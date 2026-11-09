module;
	#ifdef _WIN32
		#include <os-tools/os_macro.hpp>
		#include <os-tools/winapi/deps.hpp>
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
winapi::net::socket::socket(address::type addr, protocol proto)
{
	THROW_IF_ERR_CTOR(socket, "failed to create socket", addr, proto);
}
winapi::net::socket::socket(address::type addr, protocol proto, std::error_code& err) noexcept:
	socket(addr, proto, inheritmode::none, err)
{}
winapi::net::socket::socket(address::type addr, protocol proto, inheritmode inherit)
{
	THROW_IF_ERR_CTOR(socket, "failed to create socket", addr, proto, inherit);
}
winapi::net::socket::socket(address::type addr, protocol proto, inheritmode inherit, std::error_code& err) noexcept:
	native_(winapi::calls::WSASocketA(static_cast< int >(addr),
				determine_socket_type(proto), static_cast< int >(proto), nullptr, 0, WSA_FLAG_NO_HANDLE_INHERIT))
{
	net::process_error(operator bool(), err);
}
winapi::net::socket& winapi::net::socket::operator=(socket&& rhs) noexcept
{
	native_ = rhs.release_handle();
	return *this;
}

void winapi::net::socket::bind(address addr) const
{
	THROW_IF_ERR(bind, "failed to bind socket", addr);
}
bool winapi::net::socket::bind(address addr, std::error_code& err) const noexcept
{
	return net::process_error(winapi::calls::bind(native_, &addr.native(), sizeof(socket_address_t)) == 0, err);
}
void winapi::net::socket::listen(int max_conns) const
{
	THROW_IF_ERR(listen, "failed to listen socket", max_conns);
}
bool winapi::net::socket::listen(int max_conns, std::error_code & err) const noexcept
{
	return net::process_error(winapi::calls::listen(native_, max_conns) == 0, err);
}
std::pair< winapi::net::socket, winapi::net::address > winapi::net::socket::accept() const
{
	THROW_IF_ERR_PROXY0(accept, "failed to accept socket connection");
}
std::pair< winapi::net::socket, winapi::net::address >
		winapi::net::socket::accept(std::error_code & err) const noexcept
{
	std::pair< socket, address > result;
	int len = sizeof(socket_address_t);
	result.first = socket{winapi::calls::accept(native_, &result.second.native(), &len)};
	net::process_error(result.first, err);
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
#endif
