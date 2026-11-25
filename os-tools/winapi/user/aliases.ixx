module;
	#include <os-tools/winapi/deps.hpp>
module os.winapi:user_aliases;

#ifdef _WIN32
namespace winapi
{
	using sid_t = SID;
}
#endif
