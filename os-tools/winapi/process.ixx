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
#endif

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

	class process;
	namespace this_process
	{
		handle_t native();
		int id();
		tstring executable();
		[[noreturn]] void exit(int code);
		winapi::priority priority();
		void set_priority(winapi::priority p);
	}
}
namespace std
{
	export void swap(winapi::process& lhs, winapi::process& rhs) noexcept;
}

#ifdef _WIN32

class winapi::process
{
public:
	static constexpr int still_active = STILL_ACTIVE;

	process() noexcept = default;
	process(process&& rhs) noexcept;
	explicit process(unique_handle&& h) noexcept;
	explicit process(handle_t h) noexcept;

	explicit process(call_self_t);
	explicit process(call_self_t, std::span< const std::string > args);
	explicit process(call_self_t, std::span< const char*const > args);
	explicit process(call_self_t, const char* args);

	explicit process(const char* app, std::span< const std::string > args);
	explicit process(const char* app, std::span< const char*const > args);
	explicit process(const char* app, const char* args);
	explicit process(std::span< const char*const > cl_cmd);

	explicit process(const char* cl_cmd);

	~process();
	process& operator=(process&& rhs) noexcept;

	handle_t native() const noexcept;
	unique_handle release();
	void swap(process& rhs) noexcept;

	int id();
	int exit_code();

	bool joinable() const noexcept;
	void wait();
	bool wait(std::chrono::milliseconds dur);
	void join();
	bool join(std::chrono::milliseconds dur);
	void detach();

	winapi::priority priority();
	void set_priority(winapi::priority p);

	void kill();
	void kill(int exit_code);

private:
	unique_handle native_;
};

#endif
