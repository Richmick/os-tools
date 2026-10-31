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
import :signal;

export namespace winapi
{
	std::string prepare_cmd_args(const char* app, std::span< const char*const > args);
	std::string prepare_cmd_args(const char* app, std::span< const std::string > args);
	std::string join_args(std::span< const std::string > data);
	std::string join_args(std::span< const char*const > data);
	std::string screen(const std::string& arg);
	std::string screen(const char* arg);

	std::wstring prepare_cmd_args(const wchar_t* app, std::span< const wchar_t*const > args);
	std::wstring prepare_cmd_args(const wchar_t* app, std::span< const std::wstring > args);
	std::wstring join_args(std::span< const std::wstring > data);
	std::wstring join_args(std::span< const wchar_t*const > data);
	std::wstring screen(const std::wstring& arg);
	std::wstring screen(const wchar_t* arg);
	
	class process
	{
	public:
		/// Priorities for processes (do not match thread priorities)
		enum class priority: dword_t;
		/// Signal type enum
		using signal = winapi::ipc::signal::type;
		/// Set of parameters for precise startup within ascii charset
		struct initial_params_a;
		/// Set of parameters for precise startup
		struct initial_params;

		using group_id = dword_t;
		/// Id type
		using id = dword_t;
		/// Os handle type (handle_t, not int_handle_t)
		using native_handle_type = handle_t;
		/// Exit code, meaning that process is still alive (can overlap with user codes)
		static constexpr int still_active = STILL_ACTIVE;

		process() noexcept = default;
		process(process&& rhs) noexcept = default;
		explicit process(unique_handle&& h) noexcept;
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

		// Winapi specific (executable can be obtained by executable_aw< char >()):
		explicit process(const char* app, std::span< const std::string > args, std::error_code& err);
		explicit process(const char* app, std::span< const char* const > args, std::error_code& err);
		explicit process(initial_params& params, const char* app, std::span< const std::string > args,
					std::error_code& err);
		explicit process(initial_params& params, const char* app, std::span< const char* const > args,
					std::error_code& err);

		explicit process(const std::wstring& app, std::wstring args, std::error_code& err);
		explicit process(std::wstring cl_cmd, std::error_code& err);
		explicit process(initial_params& params, const std::wstring& app, std::wstring args, std::error_code& err);
		explicit process(initial_params& params, std::wstring cl_cmd, std::error_code& err);

		explicit process(const std::string& app, std::string args, std::error_code& err);
		explicit process(std::string cl_cmd, std::error_code& err);
		explicit process(initial_params& params, const std::string& app, std::string args, std::error_code& err);
		explicit process(initial_params& params, std::string cl_cmd, std::error_code& err);

		~process();
		process& operator=(process&& rhs) noexcept;

		native_handle_type native_handle() const noexcept;
		native_handle_type release_handle() noexcept;
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
		static constexpr inline dword_t create_new_group = CREATE_NEW_PROCESS_GROUP;
		static constexpr inline dword_t use_provided_handles = STARTF_USESTDHANDLES;
		unique_handle native_;

		static unique_handle create(const char* app, char* console, std::error_code& err);
		static unique_handle create(const wchar_t* app, wchar_t* console, std::error_code& err);
		static unique_handle create(initial_params& params, const char* app, char* console, std::error_code& err);
		static unique_handle create(initial_params& params, const wchar_t* app, wchar_t* console, std::error_code& err);
	};
	namespace this_process
	{
		process::native_handle_type native_handle();
		process::id get_id();
		path executable();
		std::optional< std::string > get_env(const std::string& name);
		void set_env(const std::string& name, const std::string& value);
		void delete_env(const std::string& name);
		std::unordered_map< std::string, std::string > environment();
		process::priority get_priority();
		void set_priority(process::priority p);

		[[noreturn]] void _exit(int code);

		handle_t stdin();
		handle_t stdout();
		handle_t stderr();
		handle_t stdin(std::error_code& err) noexcept;
		handle_t stdout(std::error_code& err) noexcept;
		handle_t stderr(std::error_code& err) noexcept;
		handle_t std_handle(io type);
		handle_t std_handle(io type, std::error_code& err) noexcept;

		template< char_option CharT = tchar_t, class Traits = std::char_traits< CharT > >
		std::basic_string< CharT, Traits > executable_aw();
		template< char_option CharT = tchar_t, class Traits = std::char_traits< CharT > >
		std::unordered_map< std::basic_string< CharT, Traits >,
					std::basic_string< CharT, Traits > > environment_aw();

		template< char_option CharT = tchar_t, class Traits = std::char_traits< CharT > >
		std::optional< std::basic_string< CharT, Traits > >
					get_env_aw(const std::basic_string< CharT, Traits >& name);
		template< char_option CharT = tchar_t, class Traits = std::char_traits< CharT > >
		void set_env_aw(const std::basic_string< CharT, Traits >& name,
					const std::basic_string< CharT, Traits >& value);
		template< char_option CharT = tchar_t, class Traits = std::char_traits< CharT > >
		void delete_env_aw(const std::basic_string< CharT, Traits >& name);
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
	unset = static_cast< dword_t >(-1)
};
struct winapi::process::initial_params
{
	void* process_security_descriptor = nullptr;
	void* thread_security_descriptor = nullptr;
	inheritmode inherit_process_handle = inheritmode::none;
	inheritmode inherit_thread_handle = inheritmode::none;

