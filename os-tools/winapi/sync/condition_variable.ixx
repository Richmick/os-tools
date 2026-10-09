module;
	#ifdef _WIN32
		#define WIN32_LEAN_AND_MEAN
		#include <windows.h>
	#endif
export module os.winapi:condition_variable;

#ifdef _WIN32
import :aliases;
import :mutex;

export namespace winapi
{
	class condition_variable;
}
class winapi::condition_variable
{
public:
	using native_handle_type = condition_variable_t*;
	condition_variable();
	condition_variable(const condition_variable&) = delete;

	native_handle_type native_handle() & noexcept;
	void wait(unique_lock< mutex >& lock);
	void wait(unique_lock< shared_mutex >& lock);
	void wait(shared_lock< shared_mutex >& lock);
	void notify_one();
	void notify_all();
private:
	static constexpr inline dword_t wait_timeout = ERROR_TIMEOUT;
	static constexpr inline dword_t wait_shared_mutex = CONDITION_VARIABLE_LOCKMODE_SHARED;

	condition_variable_t native_;
};
#endif
