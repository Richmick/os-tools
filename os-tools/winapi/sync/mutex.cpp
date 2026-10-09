module os.winapi;

#ifdef _WIN32

winapi::mutex::mutex()
{
	winapi::calls::InitializeCriticalSection(&native_);
}
winapi::mutex::mutex(std::size_t spin_count)
{
	winapi::calls::InitializeCriticalSectionAndSpinCount(&native_, static_cast< dword_t >(spin_count));
}
winapi::mutex::~mutex()
{
	winapi::calls::DeleteCriticalSection(&native_);
}

winapi::mutex::native_handle_type winapi::mutex::native_handle() & noexcept
{
	return &native_;
}
void winapi::mutex::lock()
{
	winapi::calls::EnterCriticalSection(&native_);
}
bool winapi::mutex::try_lock()
{
	return winapi::calls::TryEnterCriticalSection(&native_);
}
void winapi::mutex::unlock()
{
	winapi::calls::LeaveCriticalSection(&native_);
}

winapi::shared_mutex::shared_mutex()
{
	winapi::calls::InitializeSRWLock(&native_);
}

winapi::shared_mutex::native_handle_type winapi::shared_mutex::native_handle() & noexcept
{
	return &native_;
}
void winapi::shared_mutex::lock()
{
	winapi::calls::AcquireSRWLockExclusive(&native_);
}
bool winapi::shared_mutex::try_lock()
{
	return winapi::calls::TryAcquireSRWLockExclusive(&native_);
}
void winapi::shared_mutex::unlock()
{
	winapi::calls::ReleaseSRWLockExclusive(&native_);
}
void winapi::shared_mutex::lock_shared()
{
	winapi::calls::AcquireSRWLockShared(&native_);
}
bool winapi::shared_mutex::try_lock_shared()
{
	return winapi::calls::TryAcquireSRWLockShared(&native_);
}
void winapi::shared_mutex::unlock_shared()
{
	winapi::calls::ReleaseSRWLockShared(&native_);
}

#endif
