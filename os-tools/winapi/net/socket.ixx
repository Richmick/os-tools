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
	enum class type
	{
		stream = SOCK_STREAM,
		datagram = SOCK_DGRAM,
		raw = SOCK_RAW
	};
	using native_handle_type = socket_t;
	static constexpr inline int max_connections = SOMAXCONN;

	socket() noexcept = default;
	socket(native_handle_type h) noexcept;
	socket(socket&& rhs) noexcept;
	socket(net::address_type addr, type t, protocol proto);
	socket(net::address_type addr, type t, protocol proto, std::error_code& err) noexcept;
	socket(net::address_type addr, type t, protocol proto, inheritmode inherit);
	socket(net::address_type addr, type t, protocol proto, inheritmode inherit, std::error_code& err) noexcept;
	socket& operator=(socket&& rhs) noexcept;

	/// Bind address
	void bind(const_address_link addr) const;
	bool bind(const_address_link addr, std::error_code& err) const noexcept;
	/// Set mode = listening
	void listen(int max_conns = max_connections) const;
	bool listen(int max_conns, std::error_code& err) const noexcept;
	/// Wait for client
	socket accept() const;
	socket accept(std::error_code& err) const noexcept;
	/// Connect to remote by address
	void connect(const_address_link addr) const;
	bool connect(const_address_link addr, std::error_code& err) const noexcept;
	/// Gracefully finish connection
	void shutdown(bool close_read, bool close_write) const;
	bool shutdown(bool close_read, bool close_write, std::error_code& err) const noexcept;

	address_any address() const;
	address_any address(std::error_code& err) const noexcept;
	address_any peer_address() const;
	address_any peer_address(std::error_code& err) const noexcept;

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
	
	std::string read();
	std::size_t read(std::span< std::byte > data);
	std::size_t read_once(std::span< std::byte > data);
	void write(std::string_view data);
	void write(std::span< const std::byte > data);
	std::size_t write_once(std::string_view data);
	std::size_t write_once(std::span< const std::byte > data);

	std::string read(std::error_code& err);
	std::size_t read(std::span< std::byte > data, std::error_code& err) noexcept;
	std::size_t read_once(std::span< std::byte > data, std::error_code& err) noexcept;
	std::size_t write(std::string_view data, std::error_code& err) noexcept;
	std::size_t write(std::span< const std::byte > data, std::error_code& err) noexcept;
	std::size_t write_once(std::string_view data, std::error_code& err) noexcept;
	std::size_t write_once(std::span< const std::byte > data, std::error_code& err) noexcept;

private:
	native_handle_type native_ = empty_socket;
};
#endif
