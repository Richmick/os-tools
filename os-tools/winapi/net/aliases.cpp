module;
	#ifdef _WIN32
		#include <os-tools/os_macro.hpp>
		#include <winsock2.h>
		#include <ws2tcpip.h>
	#endif
module os.winapi;

#ifdef _WIN32
import std;

std::error_code winapi::net::last_error_code() noexcept
{
	return std::error_code{static_cast< int >(winapi::calls::WSAGetLastError()), std::system_category()};
}
std::system_error winapi::net::last_error(const char* msg)
{
	return {net::last_error_code(), msg};
}
std::system_error winapi::net::last_error(const std::string& msg)
{
	return {net::last_error_code(), msg};
}
bool winapi::net::process_error(bool success, std::error_code& err) noexcept
{
	if (success) err.clear();
	else err = net::last_error_code();
	return success;
}

winapi::net::lib_anchor::lib_anchor(std::uint8_t max_version_major, std::uint8_t max_version_minor)
{
	int err = winapi::calls::WSAStartup(max_version_major
				| static_cast< std::uint16_t >(max_version_minor) << 8, &info_);
	if (err != 0) throw std::system_error{err, std::system_category(), "failed to initialize winsockets lib"};
}
winapi::net::lib_anchor::lib_anchor(std::uint8_t max_version_major, std::uint8_t max_version_minor,
			std::error_code& err) noexcept
{
	err = std::error_code{winapi::calls::WSAStartup(max_version_major
				| static_cast< std::uint16_t >(max_version_minor) << 8, &info_), std::system_category()};
}
winapi::net::lib_anchor::~lib_anchor()
{
	[[maybe_unused]] int err = winapi::calls::WSACleanup();
}

winapi::net::address::address() noexcept:
	native_{.sa_family = 0}
{}
winapi::net::address::address(sockaddr addr) noexcept:
	native_(addr)
{}
winapi::net::address::address(std::uint8_t p1, std::uint8_t p2,
			std::uint8_t p3, std::uint8_t p4, std::uint16_t port) noexcept
{
	native_.sa_family = static_cast< std::uint16_t >(type::ipv4);
	sockaddr_in& typed = *reinterpret_cast< sockaddr_in* >(&native_);
	typed.sin_port = port;
	typed.sin_addr.S_un.S_un_b.s_b1 = p1;
	typed.sin_addr.S_un.S_un_b.s_b2 = p2;
	typed.sin_addr.S_un.S_un_b.s_b3 = p3;
	typed.sin_addr.S_un.S_un_b.s_b4 = p4;
}
winapi::net::address::type winapi::net::address::get_type() noexcept
{
	return static_cast< type >(native_.sa_family);
}
std::uint16_t winapi::net::address::port() noexcept
{
	switch (static_cast< type >(native_.sa_family))
	{
	case type::ipv4:
		return reinterpret_cast< sockaddr_in* >(&native_)->sin_port;
	case type::ipv6:
		return reinterpret_cast< sockaddr_in6* >(&native_)->sin6_port;
	default:
		return 0;
	}
}
winapi::socket_address_t& winapi::net::address::native() noexcept
{
	return native_;
}
const winapi::socket_address_t& winapi::net::address::native() const noexcept
{
	return native_;
}
#endif
