export module os.linapi:pipe;

#ifdef __linux__
import std;
import :aliases;
import :handle;

export namespace linapi::ipc
{
	class pipe: public handle_container_base
	{
	public:
		struct initial_params;

		using handle_container_base::handle_container_base;

		static void prevent_signal_emiting();
		static bool prevent_signal_emiting(std::error_code& err) noexcept;

		void set_traits(bool packet, bool nonblock);
		bool packet();
		void set_packet(bool packet);
		bool nonblock();
		void set_nonblock(bool nonblock);

		bool set_traits(bool packet, bool nonblock, std::error_code& err) noexcept;
		bool packet(std::error_code& err) noexcept;
		bool set_packet(bool packet, std::error_code& err) noexcept;
		bool nonblock(std::error_code& err) noexcept;
		bool set_nonblock(bool nonblock, std::error_code& err) noexcept;

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

	std::pair< pipe, pipe > create_pipe();
	std::pair< pipe, pipe > create_pipe(std::error_code& err) noexcept;
	std::pair< pipe, pipe > create_pipe(inheritmode inherit);
	std::pair< pipe, pipe > create_pipe(inheritmode inherit, std::error_code& err) noexcept;
	std::pair< pipe, pipe > create_pipe(const pipe::initial_params& params);
	std::pair< pipe, pipe > create_pipe(const pipe::initial_params& params, std::error_code& err) noexcept;

	class named_pipe: public pipe
	{
	public:
		struct initial_params;
		struct open_params;

		using pipe::pipe; // handle_container_base constructors

		void flush();
		bool flush(std::error_code& err) noexcept;
		void connect();
		bool connect(std::error_code& err) noexcept;
	};

	named_pipe create_server_pipe(string name, openmode mode);
	named_pipe create_server_pipe(string name, openmode mode, std::error_code& err) noexcept;
	named_pipe create_server_pipe(string name, openmode mode, inheritmode inherit);
	named_pipe create_server_pipe(string name, openmode mode, inheritmode inherit,
		std::error_code& err) noexcept;
	named_pipe create_server_pipe(const named_pipe::initial_params& params, string name);
	named_pipe create_server_pipe(const named_pipe::initial_params& params, string name,
		std::error_code& err) noexcept;

	named_pipe open_client_pipe(string name, openmode mode);
	named_pipe open_client_pipe(string name, openmode mode, std::error_code& err) noexcept;
	named_pipe open_client_pipe(string name, openmode mode, inheritmode inherit);
	named_pipe open_client_pipe(string name, openmode mode, inheritmode inherit,
		std::error_code& err) noexcept;
	named_pipe open_client_pipe(const named_pipe::open_params& params, string name);
	named_pipe open_client_pipe(const named_pipe::open_params& params, string name,
		std::error_code& err) noexcept;
}

struct linapi::ipc::pipe::initial_params
{
	inheritmode inherit = inheritmode::none;
	bool nonblock = false;
	bool packet = false;
	[[maybe_unused]] std::size_t size_hint = 0;
};
struct linapi::ipc::named_pipe::initial_params
{
	openmode mode = openmode::inout;
	inheritmode inherit = inheritmode::none;
	bool nonblock = false;
	bool packet = false;
	bool prevent_open_existing = true;
	[[maybe_unused]] std::size_t read_size_hint = 0;
	[[maybe_unused]] std::size_t write_size_hint = 0;
};
struct linapi::ipc::named_pipe::open_params
{
	openmode mode = openmode::inout;
	inheritmode inherit = inheritmode::none;
	bool nonblock = false;
	bool packet = false;
};
#endif
