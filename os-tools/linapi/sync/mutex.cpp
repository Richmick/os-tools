module os.linapi;

#ifdef __linux__

linapi::mutex::mutex()
{
	linapi::calls::pthread_mutex_init(&native_, nullptr);
}
linapi::mutex::~mutex()
{
	linapi::calls::pthread_mutex_destroy(&native_);
}

linapi::mutex::native_handle_type linapi::mutex::native_handle() & noexcept
{
	return &native_;
}
void linapi::mutex::lock()
{
	linapi::calls::pthread_mutex_lock(&native_);
}
bool linapi::mutex::try_lock()
{
	return linapi::calls::pthread_mutex_trylock(&native_);
}
void linapi::mutex::unlock()
{
	linapi::calls::pthread_mutex_unlock(&native_);
}

linapi::shared_mutex::shared_mutex()
{
	linapi::calls::pthread_rwlock_init(&native_, nullptr);
}
linapi::shared_mutex::~shared_mutex()
{
	linapi::calls::pthread_rwlock_destroy(&native_);
}

linapi::shared_mutex::native_handle_type linapi::shared_mutex::native_handle() & noexcept
{
	return &native_;
}
void linapi::shared_mutex::lock()
{
	linapi::calls::pthread_rwlock_wrlock(&native_);
}
bool linapi::shared_mutex::try_lock()
{
	return linapi::calls::pthread_rwlock_trywrlock(&native_);
}
void linapi::shared_mutex::unlock()
{
	linapi::calls::pthread_rwlock_unlock(&native_);
}
void linapi::shared_mutex::lock_shared()
{
	linapi::calls::pthread_rwlock_rdlock(&native_);
}
bool linapi::shared_mutex::try_lock_shared()
{
	return linapi::calls::pthread_rwlock_tryrdlock(&native_);
}
void linapi::shared_mutex::unlock_shared()
{
	linapi::calls::pthread_rwlock_unlock(&native_);
}

#endif
