module;
	#ifdef _WIN32
		#include <atomic>
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
		explicit inline thread(F&& f, Args&&... args);
		explicit inline thread(initial_params& params, auto&& f, auto&&... args);
		explicit thread(const initial_params& params, auto&& f, auto&&... args);
		explicit inline thread(initial_params&& params, auto&& f, auto&&... args);
		explicit inline thread(const initial_params&& params, auto&& f, auto&&... args);

		explicit thread(thread_function_t f, void* data);
		explicit thread(const initial_params& params, thread_function_t f, void* data);

		explicit thread(const process& proc, thread_function_t f, void* data);
		explicit thread(const process& proc, const initial_params& params, thread_function_t f, void* data);

		~thread();
		thread& operator=(thread&& rhs) noexcept;

		static unsigned int hardware_concurrency() noexcept;

		native_handle_type native_handle() const noexcept;
		unique_handle release_handle() noexcept;
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
		struct data_erasor;
		template< class F, class... Args >
		struct typed_erasor;
		struct no_params_tag_t
		{};

		static inline dword_t create_suspended = CREATE_SUSPENDED;
		static inline dword_t stack_size_param_is_a_reservation = STACK_SIZE_PARAM_IS_A_RESERVATION;

		unique_handle native_;
		data_erasor* erasor_ = nullptr;

		template< class F, class... Args >
		explicit thread(no_params_tag_t, F&& f, Args&&... args);

		template< class F, class... Args >
		static dword_t WINAPI thread_function(void* data);
		static security_attributes_t get_attributes(const initial_params& params);
		static dword_t get_flags(const initial_params& params);
	};
	namespace this_thread
	{
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
	normal = THREAD_PRIORITY_NORMAL,
	greater = THREAD_PRIORITY_ABOVE_NORMAL,
	high = THREAD_PRIORITY_HIGHEST,
	realtime = THREAD_PRIORITY_TIME_CRITICAL,
	unset = THREAD_PRIORITY_ERROR_RETURN
};
struct winapi::thread::initial_params
{
	security_attributes_t* attributes = nullptr; // can be modified to match requested params
	bool inherit_handles = false;
	bool create_suspended = false;
	std::size_t stack_size = 0; // default size
	bool defer_stack_commit = false; // reserve stack adresses but do not allocate
	thread::priority priority = thread::priority::unset;
};
struct winapi::thread::data_erasor
{
	std::atomic_flag oneside_released = ATOMIC_FLAG_INIT;
	data_erasor() = default;
	virtual ~data_erasor() = default;
};
template< class F, class... Args >
struct winapi::thread::typed_erasor: public data_erasor
{
	template< class F1, class... Args1 >
	typed_erasor(F1&& f, Args1&&... args);
	~typed_erasor() override = default;
	F functor;
	std::tuple< Args... > arguments;
};

template< class F, class... Args >
template< class F1, class... Args1 >
winapi::thread::typed_erasor< F, Args... >::typed_erasor(F1&& f, Args1&&...args):
	functor(std::forward< F1 >(f)),
	arguments(std::forward< Args1 >(args)...)
{}

template< class F, class... Args >
winapi::dword_t WINAPI winapi::thread::thread_function(void* data)
{
	static_assert(std::invocable< F, Args... >);

	using res = std::invoke_result_t< F, Args... >;
	using data_t = typed_erasor< F, Args... >;
	data_t* typed_data = static_cast< data_t* >(data);
	int result;
	try
	{
		if constexpr (std::is_same_v< res, int >)
		{
			result = std::apply(typed_data->functor, typed_data->arguments);
		}
		else
		{
			std::apply(typed_data->functor, typed_data->arguments);
			result = 0;
		}
	}
	catch (...)
	{
		if (typed_data->oneside_released.test_and_set()) delete typed_data;
		throw;
	}
	if (typed_data->oneside_released.test_and_set()) delete typed_data;
	return result;
}
template< class F, class... Args >
inline winapi::thread::thread(F&& f, Args&&... args):
	thread(no_params_tag_t{}, std::forward< F >(f), std::forward< Args >(args)...)
{}
template< class F, class... Args >
winapi::thread::thread(no_params_tag_t, F&& f, Args&&... args)
{
	using data_t = typed_erasor< std::decay_t< F >, std::decay_t< Args >... >;
	std::unique_ptr< data_t > data{new data_t{std::forward< F >(f), std::forward< Args >(args)...}};
	*this = thread{&thread_function< std::decay_t< F >, std::decay_t< Args >... >,
				static_cast< void* >(data.get())};
	erasor_ = data.release();
}
winapi::thread::thread(const initial_params& params, auto&& f, auto&&...args)
{
	using data_t = typed_erasor< std::decay_t< decltype(f) >, std::decay_t< decltype(args) >... >;
	std::unique_ptr< data_t > data{new data_t{std::forward< decltype(f) >(f),
					std::forward< decltype(args) >(args)...}};
	*this = thread{params, &thread_function< std::decay_t< decltype(f) >,
					std::decay_t< decltype(args) >... >,
				static_cast< void* >(data.get())};
	erasor_ = data.release();
}
inline winapi::thread::thread(initial_params& params, auto&& f, auto&&...args):
	thread(static_cast< const initial_params& >(params),
				std::forward< decltype(f) >(f), std::forward< decltype(args) >(args)...)
{}
inline winapi::thread::thread(initial_params&& params, auto&& f, auto&&...args):
	thread(static_cast< const initial_params& >(params),
				std::forward< decltype(f) >(f), std::forward< decltype(args) >(args)...)
{}
inline winapi::thread::thread(const initial_params&& params, auto&& f, auto&&...args):
	thread(params, std::forward< decltype(f) >(f), std::forward< decltype(args) >(args)...)
{}

#endif
