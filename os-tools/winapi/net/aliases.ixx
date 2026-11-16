module;
	#ifdef _WIN32
		#include <cassert>
		#include <os-tools/winapi/deps.hpp>
		#include <ws2tcpip.h>
	#endif
export module os.winapi:net_aliases;

#ifdef _WIN32
import std;
import os.utils;

export namespace winapi
{
	using socket_t = SOCKET;
	using socket_address_t = sockaddr;
	using socket_libinfo_t = WSADATA;
	constexpr inline socket_t empty_socket = INVALID_SOCKET;
	namespace calls
	{
		// Socket
		using ::socket;
		using ::WSASocketA;
		using ::WSASocketW;
		using ::closesocket;
		using ::shutdown;
		using ::getsockname;
		using ::getpeername;
		using ::bind;
		using ::listen;
		using ::accept;
		using ::connect;
		using ::send;
		using ::recv;

		// Other
		using ::WSAGetLastError;
		using ::WSAStartup;
		using ::WSACleanup;
	}
	namespace net
	{
		constexpr inline std::endian endian = std::endian::big;
		enum class protocol
		{
			tcp = IPPROTO_TCP,
			udp = IPPROTO_UDP
		};
		enum class address_type: std::uint16_t
		{
			unspecified = AF_UNSPEC, ///< Raw (!= auto)
			ipv4 = AF_INET,
			ipv6 = AF_INET6
		};

		class lib_anchor;

		template< class T >
		struct address_traits;

		using address_link = std::pair< socket_address_t*, int >;
		using const_address_link = std::pair< const socket_address_t*, int >;
		struct address_any;
		class address_ipv4;
		class address_ipv6;

		template<>
		struct address_traits< address_ipv4 >;
		template<>
		struct address_traits< address_ipv6 >;

		constexpr address_type type_of(const_address_link addr);

		std::error_code last_error_code() noexcept;
		std::system_error last_error(const char* msg);
		std::system_error last_error(const std::string& msg);
		bool process_error(bool success, std::error_code& err) noexcept;
	}
}

class winapi::net::lib_anchor
{
public:
	lib_anchor(std::uint8_t max_version_major = 2, std::uint8_t max_version_minor = 2);
	lib_anchor(std::uint8_t max_version_major, std::uint8_t max_version_minor,
		std::error_code& err) noexcept;
	lib_anchor(const lib_anchor&) = delete;
	lib_anchor& operator=(const lib_anchor&) = delete;
	~lib_anchor();

private:
	socket_libinfo_t info_;
};
struct winapi::net::address_any
{
	using native_type = sockaddr_storage;
	native_type native;

	template< class Typed >
	Typed as() const;

	operator address_link() & noexcept;
	operator const_address_link() const& noexcept;
};
class winapi::net::address_ipv4
{
public:
	using native_type = sockaddr_in;
	static constexpr inline address_type type = address_type::ipv4;

	constexpr address_ipv4() noexcept = default;
	constexpr address_ipv4(native_type addr) noexcept;
	/// Native byte order (no casts required)
	constexpr address_ipv4(std::uint8_t net, std::uint8_t host,
				std::uint8_t localhost, std::uint8_t imp, std::uint16_t port = 0) noexcept;

	constexpr address_type get_type() noexcept;
	constexpr std::uint16_t port() noexcept;
	constexpr native_type& native() noexcept;
	constexpr const native_type& native() const noexcept;

	/// Get address link
	operator address_link() & noexcept;
	/// Get address link
	operator const_address_link() const& noexcept;

private:
	native_type native_{.sin_family = static_cast< std::uint16_t >(address_type::ipv4)};
};
class winapi::net::address_ipv6
{
public:
	using native_type = sockaddr_in6;
	static constexpr inline address_type type = address_type::ipv6;

	constexpr address_ipv6() noexcept = default;
	constexpr address_ipv6(native_type addr) noexcept;
	/// Native byte order (no casts required)
	constexpr address_ipv6(std::uint16_t p1, std::uint16_t p2, std::uint16_t p3, std::uint16_t p4,
				std::uint16_t p5, std::uint16_t p6, std::uint16_t p7, std::uint16_t p8,
				std::uint16_t port = 0) noexcept;

