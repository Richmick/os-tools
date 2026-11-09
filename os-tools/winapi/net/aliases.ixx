module;
	#include <os-tools/winapi/deps.hpp>
export module os.winapi:net_aliases;

#ifdef _WIN32
import std;

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
		using ::bind;
		using ::listen;
		using ::accept;
		using ::send;
		using ::recv;

		// Other
		using ::WSAGetLastError;
		using ::WSAStartup;
		using ::WSACleanup;
	}
	namespace net
	{
		enum class protocol
		{
			tcp = IPPROTO_TCP,
			udp = IPPROTO_UDP
		};

		class lib_anchor;
		class address;

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
class winapi::net::address
{
public:
	enum class type: std::uint16_t
	{
		unspecified = AF_UNSPEC, ///< Raw (!= auto)
		ipv4 = AF_INET,
		ipv6 = AF_INET6
	};

	address() noexcept;
	address(sockaddr addr) noexcept;
	address(std::uint8_t p1, std::uint8_t p2, std::uint8_t p3, std::uint8_t p4, std::uint16_t port) noexcept;

	type get_type() noexcept;
	std::uint16_t port() noexcept;
	socket_address_t& native() noexcept;
	const socket_address_t& native() const noexcept;

private:
	socket_address_t native_;
};
#endif
