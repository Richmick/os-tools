#ifndef OS_TOOLS__LINAPI__FEATURE_TEST
#define OS_TOOLS__LINAPI__FEATURE_TEST

#ifdef __linux__
#include <unistd.h>

import std;
import os.utils;

/// Fallback for testing
void closefrom(os::dangling_struct) = delete;

namespace linapi::details
{
	template< class First, class = decltype(::closefrom(std::declval< First >())) >
	struct closefrom_check_template
	{};
}
#endif
#endif
