module os.winapi;

#ifdef _WIN32
import std;

winapi::condition_variable::condition_variable()
{
	winapi::calls::InitializeConditionVariable(&native_);
}
winapi::condition_variable::native_handle_type winapi::condition_variable::native_handle() & noexcept
{
	return &native_;
}
void winapi::condition_variable::wait(unique_lock< mutex >& lock)
{
	if (!winapi::calls::SleepConditionVariableCS(&native_, lock.mutex()->native_handle(),
					std::numeric_limits< dword_t >::max()))
	{
		if (winapi::calls::GetLastError() == wait_timeout) return;
		throw winapi::last_error("failed to wait for condition variable");
	}
}
void winapi::condition_variable::wait(unique_lock< shared_mutex >& lock)
{
	if (!winapi::calls::SleepConditionVariableSRW(&native_, lock.mutex()->native_handle(),
					std::numeric_limits< dword_t >::max(), 0))
	{
		if (winapi::calls::GetLastError() == wait_timeout) return;
		throw winapi::last_error("failed to wait for condition variable");
	}
}
void winapi::condition_variable::wait(shared_lock< shared_mutex >& lock)
{
	if (!winapi::calls::SleepConditionVariableSRW(&native_, lock.mutex()->native_handle(),
					std::numeric_limits< dword_t >::max(), wait_shared_mutex))
	{
		if (winapi::calls::GetLastError() == wait_timeout) return;
		throw winapi::last_error("failed to wait for condition variable");
	}
}
void winapi::condition_variable::notify_one()
{
	winapi::calls::WakeConditionVariable(&native_);
}
void winapi::condition_variable::notify_all()
{
	winapi::calls::WakeAllConditionVariable(&native_);
}
#endif
