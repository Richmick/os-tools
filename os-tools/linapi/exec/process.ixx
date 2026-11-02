module;
	#ifdef __linux__
		#include <unistd.h>
		#include <sys/wait.h>
	#endif
export module os.linapi:process;

#ifdef __linux__
import std;
import :aliases;
import :signal;

export namespace linapi
{
	using ready_args_t = std::pair< std::vector< std::string >, std::vector< char* > >;
	ready_args_t prepare_cmd_args(const char* app, std::span< const char*const > args);
	ready_args_t prepare_cmd_args(const path& app, std::span< const char*const > args);
	ready_args_t prepare_cmd_args(const path& app, std::span< const std::string > args);
	
	class process
	{
	public:
		/// Priorities for processes
		using priority = linapi::priority;
		/// Set of parameters for precise startup
		struct initial_params;

		/// Process group id
		using group_id = process_handle_t;
		/// Id type
		using id = process_handle_t;
		/// Os handle type (pid_t)
		using native_handle_type = process_handle_t;
		/// Exit code, meaning that process is still alive (can overlap with user codes)
		static constexpr int still_active = -2;

		process() noexcept = default;
		process(process&& rhs) noexcept;
		explicit process(native_handle_type h) noexcept;
		
		explicit process(const path& app);
		explicit process(const path& app, std::span< const string > args);
		explicit process(const path& app, std::span< const char_t* const > args);
		explicit process(const char_t* app);
		explicit process(const char_t* app, std::span< const string > args);
		explicit process(const char_t* app, std::span< const char_t* const > args);

		explicit process(initial_params& params, const path& app);
		explicit process(initial_params& params, const path& app, std::span< const string > args);
		explicit process(initial_params& params, const path& app, std::span< const char_t* const > args);
		explicit process(initial_params& params, const char_t* app);
		explicit process(initial_params& params, const char_t* app, std::span< const string > args);
		explicit process(initial_params& params, const char_t* app, std::span< const char_t* const > args);

		explicit process(const path& app, std::error_code& err);
		explicit process(const path& app, std::span< const string > args, std::error_code& err);
		explicit process(const path& app, std::span< const char_t* const > args, std::error_code& err);
		explicit process(const char_t* app, std::error_code& err);
		explicit process(const char_t* app, std::span< const string > args, std::error_code& err);
		explicit process(const char_t* app, std::span< const char_t* const > args, std::error_code& err);

		explicit process(initial_params& params, const path& app, std::error_code& err);
		explicit process(initial_params& params, const path& app, std::span< const string > args,
					std::error_code& err);
		explicit process(initial_params& params, const path& app, std::span< const char_t* const > args,
					std::error_code& err);
		explicit process(initial_params& params, const char_t* app, std::error_code& err);
		explicit process(initial_params& params, const char_t* app, std::span< const string > args,
					std::error_code& err);
		explicit process(initial_params& params, const char_t* app, std::span< const char_t* const > args,
					std::error_code& err);

		// Linapi specific (executable can be obtained by executable_aw< char >()):
		explicit process(const char* app, char*const* args);
		explicit process(const char* app, char*const* args, std::error_code& err);
		explicit process(initial_params& params, const char* app, char*const* args);
		explicit process(initial_params& params, const char* app, char*const* args, std::error_code& err);

		~process();
		process& operator=(process&& rhs) noexcept;

		native_handle_type native_handle() const noexcept;
		native_handle_type release_handle();
		void swap(process& rhs) noexcept;

		id get_id() const;
		group_id get_group_id() const;
		int exit_code() const;

		bool joinable() const noexcept;
		bool alive() const; // wait for no time
		int wait() const;
		int join();

		priority get_priority() const;
		void set_priority(priority p) const;

		void signal(ipc::signal::type type);
		void terminate(); // send signal terminate request
		void kill(); // no chance to process kill, still joinable

	private:
		static constexpr idtype_t pid_entity = P_PID;
		static constexpr int wait_immediate = WNOHANG;
		static constexpr int wait_only = WNOWAIT;
		static constexpr int wait_for_exit = WEXITED;

		native_handle_type native_{};

		process(ready_args_t argv);
		process(ready_args_t argv, std::error_code& err);
		process(initial_params& params, ready_args_t argv);
		process(initial_params& params, ready_args_t argv, std::error_code& err);
		static native_handle_type create(const char* app, char*const* console, std::error_code& err);
		static native_handle_type create(initial_params& params, const char* app, char*const* console, std::error_code& err);
	};
	namespace this_process
	{
		process::native_handle_type native_handle();
		process::id get_id();
		process::group_id get_group_id();
		process::native_handle_type parent_native_handle();
		process::id get_parent_id();
		path executable();
		std::optional< std::string > get_env(const std::string& name);
		void set_env(const std::string& name, const std::string& value);
		void delete_env(const std::string& name);
		std::unordered_map< std::string, std::string > environment();
		process::priority get_priority();
		void set_priority(process::priority p);
		void signal(ipc::signal::type type = ipc::signal::interrupt);

		[[noreturn]] void _exit(int code);

		handle_t stdin() noexcept;
		handle_t stdout() noexcept;
		handle_t stderr() noexcept;
		handle_t stdin(std::error_code& err) noexcept;
		handle_t stdout(std::error_code& err) noexcept;
		handle_t stderr(std::error_code& err) noexcept;
		handle_t std_handle(io type) noexcept;
		handle_t std_handle(io type, std::error_code& err) noexcept;
	}
}
namespace std
{
	export void swap(linapi::process& lhs, linapi::process& rhs) noexcept;
}

struct linapi::process::initial_params
{
	inheritmode inherit_handles = inheritmode::none;
	bool create_group = false;
	bool detached = false;
	process::priority priority = process::priority::unset;

	handle_t stdin = empty_handle;
	handle_t stdout = empty_handle;
	handle_t stderr = empty_handle;
};

#endif
