module;
	#ifdef __linux__
		#include <cstdlib>
		#include <unistd.h>
		#include <pthread.h>
		#include <spawn.h>
		#include <signal.h>
		#include <sys/wait.h>
	#endif
export module os.linapi:aliases;

#ifdef __linux__
import std;

export namespace linapi
{
	class unique_handle;
	class thread;
	class process;

	using handle_t = int;
	using ::idtype_t;
	using ::siginfo_t;
	using scheduler_settings_t = sched_param;
	using thread_handle_t = pthread_t;
	using process_handle_t = pid_t;
	using thread_attributes_t = pthread_attr_t;

	using ::environ;

	namespace calls
	{
		using ::readlink;
		using ::close;

		// Process
		using ::getpid;
		using ::getppid;
		using ::getenv;
		using ::setenv;
		using ::unsetenv;
		using ::kill;
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
	}

	std::system_error last_error(const char* msg);
	std::system_error last_error(const std::string& msg);

	enum class priority
	{
		idle = SCHED_IDLE,
		normal = SCHED_OTHER,
		high = SCHED_BATCH,
		realtime = SCHED_FIFO,
		greater_realtime = SCHED_RR,
	};
	enum class signal
	{
		terminate = SIGTERM,
		kill = SIGKILL,
		stop = SIGSTOP,
		interrupt = SIGINT,
		broken_pipe = SIGPIPE,
		poll = SIGPOLL,
		child_exit = SIGCHLD
	};
}

#endif