	constexpr address_type get_type() noexcept;
	constexpr std::uint16_t port() noexcept;
	constexpr native_type& native() noexcept;
	constexpr const native_type& native() const noexcept;

	/// Get address link
	operator address_link() & noexcept;
	/// Get address link
	operator const_address_link() const& noexcept;

private:
	native_type native_{.sin6_family = static_cast< std::uint16_t >(address_type::ipv6)};
};

constexpr winapi::net::address_type winapi::net::type_of(const_address_link addr)
{
	return static_cast< address_type >(addr.first->sa_family);
}

template< class Typed >
Typed winapi::net::address_any::as() const
{
	if constexpr (requires(){Typed::type;})
	{
		assert(static_cast< address_type >(native.ss_family) == Typed::type);
	}
	return Typed{*reinterpret_cast< const Typed::native_type* >(&native)};
}

constexpr winapi::net::address_ipv4::address_ipv4(native_type addr) noexcept:
	native_(addr)
{}
constexpr winapi::net::address_ipv4::address_ipv4(std::uint8_t net, std::uint8_t host,
			std::uint8_t localhost, std::uint8_t imp, std::uint16_t port) noexcept
{
	native_.sin_port = os::endian_cast< net::endian >(port);
	native_.sin_addr.s_net = net;
	native_.sin_addr.s_host = host;
	native_.sin_addr.s_lh = localhost;
	native_.sin_addr.s_impno = imp;
}
constexpr winapi::net::address_type winapi::net::address_ipv4::get_type() noexcept
{
	return static_cast< address_type >(native_.sin_family);
}
constexpr std::uint16_t winapi::net::address_ipv4::port() noexcept
{
	return os::endian_cast< std::endian::native, net::endian >(native_.sin_port);
}
constexpr winapi::net::address_ipv4::native_type& winapi::net::address_ipv4::native() noexcept
{
	return native_;
}
constexpr const winapi::net::address_ipv4::native_type& winapi::net::address_ipv4::native() const noexcept
{
	return native_;
}

constexpr winapi::net::address_ipv6::address_ipv6(native_type addr) noexcept:
	native_(addr)
{}
constexpr winapi::net::address_ipv6::address_ipv6(std::uint16_t p1, std::uint16_t p2,
				std::uint16_t p3, std::uint16_t p4, std::uint16_t p5, std::uint16_t p6,
				std::uint16_t p7, std::uint16_t p8, std::uint16_t port) noexcept
{
	native_.sin6_port = os::endian_cast< net::endian >(port);
	native_.sin6_addr.u.Word[0] = os::endian_cast< net::endian >(p1);
	native_.sin6_addr.u.Word[1] = os::endian_cast< net::endian >(p2);
	native_.sin6_addr.u.Word[2] = os::endian_cast< net::endian >(p3);
	native_.sin6_addr.u.Word[3] = os::endian_cast< net::endian >(p4);
	native_.sin6_addr.u.Word[4] = os::endian_cast< net::endian >(p5);
	native_.sin6_addr.u.Word[5] = os::endian_cast< net::endian >(p6);
	native_.sin6_addr.u.Word[6] = os::endian_cast< net::endian >(p7);
	native_.sin6_addr.u.Word[7] = os::endian_cast< net::endian >(p8);
}
constexpr winapi::net::address_type winapi::net::address_ipv6::get_type() noexcept
{
	return static_cast< address_type >(native_.sin6_family);
}
constexpr std::uint16_t winapi::net::address_ipv6::port() noexcept
{
	return os::endian_cast< std::endian::native, net::endian >(native_.sin6_port);
}
constexpr winapi::net::address_ipv6::native_type& winapi::net::address_ipv6::native() noexcept
{
	return native_;
}
constexpr const winapi::net::address_ipv6::native_type& winapi::net::address_ipv6::native() const noexcept
{
	return native_;
}

template<>
struct winapi::net::address_traits< winapi::net::address_ipv4 >
{
	using type = address_ipv4;
	constexpr static inline type any{};
	constexpr static inline type loopback{127, 0, 0, 1};
};
template<>
struct winapi::net::address_traits< winapi::net::address_ipv6 >
{
	using type = address_ipv6;
	constexpr static inline type any{};
	constexpr static inline type loopback{0, 0, 0, 0, 0, 0, 0, 1}; // [::1]
};
#endif