	inheritmode inherit_handles = inheritmode::none;
	bool create_group = false;
	dword_t flags = 0;
	process::priority priority = process::priority::unset;

	startup_info_t< wchar_t > startup_info{.cb = sizeof(startup_info_t< wchar_t >)}; // can be modified
	handle_t stdin = nullptr;
	handle_t stdout = nullptr;
	handle_t stderr = nullptr;
};



template< winapi::char_option CharT, class Traits >
std::basic_string< CharT, Traits > winapi::this_process::executable_aw()
{
	struct exe_getter
	{
		static inline std::size_t operator()(char* buf, std::size_t len)
		{
			std::size_t res = winapi::calls::GetModuleFileNameA(nullptr, buf, len);
			if (res == 0) throw winapi::last_error("failed to retrieve executable");
			return res;
		}
		static inline std::size_t operator()(wchar_t* buf, std::size_t len)
		{
			std::size_t res = winapi::calls::GetModuleFileNameW(nullptr, buf, len);
			if (res == 0) throw winapi::last_error("failed to retrieve executable");
			return res;
		}
	};
	return collect_string< CharT >(exe_getter{});
}
template< winapi::char_option CharT, class Traits >
std::unordered_map< std::basic_string< CharT, Traits >, std::basic_string< CharT, Traits > >
		winapi::this_process::environment_aw()
{
	using ptr_t = std::unique_ptr< CharT[], std::conditional_t< std::is_same_v< CharT, char >,
					decltype(&winapi::calls::FreeEnvironmentStringsA),
					decltype(&winapi::calls::FreeEnvironmentStringsW) > >;
	using str_t = std::basic_string< CharT, Traits >;
	std::unordered_map< str_t, str_t > result;
	std::optional< ptr_t > env;
	if constexpr (std::is_same_v< CharT, char >)
		env = ptr_t{winapi::calls::GetEnvironmentStrings(), &winapi::calls::FreeEnvironmentStringsA};
	else
		env = ptr_t{winapi::calls::GetEnvironmentStringsW(), &winapi::calls::FreeEnvironmentStringsW};

	if (env->get() == nullptr)
	{
		throw winapi::last_error("failed to retrieve env");
	}
	CharT* caret = env->get();
	while (*caret != '\0')
	{
		using view = std::basic_string_view< CharT, Traits >;
		view line{caret};
		typename view::size_type eq_sign = line.find_first_of('=');
		if (eq_sign == view::npos)
		{
			throw std::runtime_error{"no equal sign in env variable"};
		}
		view key = line.substr(0, eq_sign), value = line.substr(eq_sign + 1);
		result.emplace(key, value);
		caret += line.length() + 1;
	}
	return result;
}

template< winapi::char_option CharT, class Traits >
std::optional< std::basic_string< CharT, Traits > >
		winapi::this_process::get_env_aw(const std::basic_string< CharT, Traits >& name)
{
	using return_type = std::optional< std::basic_string< CharT, Traits > >;
	constexpr std::size_t length = 256;
	std::size_t res_len;
	{
		CharT buf[length];
		if constexpr (std::is_same_v< CharT, char >)
			res_len = winapi::calls::GetEnvironmentVariableA(name.c_str(), buf, length);
		else
			res_len = winapi::calls::GetEnvironmentVariableW(name.c_str(), buf, length);
		if (res_len == 0)
		{
			if (winapi::calls::GetLastError() == ERROR_ENVVAR_NOT_FOUND) return std::nullopt;
			return {""};
		}
		if (res_len < length) return return_type{std::in_place, buf, res_len};
	}
	std::basic_string< CharT, Traits > buf;
	while (true)
	{
		buf.resize(res_len);
		if constexpr (std::is_same_v< CharT, char >)
			res_len = winapi::calls::GetEnvironmentVariableA(name.c_str(), buf.data(), buf.length() + 1);
		else
			res_len = winapi::calls::GetEnvironmentVariableW(name.c_str(), buf.data(), buf.length() + 1);
		if (res_len == 0)
		{
			if (winapi::calls::GetLastError() == ERROR_ENVVAR_NOT_FOUND) return std::nullopt;
			return {""};
		}
		if (res_len <= buf.length()) return buf;
	}
}
template< winapi::char_option CharT, class Traits >
void winapi::this_process::set_env_aw(const std::basic_string< CharT, Traits >& name,
			const std::basic_string< CharT, Traits >& value)
{
	bool success;
	if constexpr (std::is_same_v< CharT, char >)
		success = winapi::calls::SetEnvironmentVariableA(name.c_str(), value.c_str());
	else
		success = winapi::calls::SetEnvironmentVariableW(name.c_str(), value.c_str());
	if (!success) throw winapi::last_error("failed to set env var");
}
template< winapi::char_option CharT, class Traits >
void winapi::this_process::delete_env_aw(const std::basic_string< CharT, Traits >& name)
{
	bool success;
	if constexpr (std::is_same_v< CharT, char >)
		success = winapi::calls::SetEnvironmentVariableA(name.c_str(), nullptr);
	else
		success = winapi::calls::SetEnvironmentVariableW(name.c_str(), nullptr);
	if (!success) throw winapi::last_error("failed to clear env var");
}

#endif
