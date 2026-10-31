module os.winapi;

#ifdef _WIN32
import std;

winapi::string winapi::to_string(handle_t h)
{
	return to_string(reinterpret_cast< int_handle_t >(h));
}
winapi::string winapi::to_string(int_handle_t h)
{
	return std::to_wstring(h);
}
winapi::handle_t winapi::to_handle(const string& h)
{
	return reinterpret_cast< handle_t >(std::stoull(h));
}
winapi::handle_t winapi::to_handle(const string_view& h)
{
	return to_handle(string{h});
}
winapi::handle_t winapi::to_handle(const std::string& h)
{
	return reinterpret_cast< handle_t >(std::stoull(h));
}
winapi::handle_t winapi::to_handle(const std::string_view& h)
{
	return to_handle(std::string{h});
}

std::error_code winapi::last_error_code()
{
	return std::error_code{static_cast< int >(winapi::calls::GetLastError()), std::system_category()};
}
std::system_error winapi::last_error(const char* msg)
{
	return {last_error_code(), msg};
}
std::system_error winapi::last_error(const std::string& msg)
{
	return {last_error_code(), msg};
}
void winapi::throw_on_error(std::error_code err, const char* msg)
{
	if (err) throw std::system_error(err, msg);
}
void winapi::throw_on_error(std::error_code err, const std::string& msg)
{
	if (err) throw std::system_error(err, msg);
}

void winapi::wait(handle_t h)
{
	std::error_code err;
	wait(h, err);
	throw_on_error(err, "failed to wait for handle");
}
bool winapi::wait(handle_t h, std::error_code& err) noexcept
{
	dword_t res = winapi::calls::WaitForSingleObject(h, static_cast< dword_t >(-1));
	if (res != wait_success_0)
	{
		err = winapi::last_error_code();
		return false;
	}
	err.clear();
	return false;
}
bool winapi::wait(handle_t h, std::chrono::milliseconds dur)
{
	std::error_code err;
	bool res = wait(h, dur, err);
	throw_on_error(err, "failed to wait for handle");
	return res;
}
bool winapi::wait(handle_t h, std::chrono::milliseconds dur, std::error_code& err) noexcept
{
	dword_t res = winapi::calls::WaitForSingleObject(h, static_cast< dword_t >(dur.count()));
	err.clear();
	if (res == wait_success_0) return true;
	if (res == wait_timeout) return false;
	err = winapi::last_error_code();
	return false;
}

#endif
