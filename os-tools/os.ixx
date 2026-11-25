module;
	#include <os-tools/os_macro.hpp>
export module os;

export import os.argv_parser;

#ifdef _WIN32
	export import os.winapi;
#elifdef __linux__
	export import os.linapi;
#endif

export namespace os
{
	using __OS_NAMESPACE::unique_lock;
	using __OS_NAMESPACE::shared_lock;
	using __OS_NAMESPACE::path; ///< Alias for std::filesystem::path
	using __OS_NAMESPACE::char_t; ///< Preferable char type for calls
	using __OS_NAMESPACE::string; ///< Preferable string type for calls
	using __OS_NAMESPACE::string_view; ///< Preferable string view type for calls
	using __OS_NAMESPACE::to_string;
	using __OS_NAMESPACE::to_handle;

	using __OS_NAMESPACE::handle_t;
	using __OS_NAMESPACE::int_handle_t;
	using __OS_NAMESPACE::unique_handle;
	using __OS_NAMESPACE::set_inheritance;
	using __OS_NAMESPACE::get_inheritance;

	using __OS_NAMESPACE::make_empty_t;
	using __OS_NAMESPACE::make_empty;
	using __OS_NAMESPACE::openmode;
	using __OS_NAMESPACE::filemode;
	using __OS_NAMESPACE::inheritmode;
	using __OS_NAMESPACE::io;

	using __OS_NAMESPACE::throw_on_error;

	using __OS_NAMESPACE::is_blank;
	using __OS_NAMESPACE::empty_handle;
	using __OS_NAMESPACE::empty_socket;

	using __OS_NAMESPACE::thread;
	using __OS_NAMESPACE::process;

	using __OS_NAMESPACE::file_mapping;
	using __OS_NAMESPACE::mapped_memory;
	namespace this_thread
	{
		using __OS_NAMESPACE::this_thread::native_handle;
		using __OS_NAMESPACE::this_thread::get_id;
		using __OS_NAMESPACE::this_thread::get_process_id;
		using __OS_NAMESPACE::this_thread::get_priority;
		using __OS_NAMESPACE::this_thread::set_priority;
		//using __OS_NAMESPACE::this_thread::get_ideal_processor;
		//using __OS_NAMESPACE::this_thread::set_ideal_processor;
	}
	namespace this_process
	{
		using __OS_NAMESPACE::this_process::native_handle;
		using __OS_NAMESPACE::this_process::get_id;
		using __OS_NAMESPACE::this_process::get_user_id;
		using __OS_NAMESPACE::this_process::executable;
		using __OS_NAMESPACE::this_process::get_env;
		using __OS_NAMESPACE::this_process::set_env;
		using __OS_NAMESPACE::this_process::delete_env;
		using __OS_NAMESPACE::this_process::environment;
		using __OS_NAMESPACE::this_process::get_priority;
		using __OS_NAMESPACE::this_process::set_priority;
		using __OS_NAMESPACE::this_process::_exit;
		using __OS_NAMESPACE::this_process::stdin;
		using __OS_NAMESPACE::this_process::stdout;
		using __OS_NAMESPACE::this_process::stderr;
		using __OS_NAMESPACE::this_process::std_handle;
	}
	namespace ipc
	{
		namespace signal
		{
			using __OS_NAMESPACE::ipc::signal::type;
			using __OS_NAMESPACE::ipc::signal::interrupt;
			using __OS_NAMESPACE::ipc::signal::terminate;

			using __OS_NAMESPACE::ipc::signal::send;
			using __OS_NAMESPACE::ipc::signal::ignore;
			using __OS_NAMESPACE::ipc::signal::restore_default;
		}
		using __OS_NAMESPACE::ipc::pipe;
		using __OS_NAMESPACE::ipc::create_pipe;

		using __OS_NAMESPACE::ipc::named_pipe;
		using __OS_NAMESPACE::ipc::create_server_pipe;
		using __OS_NAMESPACE::ipc::open_client_pipe;

		using __OS_NAMESPACE::ipc::event;

		using __OS_NAMESPACE::ipc::create_shared_memory;
	}
	namespace net
	{
		using __OS_NAMESPACE::net::lib_anchor;

		using __OS_NAMESPACE::net::protocol;
		using __OS_NAMESPACE::net::address_type;
		using __OS_NAMESPACE::net::address_link;
		using __OS_NAMESPACE::net::const_address_link;
		using __OS_NAMESPACE::net::address_traits;
		using __OS_NAMESPACE::net::type_of;

		using __OS_NAMESPACE::net::address_any;
		using __OS_NAMESPACE::net::address_ipv4;
		using __OS_NAMESPACE::net::address_ipv6;
		using __OS_NAMESPACE::net::socket;
	}
	namespace numa
	{
		/*using __OS_NAMESPACE::numa::node;
		using __OS_NAMESPACE::numa::processor;
		using __OS_NAMESPACE::numa::cpu_set;
		using __OS_NAMESPACE::numa::cpu_set_info;*/
	}

	using __OS_NAMESPACE::mutex;
	using __OS_NAMESPACE::shared_mutex;
	using __OS_NAMESPACE::condition_variable;
}
