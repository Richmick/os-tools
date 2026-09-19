module;
	#ifdef _WIN32
		#define WIN32_LEAN_AND_MEAN
		#include <windows.h>
	#endif
export module os.winapi:thread;

#ifdef _WIN32
import std;
import :aliases;
import :handle;
import :process;

export namespace winapi
{
	class thread
	{
	public:
		enum class priority;
		struct initial_params;
		using native_handle_type = handle_t;
		using id = dword_t;
		using thread_function_t = LPTHREAD_START_ROUTINE;

		thread() noexcept = default;
		thread(thread&& rhs) noexcept;
		explicit thread(native_handle_type h) noexcept;
		explicit thread(unique_handle&& h) noexcept;

		template< class F, class... Args >
		explicit thread(F&& f, Args&&... args);
		template< class F, class... Args >
		explicit thread(const initial_params& params, F&& f, Args&&... args);
		explicit thread(thread_function_t f, void* data);
		explicit thread(const initial_params& params, thread_function_t f, void* data);

		explicit thread(const process& proc, thread_function_t f, void* data);
		explicit thread(const process& proc, const initial_params& params, thread_function_t f, void* data);

		~thread();
		thread& operator=(thread&& rhs) noexcept;

		static unsigned int hardware_concurrency() noexcept;

		native_handle_type native() const noexcept;
		native_handle_type native_handle() const noexcept;
		unique_handle release() noexcept;
		void swap(thread& rhs) noexcept;
		id get_id() const;
		process::id get_process_id() const;

		bool joinable() const noexcept;
		int wait() const;
		bool wait(std::chrono::milliseconds dur) const;
		int join();
		bool join(std::chrono::milliseconds dur);
		void detach();

		int exit_code() const;
		void kill() const;
		void kill(int exit_code) const;

		priority get_priority() const;
		void set_priority(priority p) const;
	private:
		static inline dword_t create_suspended = CREATE_SUSPENDED;
		static inline dword_t stack_size_param_is_a_reservation = STACK_SIZE_PARAM_IS_A_RESERVATION;

		unique_handle native_;

		template< class F, class... Args >
		static dword_t WINAPI thread_function(void* data);
		static security_attributes_t get_attributes(const initial_params& params);
		static dword_t get_flags(const initial_params& params);
	};
	namespace this_thread
	{
		thread::native_handle_type native();
		thread::native_handle_type native_handle();
		thread::id get_id();
		process::id get_process_id();
		[[noreturn]] void exit(int code);
		thread::priority get_priority();
		void set_priority(thread::priority p);
	}
}
namespace std
{
	export void swap(winapi::thread& lhs, winapi::thread& rhs) noexcept;
}

enum class winapi::thread::priority
{
	idle = THREAD_PRIORITY_IDLE,
	low = THREAD_PRIORITY_LOWEST,
	lower = THREAD_PRIORITY_BELOW_NORMAL,
	normal = NORMAL_PRIORITY_CLASS,
	greater = THREAD_PRIORITY_ABOVE_NORMAL,
	high = THREAD_PRIORITY_HIGHEST,
	realtime = THREAD_PRIORITY_TIME_CRITICAL,
	error = THREAD_PRIORITY_ERROR_RETURN
};
struct winapi::thread::initial_params
{
	security_attributes_t* attributes = nullptr; // can be modified to match requested params
	bool inherit_handles = false;
	bool create_suspended = false;
	std::size_t stack_size = 0; // default size
	bool defer_stack_commit = false; // reserve stack adresses but do not allocate
	priority explicit_priority = priority::normal;
	bool use_priority = false;
};

template< class F, class... Args >
winapi::dword_t WINAPI winapi::thread::thread_function(void* data)
{
	static_assert(std::invocable< std::decay_t< F >, std::decay_t< Args >... >);

	using data_t = std::pair< std::tuple< std::decay_t< Args >... >, F >;
	std::unique_ptr< data_t > typed_data{static_cast< data_t* >(data)};
	std::apply(typed_data->second, typed_data->first);
	return 0;
}
template< class F, class... Args >
winapi::thread::thread(F&& f, Args&&... args)
{
	using data_t = std::pair< std::tuple< std::decay_t< Args >... >, F >;
	std::unique_ptr data = std::make_unique< data_t >(std::piecewise_construct,
				std::forward_as_tuple(std::forward< Args >(args)...),
				std::forward_as_tuple(std::forward< F >(f)));
	*this = thread{&thread_function< F, Args... >, static_cast< void* >(data.get())};
	data.release();
}
template< class F, class ...Args >
winapi::thread::thread(const initial_params& params, F&& f, Args&&...args)
{
	using data_t = std::pair< std::tuple< std::decay_t< Args >... >, F >;
	std::unique_ptr data = std::make_unique< data_t >(std::piecewise_construct,
				std::forward_as_tuple(std::forward< Args >(args)...),
				std::forward_as_tuple(std::forward< F >(f)));
	*this = thread{params, &thread_function< F, Args... >, static_cast< void* >(data.get())};
	data.release();
}

#endif
