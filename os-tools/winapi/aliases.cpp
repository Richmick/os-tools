module os.winapi;

std::system_error winapi::last_error(const char* msg)
{
	return {static_cast< int >(winapi::GetLastError()), std::system_category(), msg};
}
std::system_error winapi::last_error(const std::string& msg)
{
	return {static_cast< int >(winapi::GetLastError()), std::system_category(), msg};
}
void winapi::wait(handle_t h)
{
	dword_t res = winapi::WaitForSingleObject(h, static_cast< dword_t >(-1));
	if (res != wait_success_0)
	{
		throw winapi::last_error("failed to wait for handle");
	}
}
bool winapi::wait(handle_t h, std::chrono::milliseconds dur)
{
	dword_t res = winapi::WaitForSingleObject(h, static_cast< dword_t >(dur.count()));
	if (res == wait_success_0) return true;
	if (res == wait_timeout) return false;
	throw winapi::last_error("failed to wait for handle");
}
