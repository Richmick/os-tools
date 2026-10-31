#ifndef OS_TOOLS__OS_MACRO_HPP
#define OS_TOOLS__OS_MACRO_HPP

#ifdef _WIN32
	#define OS_STR(str) L##str
	#define EXE_EXTENSION L".exe"
#else
	#define OS_STR(str) str
	#define EXE_EXTENSION ""
#endif

#endif
