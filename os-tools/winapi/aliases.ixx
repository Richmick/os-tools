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
	using std::unique_lock;
	using std::shared_lock;
	using std::filesystem::path;

	enum class inheritmode: bool
	{
		none = false,
		all = true
	};
	enum class openmode
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

	using handle_t = HANDLE;
	using int_handle_t = std::size_t;
	using hinstance_t = HINSTANCE;
	using hmodule_t = HMODULE;
	using dword_t = DWORD;
	using cpuset_info = SYSTEM_CPU_SET_INFORMATION;
	using cpuset_info_type = CPU_SET_INFORMATION_TYPE;
	using processor_number = PROCESSOR_NUMBER;

	using char_t = wchar_t; ///< Preferable char type for calls
	using string = std::wstring; ///< Preferable string type for calls
	using string_view = std::wstring_view; ///< Preferable string type for calls
	string to_string(handle_t h);
	string to_string(int_handle_t h);
	handle_t to_handle(const string& h);
	handle_t to_handle(const string_view& h);
	handle_t to_handle(const std::string& h);
	handle_t to_handle(const std::string_view& h);

	using tchar_t = TCHAR;
	constexpr inline bool use_ascii = std::is_same_v< tchar_t, char >;
	using tstring = std::conditional_t< use_ascii, std::string, std::wstring >;

	using security_attributes_t = SECURITY_ATTRIBUTES;
	template< class CharT = char_t >
	using startup_info_t = std::conditional_t< std::is_same_v< CharT, char >, STARTUPINFOA, STARTUPINFOW >;
	using process_info_t = PROCESS_INFORMATION;
	using critical_section_t = CRITICAL_SECTION;
	using condition_variable_t = CONDITION_VARIABLE;
	using srw_lock_t = SRWLOCK;

	struct clock
	{
		using rep = std::int64_t;
		using period = std::ratio< 1, 10'000'000 >;
		using duration = std::chrono::duration< rep, period >;
		using time_point = std::chrono::time_point< clock >;

		static constexpr inline bool is_steady = true;
	};
	using sys_time = clock::time_point;
	using sys_duration = clock::duration;
	struct times
	{
		sys_duration user, kernel, idle;
		sys_duration real() const noexcept;
	};

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
		using ::SetThreadIdealProcessorEx;
		using ::GetThreadIdealProcessorEx;
		using ::GetExitCodeThread;

		// Semaphore
		using ::CreateSemaphoreA;
		using ::CreateSemaphoreW;
		using ::CreateSemaphoreExA;
		using ::CreateSemaphoreExW;
		using ::OpenSemaphoreA;
		using ::OpenSemaphoreW;
		using ::ReleaseSemaphore;

		// Mutex
		using ::InitializeCriticalSection;
		using ::InitializeCriticalSectionAndSpinCount;
		using ::DeleteCriticalSection;
		using ::EnterCriticalSection;
		using ::TryEnterCriticalSection;
		using ::LeaveCriticalSection;

		// Shared mutex
		using ::InitializeSRWLock;
		using ::AcquireSRWLockExclusive;
		using ::AcquireSRWLockShared;
		using ::TryAcquireSRWLockExclusive;
		using ::TryAcquireSRWLockShared;
		using ::ReleaseSRWLockExclusive;
		using ::ReleaseSRWLockShared;

		// Condition variable
		using ::InitializeConditionVariable;
		using ::SleepConditionVariableCS;
		using ::SleepConditionVariableSRW;
		using ::WakeAllConditionVariable;
		using ::WakeConditionVariable;

		// Files
		using ::ReadFile;
		using ::ReadFileEx;
		using ::WriteFile;
		using ::WriteFileEx;

		// Pipes
		using ::CreatePipe;

		// Events
		using ::CreateEventA;
		using ::CreateEventW;
		using ::SetEvent;

		// Signals
		using ::SetConsoleCtrlHandler;
		using ::GenerateConsoleCtrlEvent;

		// Numa
		using ::GetCurrentProcessorNumberEx;
		using ::GetSystemCpuSetInformation;
		using ::GetNumaProcessorNode;
		using ::GetNumaProcessorNodeEx;
		using ::GetActiveProcessorCount;
		using ::GetActiveProcessorGroupCount;
		using ::GetMaximumProcessorCount;
		using ::GetMaximumProcessorGroupCount;

		// Others
		using ::GetLastError;
		using ::WaitForSingleObject;
		using ::WaitForMultipleObjects;
	}
	constexpr inline dword_t wait_success_0 = WAIT_OBJECT_0;
	constexpr inline dword_t wait_timeout = WAIT_TIMEOUT;

	std::error_code last_error_code();
	std::system_error last_error(const char* msg);
	std::system_error last_error(const std::string& msg);
	void throw_on_error(std::error_code err, const char* msg);
	void throw_on_error(std::error_code err, const std::string& msg);
	void process_error(bool success, std::error_code& err) noexcept;
	handle_t process_error(handle_t res, std::error_code& err) noexcept;
	
	enum class io: dword_t
	{
		in = STD_INPUT_HANDLE,
		out = STD_OUTPUT_HANDLE,
		err = STD_ERROR_HANDLE
	};

	void wait(handle_t h);
	bool wait(handle_t h, std::error_code& err) noexcept;
	bool wait(handle_t h, std::chrono::milliseconds dur);
	bool wait(handle_t h, std::chrono::milliseconds dur, std::error_code& err) noexcept;

	template< class CharT >
	concept char_option = std::is_same_v< CharT, char > || std::is_same_v< CharT, wchar_t >;
	template< char_option CharT >
	dword_t create_process_aw(const CharT* executable,
				startup_info_t< CharT >& startup_info,
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
winapi::dword_t winapi::create_process_aw(const CharT* executable, startup_info_t< CharT >& startup_info,
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
