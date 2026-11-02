module;
	#ifdef __linux__
		#include <signal.h>
	#endif
export module os.linapi:signal;

#ifdef __linux__
import :aliases;

export namespace linapi::ipc::signal
{
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

	void send(pid_t process_group_id, type t);
	bool send(pid_t process_group_id, type t, std::error_code& err) noexcept;
}
#endif
