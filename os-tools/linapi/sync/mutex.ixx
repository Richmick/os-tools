export module os.linapi:mutex;

#ifdef __linux__
import :aliases;

export namespace linapi
{
	class mutex;
	class shared_mutex;
}

class linapi::mutex
{
public:
	using native_handle_type = pthread_mutex_t*;

	mutex();
	mutex(mutex&&) = delete;
	mutex(const mutex&) = delete;
	~mutex();

	native_handle_type native_handle() & noexcept;

	void lock();
	bool try_lock();
	void unlock();

private:
	pthread_mutex_t native_;
};

class linapi::shared_mutex
{
public:
	using native_handle_type = pthread_rwlock_t*;

	shared_mutex();
	shared_mutex(shared_mutex&&) = delete;
	shared_mutex(const shared_mutex&) = delete;
	~shared_mutex();

	native_handle_type native_handle() & noexcept;

	void lock();
	bool try_lock();
	void unlock();

	void lock_shared();
	bool try_lock_shared();
	void unlock_shared();

private:
	pthread_rwlock_t native_;
};

#endif
