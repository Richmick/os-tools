#ifndef OS_TOOLS__OS_MACRO_HPP
#define OS_TOOLS__OS_MACRO_HPP

#ifdef _WIN32
	#define OS_STR(str) L##str
	#define EXE_EXTENSION L".exe"
#else
	#define OS_STR(str) str
	#define EXE_EXTENSION ""
#endif

#define THROW_IF_ERR(func, msg, ...) \
	std::error_code err; \
	func(__VA_ARGS__, err); \
	if (err) throw std::system_error{err, msg}
#define THROW_IF_ERR_CTOR(type, msg, ...) \
	std::error_code err; \
	*this = type{__VA_ARGS__, err}; \
	if (err) throw std::system_error{err, msg}
#define THROW_IF_ERR_PROXY(func, msg, ...) \
	std::error_code err; \
	auto result = func(__VA_ARGS__, err); \
	if (err) throw std::system_error{err, msg}; \
	return result
#define THROW_IF_ERR_PROXY0(func, msg) \
	std::error_code err; \
	auto result = func(err); \
	if (err) throw std::system_error{err, msg}; \
	return result

#endif
