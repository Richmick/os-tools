module;
	#include <cerrno>
module os.linapi;

#ifdef __linux__
import std;

std::system_error linapi::last_error(const char* msg)
{
	return {static_cast< int >(errno), std::system_category(), msg};
}
std::system_error linapi::last_error(const std::string& msg)
{
	return {static_cast< int >(errno), std::system_category(), msg};
}

#endif
