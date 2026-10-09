module;
	#ifdef __linux__
		#include <atomic>
		#include <cassert>
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
		explicit inline thread(F&& f, Args&&... args);
		explicit inline thread(initial_params& params, auto&& f, auto&&... args);
		explicit thread(const initial_params& params, auto&& f, auto&&... args);
		explicit inline thread(initial_params&& params, auto&& f, auto&&... args);
		explicit inline thread(const initial_params&& params, auto&& f, auto&&... args);

		explicit thread(thread_function_t f, void* data);
		explicit thread(const initial_params& params, thread_function_t f, void* data);

		~thread();
		thread& operator=(thread&& rhs) noexcept;

		static unsigned int hardware_concurrency() noexcept;

		native_handle_type native_handle() const noexcept;
		native_handle_type release_handle() noexcept;
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
		struct data_erasor;
		template< class F, class... Args >
		struct typed_erasor;
		struct no_params_tag_t
		{};

		static constexpr int implicit_priority = PTHREAD_INHERIT_SCHED;
		static constexpr int explicit_priority = PTHREAD_EXPLICIT_SCHED;
		thread_handle_t native_{};
		data_erasor* erasor_ = nullptr;

		template< class F, class... Args >
		explicit thread(no_params_tag_t, F&& f, Args&&... args);

		template< class F, class... Args >
		static void* thread_function(void* data);
	};
	export namespace this_thread
	{
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
	thread::priority priority = thread::priority::unset;
};

struct linapi::thread::data_erasor
{
	std::atomic_flag oneside_released = ATOMIC_FLAG_INIT;
	data_erasor() = default;
	virtual ~data_erasor() = default;
};
template< class F, class... Args >
struct linapi::thread::typed_erasor: public data_erasor
{
	template< class F1, class... Args1 >
	typed_erasor(F1&& f, Args1&&... args);
	~typed_erasor() override = default;
	F functor;
	std::tuple< Args... > arguments;
};

template< class F, class... Args >
template< class F1, class... Args1 >
linapi::thread::typed_erasor< F, Args... >::typed_erasor(F1&& f, Args1&&...args):
	functor(std::forward< F1 >(f)),
	arguments(std::forward< Args1 >(args)...)
{}

template< class F, class... Args >
void* linapi::thread::thread_function(void* data)
{
	static_assert(std::invocable< F, Args... >);

	using res = std::invoke_result_t< F, Args... >;
	using data_t = typed_erasor< F, Args... >;
	data_t* typed_data = static_cast< data_t* >(data);
	assert(typed_data != nullptr);
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
	return reinterpret_cast< void* >(static_cast< std::ptrdiff_t >(result));
}
template< class F, class... Args >
inline linapi::thread::thread(F&& f, Args&&... args):
	thread(no_params_tag_t{}, std::forward< F >(f), std::forward< Args >(args)...)
{}
template< class F, class... Args >
linapi::thread::thread(no_params_tag_t, F&& f, Args&&... args)
{
	using data_t = typed_erasor< std::decay_t< F >, std::decay_t< Args >... >;
	std::unique_ptr< data_t > data{new data_t{std::forward< F >(f), std::forward< Args >(args)...}};
	*this = thread{&thread_function< std::decay_t< F >, std::decay_t< Args >... >,
				static_cast< void* >(data.get())};
	erasor_ = data.release();
}
linapi::thread::thread(const initial_params& params, auto&& f, auto&&...args)
{
	using data_t = typed_erasor< std::decay_t< decltype(f) >, std::decay_t< decltype(args) >... >;
	std::unique_ptr< data_t > data{new data_t{std::forward< decltype(f) >(f),
					std::forward< decltype(args) >(args)...}};
	*this = thread{params, &thread_function< std::decay_t< decltype(f) >,
					std::decay_t< decltype(args) >... >,
				static_cast< void* >(data.get())};
	erasor_ = data.release();
}
inline linapi::thread::thread(initial_params& params, auto&& f, auto&&...args):
	thread(static_cast< const initial_params& >(params),
				std::forward< decltype(f) >(f), std::forward< decltype(args) >(args)...)
{}
inline linapi::thread::thread(initial_params&& params, auto&& f, auto&&...args):
	thread(static_cast< const initial_params& >(params),
				std::forward< decltype(f) >(f), std::forward< decltype(args) >(args)...)
{}
inline linapi::thread::thread(const initial_params&& params, auto&& f, auto&&...args):
	thread(params, std::forward< decltype(f) >(f), std::forward< decltype(args) >(args)...)
{}

#endif
