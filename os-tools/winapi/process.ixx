module;
	#ifdef _WIN32
		#define WIN32_LEAN_AND_MEAN
		#include <windows.h>
	#endif
export module os.winapi:process;

#ifdef _WIN32
import std;
import :aliases;
import :handle;

export namespace winapi
{
	struct call_self_t
	{};
	constexpr inline call_self_t call_self;
	std::string prepare_cmd_args(const char* app, std::span< const char*const > args);
	std::string prepare_cmd_args(const std::string& app, std::span< const char*const > args);
	std::string prepare_cmd_args(const std::string& app, std::span< const std::string > args);
	std::string join_args(std::span< const std::string > data);
	std::string join_args(std::span< const char*const > data);
	std::string screen(const std::string& arg);
	std::string screen(const char* arg);
	
	class process
	{
	public:
		enum class priority: dword_t;
		using id = dword_t;
		using native_handle_type = handle_t;
		static constexpr int still_active = STILL_ACTIVE;

		process() noexcept = default;
		process(process&& rhs) noexcept;
		explicit process(unique_handle&& h) noexcept;
		explicit process(native_handle_type h) noexcept;

		explicit process(call_self_t);
		explicit process(call_self_t, std::span< const std::string > args);
		explicit process(call_self_t, std::span< const char*const > args);
		explicit process(call_self_t, std::string args); // raw args should include app name

		explicit process(const std::string& app, std::span< const std::string > args);
		explicit process(const char* app, std::span< const std::string > args);
		explicit process(const char* app, std::span< const char*const > args);

		explicit process(const std::string& app, std::string args);
		explicit process(std::span< const char*const > cl_cmd);
		explicit process(std::string cl_cmd);

		~process();
		process& operator=(process&& rhs) noexcept;

		native_handle_type native() const noexcept;
		native_handle_type native_handle() const noexcept;
		native_handle_type release() noexcept;
		void swap(process& rhs) noexcept;

		id get_id() const;
		int exit_code() const;

		bool joinable() const noexcept;
		bool alive() const; // wait for no time
		int wait() const;
		bool wait(std::chrono::milliseconds dur) const;
		int join();
		bool join(std::chrono::milliseconds dur);
		void detach();

		priority get_priority() const;
		void set_priority(priority p) const;

		void kill();
		void kill(int exit_code);

	private:
		unique_handle native_;
		static unique_handle create(const char* app, char* console);
	};
	namespace this_process
	{
		process::native_handle_type native();
		process::native_handle_type native_handle();
		process::id get_id();
		tstring executable();
		[[noreturn]] void exit(int code);
		process::priority get_priority();
		void set_priority(process::priority p);
	}
}
namespace std
{
	export void swap(winapi::process& lhs, winapi::process& rhs) noexcept;
}

enum class winapi::process::priority: winapi::dword_t
{
	idle = IDLE_PRIORITY_CLASS,
	lower = BELOW_NORMAL_PRIORITY_CLASS,
	normal = NORMAL_PRIORITY_CLASS,
	greater = ABOVE_NORMAL_PRIORITY_CLASS,
	high = HIGH_PRIORITY_CLASS,
	realtime = REALTIME_PRIORITY_CLASS,
};

#endif
