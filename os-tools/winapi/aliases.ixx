module;
	#ifdef _WIN32
		#define WIN32_LEAN_AND_MEAN
		#include <windows.h>
	#endif
export module os.winapi:aliases;

#ifdef _WIN32
import std;

export namespace winapi
{
	class unique_handle;
	class thread;
	class process;

	using handle_t = HANDLE;
	using hinstance_t = HINSTANCE;
	using hmodule_t = HMODULE;
	using dword_t = DWORD;

	using tchar_t = TCHAR;
	constexpr inline bool use_ascii = std::is_same_v< tchar_t, char >;
	using tstring = std::conditional_t< use_ascii, std::string, std::wstring >;

	using security_attributes_t = SECURITY_ATTRIBUTES;
	using startup_info_t = STARTUPINFO;
	using process_info_t = PROCESS_INFORMATION;

	namespace calls
	{
		// Handles
		using ::CloseHandle;
		using ::SetHandleInformation;
		using ::GetStdHandle;

		// Process
		using ::CreateProcessA;
		using ::CreateProcessW;
		using ::TerminateProcess;
		using ::ExitProcess;
		using ::GetCurrentProcess;
		using ::GetCurrentProcessId;
		using ::GetProcessId;
		using ::GetPriorityClass;
		using ::SetPriorityClass;
		using ::GetExitCodeProcess;
		using ::GetModuleFileNameA;
		using ::GetModuleFileNameW;
		using ::GetEnvironmentVariableA;
		using ::GetEnvironmentVariableW;
		using ::SetEnvironmentVariableA;
		using ::SetEnvironmentVariableW;
		using ::GetEnvironmentStrings;
		using ::GetEnvironmentStringsW;
		using ::FreeEnvironmentStringsA;
		using ::FreeEnvironmentStringsW;

		// Thread
		using ::CreateThread;
		using ::CreateRemoteThread;
		using ::TerminateThread;
		using ::ExitThread;
		using ::GetCurrentThread;
		using ::GetCurrentThreadId;
		using ::GetProcessIdOfThread;
		using ::GetThreadId;
		using ::GetThreadPriority;
		using ::SetThreadPriority;
		using ::GetExitCodeThread;

		// Others
		using ::GetLastError;
		using ::WaitForSingleObject;
		using ::WaitForMultipleObjects;
	}
	constexpr inline dword_t wait_success_0 = WAIT_OBJECT_0;
	constexpr inline dword_t wait_timeout = WAIT_TIMEOUT;

	std::system_error last_error(const char* msg);
	std::system_error last_error(const std::string& msg);
	void wait(handle_t h);
	bool wait(handle_t h, std::chrono::milliseconds dur);

	template< class CharT >
	concept char_option = std::is_same_v< CharT, char > || std::is_same_v< CharT, wchar_t >;
	template< char_option CharT >
	dword_t create_process_aw(const CharT* executable,
				startup_info_t& startup_info,
				process_info_t& proc_info,
				CharT* args = nullptr,
				security_attributes_t* proc_attr = nullptr,
				security_attributes_t* thread_attr = nullptr,
				bool inherit_handles = false,
				dword_t flags = 0,
				void* env = nullptr,
				const CharT* dir = nullptr);
	template< class CharT, class Traits = std::char_traits< CharT >, std::invocable< CharT*, std::size_t > F >
	std::basic_string< CharT, Traits > collect_string(F f);
}

template< winapi::char_option CharT >
winapi::dword_t winapi::create_process_aw(const CharT* executable, startup_info_t& startup_info,
			process_info_t& proc_info, CharT* args, security_attributes_t* proc_attr,
			security_attributes_t* thread_attr, bool inherit_handles, dword_t flags,
			void* env, const CharT* dir)
{
	if constexpr (std::is_same_v< CharT, char >) return winapi::calls::CreateProcessA(executable, args,
				proc_attr, thread_attr, inherit_handles, flags, env, dir, &startup_info, &proc_info);
	else return winapi::calls::CreateProcessW(executable, args,
				proc_attr, thread_attr, inherit_handles, flags, env, dir, &startup_info, &proc_info);
}
template< class CharT, class Traits, std::invocable< CharT*, std::size_t > F >
std::basic_string< CharT, Traits > winapi::collect_string(F f)
{
	constexpr std::size_t length = 256;
	{
		CharT buf[length];
		std::size_t res_len = f(buf, length);
		if (res_len < length) return {buf, res_len};
	}
	std::basic_string< CharT, Traits > buf;
	buf.reserve(length * 2);
	std::size_t res_len = f(buf.data(), buf.size());
	while (res_len == buf.size())
	{
		buf.clear();
		buf.reserve(res_len * 2);
		res_len = f(buf.data(), buf.size());
	}
	return {buf, res_len};
}

#endif
