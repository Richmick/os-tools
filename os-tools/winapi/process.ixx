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
		struct initial_params;
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
		std::string executable();
		std::optional< std::string > get_env(const std::string& name);
		void set_env(const std::string& name, const std::string& value);
		void delete_env(const std::string& name);
		std::unordered_map< std::string, std::string > environment();
		[[noreturn]] void exit(int code);
		process::priority get_priority();
		void set_priority(process::priority p);

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
};
struct winapi::process::initial_params
{
	const security_attributes_t* attributes = nullptr;
	const security_attributes_t* thread_attributes = nullptr;
	bool inherit_handles = false;
	priority explicit_priority = priority::normal;
	bool use_priority = false;
	bool create_group = false;
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
