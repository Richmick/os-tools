#ifndef OS_TOOLS__WINAPI__DEPS_HPP
#define OS_TOOLS__WINAPI__DEPS_HPP

#ifdef _WIN32
	#include <winsock2.h>
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#endif

#endif
