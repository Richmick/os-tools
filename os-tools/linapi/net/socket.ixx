module;
	#ifdef __linux__
		#include <sys/socket.h>
	#endif
export module os.linapi:socket;

#ifdef __linux__
import std;
import :aliases;
import :handle;
import :net_aliases;

export namespace linapi::net
{
	class socket;
}
class linapi::net::socket: public handle_container_base
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

	using handle_container_base::handle_container_base;
	socket(net::address_type addr, type t, protocol proto);
	socket(net::address_type addr, type t, protocol proto, std::error_code& err) noexcept;
	socket(net::address_type addr, type t, protocol proto, inheritmode inherit);
	socket(net::address_type addr, type t, protocol proto, inheritmode inherit, std::error_code& err) noexcept;

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
};
#endif
