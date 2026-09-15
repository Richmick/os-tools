module;
	#ifdef __linux__
		#include <unistd.h>
		#include <sys/wait.h>
	#endif
export module os.linapi:process;

#ifdef __linux__
import std;
import :aliases;

export namespace linapi
{
	struct call_self_t
	{};
	constexpr inline call_self_t call_self;

	using ready_args_t = std::pair< std::vector< std::string >, std::vector< char* > >;
	ready_args_t prepare_cmd_args(const char* app, std::span< const char*const > args);
	ready_args_t prepare_cmd_args(const std::string& app, std::span< const char*const > args);
	ready_args_t prepare_cmd_args(const std::string& app, std::span< const std::string > args);
	
	class process
	{
	public:
		using priority = linapi::priority;
		using native_handle_type = process_handle_t;
		using id = process_handle_t;
		static constexpr int still_active = -2;

		process() noexcept = default;
		process(process&& rhs) noexcept;
		explicit process(native_handle_type h) noexcept;

		explicit process(call_self_t);
		explicit process(call_self_t, std::span< const std::string > args);
		explicit process(call_self_t, std::span< const char*const > args);
		explicit process(call_self_t, char*const* args); // raw args should include app name

		explicit process(const std::string& app, std::span< const std::string > args);
		explicit process(const char* app, std::span< const std::string > args);
		explicit process(const char* app, std::span< const char*const > args);
		explicit process(const char* app, char*const* args);

		~process();
		process& operator=(process&& rhs) noexcept;

		native_handle_type native() const noexcept;
		native_handle_type native_handle() const noexcept;
		native_handle_type release();
		void swap(process& rhs) noexcept;

		id get_id() const;
		int exit_code() const;

		bool joinable() const noexcept;
		bool alive() const; // wait for no time
		int wait() const;
		int join();

		priority get_priority() const;
		void set_priority(priority p) const;

		void kill(); // still joinable

	private:
		static constexpr idtype_t pid_entity = P_PID;
		static constexpr int wait_immediate = WNOHANG;
		static constexpr int wait_only = WNOWAIT;
		static constexpr int wait_for_exit = WEXITED;
		native_handle_type native_;
		static native_handle_type create(const char* app, char*const* console);

		process(ready_args_t argv);
	};
	namespace this_process
	{
		process::native_handle_type native();
		process::native_handle_type native_handle();
		process::id get_id();
		process::native_handle_type parent_native();
		process::native_handle_type parent_native_handle();
		process::id get_parent_id();
		std::string executable();
		[[noreturn]] void exit(int code);
		process::priority get_priority();
		void set_priority(process::priority p);
	}
}
namespace std
{
	export void swap(linapi::process& lhs, linapi::process& rhs) noexcept;
}

#endif
