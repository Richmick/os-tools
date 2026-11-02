module os.winapi;

#ifdef _WIN32
import std;

void winapi::ipc::signal::register_callback(callback cb)
{
	std::error_code err;
	register_callback(cb, err);
	throw_on_error(err, "failed to register callback");
}
bool winapi::ipc::signal::register_callback(callback cb, std::error_code& err) noexcept
{
	if (winapi::calls::SetConsoleCtrlHandler(cb, true))
	{
		err = winapi::last_error_code();
		return false;
	}
	err.clear();
	return true;
}
void winapi::ipc::signal::remove_callback(callback cb)
{
	std::error_code err;
	remove_callback(cb, err);
	throw_on_error(err, "failed to remove callback");
}
bool winapi::ipc::signal::remove_callback(callback cb, std::error_code& err) noexcept
{
	if (winapi::calls::SetConsoleCtrlHandler(cb, false))
	{
		err = winapi::last_error_code();
		return false;
	}
	err.clear();
	return true;
}
void winapi::ipc::signal::send(dword_t process_group_id, type t)
{
	std::error_code err;
	send(process_group_id, t, err);
	throw_on_error(err, "failed to remove callback");
}
bool winapi::ipc::signal::send(dword_t process_group_id, type t, std::error_code& err) noexcept
{
	process_error(winapi::calls::GenerateConsoleCtrlEvent(static_cast< dword_t >(t), process_group_id), err);
	return static_cast< bool >(err);
}
#endif
