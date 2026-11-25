module os.linapi;

#ifdef __linux__
import std;

linapi::condition_variable::condition_variable()
{
	linapi::calls::pthread_cond_init(&native_, nullptr);
}
linapi::condition_variable::~condition_variable()
{
	linapi::calls::pthread_cond_destroy(&native_);
}
linapi::condition_variable::native_handle_type linapi::condition_variable::native_handle() & noexcept
{
	return &native_;
}
void linapi::condition_variable::wait(unique_lock< mutex >& lock)
{
	linapi::calls::pthread_cond_wait(&native_, lock.mutex()->native_handle());
}
void linapi::condition_variable::notify_one()
{
	linapi::calls::pthread_cond_signal(&native_);
}
void linapi::condition_variable::notify_all()
{
	linapi::calls::pthread_cond_broadcast(&native_);
}
#endif
