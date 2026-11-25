module;
	#ifdef __linux__
		#include <cassert>
		#include <sys/socket.h>
		#include <netinet/in.h>
	#endif
export module os.linapi:net_aliases;

#ifdef __linux__
import std;
import os.utils;
import :aliases;

export namespace linapi
{
	using socket_t = handle_t;
	using ::socklen_t;
	using socket_address_t = sockaddr;
	constexpr inline socket_t empty_socket = empty_handle;
	namespace calls
	{
		// Socket
		using ::socket;
		using ::shutdown;
		using ::getsockname;
		using ::getpeername;
		using ::bind;
		using ::listen;
		using ::accept;
		using ::connect;
		using ::send;
		using ::recv;
	}
	namespace net
	{
		constexpr inline std::endian endian = std::endian::big;
		enum class protocol
		{
			tcp = 0,
			udp = 0
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

		using address_link = std::pair< socket_address_t*, socklen_t >;
		using const_address_link = std::pair< const socket_address_t*, socklen_t >;
		struct address_any;
		class address_ipv4;
		class address_ipv6;

		template<>
		struct address_traits< address_ipv4 >;
		template<>
		struct address_traits< address_ipv6 >;

		constexpr address_type type_of(const_address_link addr);
	}
}

class linapi::net::lib_anchor
{
public:
	lib_anchor() = default;
	lib_anchor(lib_anchor&&) = delete;
	lib_anchor(const lib_anchor&) = delete;
	lib_anchor& operator=(lib_anchor&&) = delete;
	lib_anchor& operator=(const lib_anchor&) = delete;
};
struct linapi::net::address_any
{
	using native_type = sockaddr_storage;
	native_type native;

	template< class Typed >
	Typed as() const;

	operator address_link() & noexcept;
	operator const_address_link() const& noexcept;
};
class linapi::net::address_ipv4
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
class linapi::net::address_ipv6
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

constexpr linapi::net::address_type linapi::net::type_of(const_address_link addr)
{
	return static_cast< address_type >(addr.first->sa_family);
}

template< class Typed >
Typed linapi::net::address_any::as() const
{
	if constexpr (requires(){Typed::type;})
	{
		assert(static_cast< address_type >(native.ss_family) == Typed::type);
	}
	return Typed{*reinterpret_cast< const Typed::native_type* >(&native)};
}

constexpr linapi::net::address_ipv4::address_ipv4(native_type addr) noexcept:
	native_(addr)
{}
constexpr linapi::net::address_ipv4::address_ipv4(std::uint8_t net, std::uint8_t host,
			std::uint8_t localhost, std::uint8_t imp, std::uint16_t port) noexcept
{
	native_.sin_port = os::endian_cast< net::endian >(port);
	std::array< std::byte, 4 > addr{static_cast< std::byte >(net), static_cast< std::byte >(host),
				static_cast< std::byte >(localhost), static_cast< std::byte >(imp)};
	native_.sin_addr.s_addr = std::bit_cast< std::uint32_t >(addr);
}
constexpr linapi::net::address_type linapi::net::address_ipv4::get_type() noexcept
{
	return static_cast< address_type >(native_.sin_family);
}
constexpr std::uint16_t linapi::net::address_ipv4::port() noexcept
{
	return os::endian_cast< std::endian::native, net::endian >(native_.sin_port);
}
constexpr linapi::net::address_ipv4::native_type& linapi::net::address_ipv4::native() noexcept
{
	return native_;
}
constexpr const linapi::net::address_ipv4::native_type& linapi::net::address_ipv4::native() const noexcept
{
	return native_;
}

constexpr linapi::net::address_ipv6::address_ipv6(native_type addr) noexcept:
	native_(addr)
{}
constexpr linapi::net::address_ipv6::address_ipv6(std::uint16_t p1, std::uint16_t p2,
				std::uint16_t p3, std::uint16_t p4, std::uint16_t p5, std::uint16_t p6,
				std::uint16_t p7, std::uint16_t p8, std::uint16_t port) noexcept
{
	native_.sin6_port = os::endian_cast< net::endian >(port);
	native_.sin6_addr.s6_addr16[0] = os::endian_cast< net::endian >(p1);
	native_.sin6_addr.s6_addr16[1] = os::endian_cast< net::endian >(p2);
	native_.sin6_addr.s6_addr16[2] = os::endian_cast< net::endian >(p3);
	native_.sin6_addr.s6_addr16[3] = os::endian_cast< net::endian >(p4);
	native_.sin6_addr.s6_addr16[4] = os::endian_cast< net::endian >(p5);
	native_.sin6_addr.s6_addr16[5] = os::endian_cast< net::endian >(p6);
	native_.sin6_addr.s6_addr16[6] = os::endian_cast< net::endian >(p7);
	native_.sin6_addr.s6_addr16[7] = os::endian_cast< net::endian >(p8);
}
constexpr linapi::net::address_type linapi::net::address_ipv6::get_type() noexcept
{
	return static_cast< address_type >(native_.sin6_family);
}
constexpr std::uint16_t linapi::net::address_ipv6::port() noexcept
{
	return os::endian_cast< std::endian::native, net::endian >(native_.sin6_port);
}
constexpr linapi::net::address_ipv6::native_type& linapi::net::address_ipv6::native() noexcept
{
	return native_;
}
constexpr const linapi::net::address_ipv6::native_type& linapi::net::address_ipv6::native() const noexcept
{
	return native_;
}

template<>
struct linapi::net::address_traits< linapi::net::address_ipv4 >
{
	using type = address_ipv4;
	constexpr static inline type any{};
	constexpr static inline type loopback{127, 0, 0, 1};
};
template<>
struct linapi::net::address_traits< linapi::net::address_ipv6 >
{
	using type = address_ipv6;
	constexpr static inline type any{};
	constexpr static inline type loopback{0, 0, 0, 0, 0, 0, 0, 1}; // [::1]
};
#endif
