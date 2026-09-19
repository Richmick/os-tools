module;
	#ifdef __linux__
		#include <pthread.h>
	#endif
export module os.linapi:thread;

#ifdef __linux__
import std;
import :aliases;
import :process;

namespace linapi
{
	class thread
	{
	public:
		struct initial_params;
		using priority = linapi::priority;
		using native_handle_type = thread_handle_t;
		using id = thread_handle_t;
		using thread_function_t = void*(*)(void*);

		thread() noexcept = default;
		thread(thread&& rhs) noexcept;
		explicit thread(native_handle_type h) noexcept;

		template< class F, class... Args >
		explicit thread(F&& f, Args&&... args);
		template< class F, class... Args >
		explicit thread(const initial_params& params, F&& f, Args&&... args);
		explicit thread(thread_function_t f, void* data);
		explicit thread(const initial_params& params, thread_function_t f, void* data);

		~thread();
		thread& operator=(thread&& rhs) noexcept;

		static unsigned int hardware_concurrency() noexcept;

		native_handle_type native() const noexcept;
		native_handle_type native_handle() const noexcept;
		native_handle_type release() noexcept;
		void swap(thread& rhs) noexcept;
		id get_id() const;
		process::id get_process_id() const; // same as this_process::get_id()

		bool joinable() const noexcept;
		int join();
		void* join_val();
		void detach();
		void kill() const;

		priority get_priority() const;
		void set_priority(priority p) const;
	private:
		static constexpr int implicit_priority = PTHREAD_INHERIT_SCHED;
		static constexpr int explicit_priority = PTHREAD_EXPLICIT_SCHED;
		thread_handle_t native_{};

		template< class F, class... Args >
		static void* thread_function(void* data);
	};
	export namespace this_thread
	{
		thread_handle_t native();
		thread_handle_t native_handle();
		thread_handle_t get_id();
		[[noreturn]] void exit(int code);
		[[noreturn]] void exit(void* data);
		thread::priority get_priority();
		void set_priority(thread::priority p);
	}
}
namespace std
{
	export void swap(linapi::thread& lhs, linapi::thread& rhs) noexcept;
}

struct linapi::thread::initial_params
{
	thread_attributes_t* attributes = nullptr; // can be modified to match requested params
	std::size_t stack_size = 0; // default size
	priority explicit_priority = priority::normal;
	bool use_priority = false;
};

template< class F, class... Args >
void* linapi::thread::thread_function(void* data)
{
	static_assert(std::invocable< std::decay_t< F >, std::decay_t< Args >... >);

	using data_t = std::pair< std::tuple< std::decay_t< Args >... >, F >;
	std::unique_ptr< data_t > typed_data{static_cast< data_t* >(data)};
	std::apply(typed_data->second, typed_data->first);
	return 0;
}
template< class F, class... Args >
linapi::thread::thread(F&& f, Args&&... args)
{
	using data_t = std::pair< std::tuple< std::decay_t< Args >... >, F >;
	std::unique_ptr data = std::make_unique< data_t >(std::piecewise_construct,
				std::forward_as_tuple(std::forward< Args >(args)...),
				std::forward_as_tuple(std::forward< F >(f)));
	*this = thread{&thread_function< F, Args... >, static_cast< void* >(data.get())};
	data.release();
}
template< class F, class ...Args >
linapi::thread::thread(const initial_params& params, F&& f, Args&&...args)
{
	using data_t = std::pair< std::tuple< std::decay_t< Args >... >, F >;
	std::unique_ptr data = std::make_unique< data_t >(std::piecewise_construct,
				std::forward_as_tuple(std::forward< Args >(args)...),
				std::forward_as_tuple(std::forward< F >(f)));
	*this = thread{params, &thread_function< F, Args... >, static_cast< void* >(data.get())};
	data.release();
}

#endif
