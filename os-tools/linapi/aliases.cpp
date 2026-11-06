module;
	#include <cerrno>
	#include <string>
	#include <string_view>
	#include <system_error>
module os.linapi;

#ifdef __linux__
linapi::string linapi::to_string(handle_t h)
{
	return std::to_string(h);
}
linapi::handle_t linapi::to_handle(const string& h)
{
	return static_cast< handle_t >(std::stoi(h));
}
linapi::handle_t linapi::to_handle(const string_view& h)
{
	return to_handle(string{h});
}

std::error_code linapi::last_error_code()
{
	return std::error_code{static_cast< int >(errno), std::system_category()};
}
std::system_error linapi::last_error(const char* msg)
{
	return {last_error_code(), msg};
}
std::system_error linapi::last_error(const std::string& msg)
{
	return {last_error_code(), msg};
}
void linapi::throw_on_error(std::error_code err, const char* msg)
{
	if (err) throw std::system_error(err, msg);
}
void linapi::throw_on_error(std::error_code err, const std::string& msg)
{
	if (err) throw std::system_error(err, msg);
}
void linapi::process_error(bool success, std::error_code& err) noexcept
{
	if (success) err.clear();
	else err = last_error_code();
}
linapi::handle_t linapi::process_error(handle_t res, std::error_code& err) noexcept
{
	if (!linapi::is_blank(res)) err.clear();
	else err = last_error_code();
	return res;
}

#endif
