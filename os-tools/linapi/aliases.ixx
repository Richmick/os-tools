module;
	#ifdef __linux__
		#include <cstdlib>
		#include <unistd.h>
		#include <pthread.h>
		#include <spawn.h>
		#include <signal.h>
		#include <sys/wait.h>
		#include <sys/eventfd.h>
		#include <sys/resource.h>
		#include <fcntl.h>

		#include <filesystem>
		#include <cstdint>
		#include <mutex>
		#include <shared_mutex>
		#include <string>
		#include <string_view>
		#include <system_error>
	#endif
export module os.linapi:aliases;

#ifdef __linux__
export namespace linapi
{
	using std::unique_lock;
	using std::shared_lock;
	using std::filesystem::path;

	enum class inheritmode: bool
	{
		none = false,
		all = true
	};
	enum class openmode: std::uint8_t
	{
		closed, in, out, inout
	};
	struct make_empty_t
	{};
	constexpr inline make_empty_t make_empty;
	struct open_by_name_t
	{};
	constexpr inline open_by_name_t open_by_name;

	class unique_handle;
	class thread;
	class process;

	using handle_t = int;
	using int_handle_t = int;
	using ::idtype_t;
	using ::siginfo_t;
	using sigaction_t = struct ::sigaction;
	using rlimit_t = struct ::rlimit;
	using scheduler_settings_t = sched_param;
	using thread_handle_t = pthread_t;
	using process_handle_t = pid_t;
	using thread_attributes_t = pthread_attr_t;
	constexpr inline handle_t empty_handle = -1;

	enum class io
	{
		in = STDIN_FILENO,
		out = STDOUT_FILENO,
		err = STDERR_FILENO
	};

	using char_t = char; ///< Preferable char type for calls
	using string = std::string; ///< Preferable string type for calls
	using string_view = std::string_view; ///< Preferable string type for calls
	string to_string(handle_t h);
	handle_t to_handle(const string& h);
	handle_t to_handle(const string_view& h);

	using ::environ;

	namespace calls
	{
		using ::readlink;
		// Handles
		using ::read;
		using ::write;
		using ::close;
		using ::dup;
		using ::dup2;
		using ::dup3;
		using ::fcntl;
		using ::getrlimit;
		//using ::closefrom;

		// Process
		using ::getpid;
		using ::setsid;
		using ::getppid;
		using ::getpgid;
		using ::getenv;
		using ::setenv;
		using ::unsetenv;
		using ::posix_spawn;
		using ::fork;
		using ::vfork;
		using ::_exit;
		using ::execl; // (const char* path, const char* arg0, ... /*, (char *)0 */);
		using ::execle; // (const char* path, const char* arg0, ... /*,(char *)0, char *const envp[]*/);
		using ::execlp; // (const char* file, const char* arg0, ... /*, (char *)0 */);
		using ::execv; // (const char* path, char* const argv[]);
		using ::execve; // (const char* path, char* const argv[], char* const envp[]);
		using ::execvp; // (const char* file, char* const argv[]);
		using ::fexecve; // (int fd, char* const argv[], char* const envp[]);
		using ::waitid;
		using ::waitpid;
		using ::sched_getscheduler;
		using ::sched_setscheduler;

		// Thread
		using ::pthread_create;
		using ::pthread_exit;
		using ::pthread_join;
		using ::pthread_detach;
		using ::pthread_kill;
		using ::pthread_self;
		using ::pthread_setschedprio;
		using ::pthread_setschedparam;
		using ::pthread_getschedparam;
		using ::pthread_attr_init;
		using ::pthread_attr_destroy;
		using ::pthread_attr_setstacksize;
		using ::pthread_attr_setschedpolicy;
		using ::pthread_attr_setinheritsched;

		// Pipes
		using ::pipe;
		using ::pipe2;

		// Events
		using ::eventfd;

		// Signals
		using ::sigaction;
		using ::kill;
		using ::killpg;
	}

	std::error_code last_error_code();
	std::system_error last_error(const char* msg);
	std::system_error last_error(const std::string& msg);
	void throw_on_error(std::error_code err, const char* msg);
	void throw_on_error(std::error_code err, const std::string& msg);
	void process_error(bool success, std::error_code& err) noexcept;
	handle_t process_error(handle_t res, std::error_code& err) noexcept;

	enum class priority
	{
		idle = SCHED_IDLE,
		normal = SCHED_OTHER,
		high = SCHED_BATCH,
		realtime = SCHED_FIFO,
		greater_realtime = SCHED_RR,
		unset = -1
	};
}

#endif
