module;
	#include <os-tools/winapi/deps.hpp>
export module os.winapi:socket;

#ifdef _WIN32
import std;
import :aliases;
import :net_aliases;

export namespace winapi::net
{
	class socket;
}
class winapi::net::socket
{
public:
	using native_handle_type = socket_t;
	static constexpr inline int max_connections = SOMAXCONN;

	socket() noexcept = default;
	socket(native_handle_type h) noexcept;
	socket(socket&& rhs) noexcept;
	socket(address::type addr, protocol proto);
	socket(address::type addr, protocol proto, std::error_code& err) noexcept;
	socket(address::type addr, protocol proto, inheritmode inherit);
	socket(address::type addr, protocol proto, inheritmode inherit, std::error_code& err) noexcept;
	socket& operator=(socket&& rhs) noexcept;

	/// Bind address
	void bind(address addr) const;
	bool bind(address addr, std::error_code& err) const noexcept;
	/// Set mode = listening
	void listen(int max_conns = max_connections) const;
	bool listen(int max_conns, std::error_code& err) const noexcept;
	/// Wait for client
	std::pair< socket, address > accept() const;
	std::pair< socket, address > accept(std::error_code& err) const noexcept;

	native_handle_type native_handle() const noexcept;
	native_handle_type release_handle() noexcept;
	void close_handle() noexcept;
	operator bool() noexcept;

	void enable_handle_inheritance() const;
	bool enable_handle_inheritance(std::error_code& err) const noexcept;
	void disable_handle_inheritance() const;
	bool disable_handle_inheritance(std::error_code& err) const noexcept;
	inheritmode handle_inheritance() const;
	inheritmode handle_inheritance(std::error_code& err) const noexcept;

private:
	native_handle_type native_ = empty_socket;

	static constexpr int determine_socket_type(protocol proto) noexcept;
};

constexpr int winapi::net::socket::determine_socket_type(protocol proto) noexcept
{
	switch (proto)
	{
	case protocol::tcp:
		return SOCK_STREAM;
	case protocol::udp:
		return SOCK_DGRAM;
	default:
		return SOCK_RAW;
	}
}
#endif
