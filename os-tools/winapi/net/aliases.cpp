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

winapi::net::address_any::operator winapi::net::address_link() & noexcept
{
	sockaddr_in6 k;
	return {reinterpret_cast< socket_address_t* >(&native), sizeof(native_type)};
}
winapi::net::address_any::operator winapi::net::const_address_link() const& noexcept
{
	return {reinterpret_cast< const socket_address_t* >(&native), sizeof(native_type)};
}

winapi::net::address_ipv4::operator winapi::net::address_link() & noexcept
{
	return {reinterpret_cast< socket_address_t* >(&native_), sizeof(native_type)};
}
winapi::net::address_ipv4::operator winapi::net::const_address_link() const& noexcept
{
	return {reinterpret_cast< const socket_address_t* >(&native_), sizeof(native_type)};
}

winapi::net::address_ipv6::operator winapi::net::address_link() & noexcept
{
	return {reinterpret_cast< socket_address_t* >(&native_), sizeof(native_type)};
}
winapi::net::address_ipv6::operator winapi::net::const_address_link() const& noexcept
{
	return {reinterpret_cast< const socket_address_t* >(&native_), sizeof(native_type)};
}
#endif
