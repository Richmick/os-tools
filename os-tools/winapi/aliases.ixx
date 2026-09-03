module;
	#ifdef _WIN32
		#define WIN32_LEAN_AND_MEAN
		#include <windows.h>
	#endif
export module os.winapi:aliases;

#ifdef _WIN32

import std;

namespace winapi
{
	export using handle_t = HANDLE;
	export using dword_t = DWORD;

	// Handles
	export using ::CloseHandle;
	export using ::SetHandleInformation;
	export using ::GetStdHandle;

	// Others
	export using ::GetLastError;
	export using ::WaitForSingleObject;
	export using ::WaitForMultipleObjects;

	export std::system_error last_error(const char* msg)
	{
		return {static_cast< int >(GetLastError()), std::system_category(), msg};
	}
	export std::system_error last_error(const std::string& msg)
	{
		return {static_cast< int >(GetLastError()), std::system_category(), msg};
	}
}

#endif
