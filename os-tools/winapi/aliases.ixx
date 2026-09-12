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
	constexpr inline dword_t wait_success_0 = WAIT_OBJECT_0;
	constexpr inline dword_t wait_timeout = WAIT_TIMEOUT;

	std::system_error last_error(const char* msg);
	std::system_error last_error(const std::string& msg);
	void wait(handle_t h);
	bool wait(handle_t h, std::chrono::milliseconds dur);
	template< class CharT >
	dword_t create_process(const CharT* executable,
				startup_info_t& startup_info,
				process_info_t& proc_info,
				CharT* args = nullptr,
				security_attributes_t* proc_attr = nullptr,
				security_attributes_t* thread_attr = nullptr,
				bool inherit_handles = false,
				dword_t flags = 0,
				void* env = nullptr,
				const CharT* dir = nullptr);
	template< class CharT >
	dword_t get_module_file_name(hmodule_t mod, CharT* buf, std::size_t len);

	enum class priority: dword_t
	{
		idle = IDLE_PRIORITY_CLASS,
		lower = BELOW_NORMAL_PRIORITY_CLASS,
		normal = NORMAL_PRIORITY_CLASS,
		greater = ABOVE_NORMAL_PRIORITY_CLASS,
		high = HIGH_PRIORITY_CLASS,
		realtime = REALTIME_PRIORITY_CLASS,
	};
}

template< class CharT >
winapi::dword_t winapi::create_process(const CharT* executable, startup_info_t& startup_info,
			process_info_t& proc_info, CharT* args, security_attributes_t* proc_attr,
			security_attributes_t* thread_attr, bool inherit_handles, dword_t flags,
			void* env, const CharT* dir)
{
	if constexpr (std::is_same_v< CharT, char >) return winapi::CreateProcessA(executable, args,
				proc_attr, thread_attr, inherit_handles, flags, env, dir, &startup_info, &proc_info);
	else return winapi::CreateProcessW(executable, args,
				proc_attr, thread_attr, inherit_handles, flags, env, dir, &startup_info, &proc_info);
}
template< class CharT >
winapi::dword_t winapi::get_module_file_name(hmodule_t h, CharT* buf, std::size_t len)
{
	if constexpr (std::is_same_v< CharT, char >) return winapi::GetModuleFileNameA(h, buf, len);
	else return winapi::GetModuleFileNameW(h, buf, len);
}

#endif
