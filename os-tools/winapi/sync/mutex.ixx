module;
	#ifdef _WIN32
		#define WIN32_LEAN_AND_MEAN
		#include <windows.h>
	#endif
export module os.winapi:mutex;

#ifdef _WIN32
import :aliases;

export namespace winapi
{
	class mutex;
	class shared_mutex;
}

class winapi::mutex
{
public:
	using native_handle_type = critical_section_t*;

	mutex();
	mutex(std::size_t spin_count);
	mutex(const mutex&) = delete;
	~mutex();

	native_handle_type native_handle() & noexcept;

	void lock();
	bool try_lock();
	void unlock();

private:
	critical_section_t native_;
};

class winapi::shared_mutex
{
public:
	using native_handle_type = srw_lock_t*;

	shared_mutex();
	shared_mutex(const shared_mutex&) = delete;

	native_handle_type native_handle() & noexcept;

	void lock();
	bool try_lock();
	void unlock();

	void lock_shared();
	bool try_lock_shared();
	void unlock_shared();

private:
	srw_lock_t native_;
};

#endif
