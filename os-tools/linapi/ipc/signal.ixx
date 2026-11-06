module;
	#ifdef __linux__
		#include <system_error>
		#include <signal.h>
	#endif
export module os.linapi:signal;

#ifdef __linux__
import :aliases;

export namespace linapi::ipc::signal
{
	using callback = void(*)(int, siginfo_t*, void*);
	/// Signal type enum
	enum type
	{
		terminate = SIGTERM, ///< Request
		kill = SIGKILL, ///< Kill without request
		stop = SIGSTOP,
		interrupt = SIGINT, ///< Interrupt request
		broken_pipe = SIGPIPE,
		poll = SIGPOLL,
		child_exit = SIGCHLD
	};

	/// Add callback to process' list
	void register_callback(callback cb, type t);
	/// Add callback to process' list
	bool register_callback(callback cb, type t, std::error_code& err) noexcept;

	void send(pid_t process_group_id, type t);
	bool send(pid_t process_group_id, type t, std::error_code& err) noexcept;
}
#endif
