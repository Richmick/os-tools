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
#endif

export namespace winapi
{
	class thread;
	namespace this_thread
	{
		handle_t native();
		int id();
		[[noreturn]] void exit(int code);
		winapi::priority priority();
		void set_priority(winapi::priority p);
	}
}
namespace std
{
	export void swap(winapi::thread& lhs, winapi::thread& rhs) noexcept;
}

#ifdef _WIN32

class winapi::thread
{
public:
	struct initial_params;
	using thread_function_t = LPTHREAD_START_ROUTINE;

	thread() noexcept = default;
	thread(thread&& rhs) noexcept;
	explicit thread(unique_handle&& h) noexcept;
	explicit thread(handle_t h) noexcept;

	template< class F, class... Args >
	explicit thread(F&& f, Args&&... args);
	template< class F, class... Args >
	explicit thread(const initial_params& params, F&& f, Args&&... args);
	explicit thread(thread_function_t f, void* data);
	explicit thread(const initial_params& params, thread_function_t f, void* data);

	template< class F, class... Args >
	explicit thread(const process& proc, F&& f, Args&&... args);
	template< class F, class... Args >
	explicit thread(const process& proc, const initial_params& params, F&& f, Args&&... args);
	explicit thread(const process& proc, thread_function_t f, void* data);
	explicit thread(const process& proc, const initial_params& params, thread_function_t f, void* data);

	~thread();
	thread& operator=(thread&& rhs) noexcept;

	handle_t native() const noexcept;
	unique_handle release();
	void swap(thread& rhs) noexcept;
	int id();
	int process_id();

	bool joinable() const noexcept;
	void wait();
	bool wait(std::chrono::milliseconds dur);
	void join();
	bool join(std::chrono::milliseconds dur);
	void detach();

	int exit_code();
	void kill();
	void kill(int exit_code);

	winapi::priority priority();
	void set_priority(winapi::priority p);
private:
	static inline dword_t create_suspended = CREATE_SUSPENDED;
	static inline dword_t stack_size_param_is_a_reservation = STACK_SIZE_PARAM_IS_A_RESERVATION;

	unique_handle native_;

	template< class F, class... Args >
	static dword_t WINAPI thread_function(void* data);
	static security_attributes_t get_attributes(const initial_params& params);
	static dword_t get_flags(const initial_params& params);
};
struct winapi::thread::initial_params
{
	security_attributes_t attributes{};
	bool inherit_handles = false;
	bool create_suspended = false;
	std::size_t stack_size = 0; // default size
	bool defer_stack_commit = false; // reserve stack adresses but do not allocate
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
template< class F, class... Args >
winapi::thread::thread(const process& proc, F&& f, Args&&... args)
{
	using data_t = std::pair< std::tuple< std::decay_t< Args >... >, F >;
	std::unique_ptr data = std::make_unique< data_t >(std::piecewise_construct,
				std::forward_as_tuple(std::forward< Args >(args)...),
				std::forward_as_tuple(std::forward< F >(f)));
	*this = thread{proc, &thread_function< F, Args... >, static_cast< void* >(data.get())};
	data.release();
}
template< class F, class ...Args >
winapi::thread::thread(const process& proc, const initial_params& params, F&& f, Args&&...args)
{
	using data_t = std::pair< std::tuple< std::decay_t< Args >... >, F >;
	std::unique_ptr data = std::make_unique< data_t >(std::piecewise_construct,
				std::forward_as_tuple(std::forward< Args >(args)...),
				std::forward_as_tuple(std::forward< F >(f)));
	*this = thread{proc, params, &thread_function< F, Args... >, static_cast< void* >(data.get())};
	data.release();
}

#endif
