export module os.linapi:condition_variable;

#ifdef __linux__
import :aliases;
import :mutex;

export namespace linapi
{
	class condition_variable;
}
class linapi::condition_variable
{
public:
	using native_handle_type = pthread_cond_t*;
	condition_variable();
	condition_variable(condition_variable&&) = delete;
	condition_variable(const condition_variable&) = delete;
	~condition_variable();

	native_handle_type native_handle() & noexcept;
	void wait(unique_lock< mutex >& lock);
	void wait(unique_lock< shared_mutex >& lock);
	void wait(shared_lock< shared_mutex >& lock);
	void notify_one();
	void notify_all();

private:
	pthread_cond_t native_;
};
#endif
