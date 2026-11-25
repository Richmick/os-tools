module;
	#include <os-tools/os_macro.hpp>
	#include <os-tools/winapi/deps.hpp>
module os.winapi;

#ifdef _WIN32
import std;

winapi::file_mapping winapi::ipc::create_shared_memory(filemode mode, std::size_t len)
{
	THROW_IF_ERR_PROXY(create_shared_memory, "failed to create anonymous shared memory", mode, len);
}
winapi::file_mapping winapi::ipc::create_shared_memory(filemode mode, std::size_t len, std::error_code& err) noexcept
{
	return create_shared_memory(mode, len, inheritmode::none, err);
}
winapi::file_mapping winapi::ipc::create_shared_memory(filemode mode, std::size_t len, inheritmode inherit)
{
	THROW_IF_ERR_PROXY(create_shared_memory, "failed to create anonymous shared memory", mode, len, inherit);
}
winapi::file_mapping winapi::ipc::create_shared_memory(filemode mode, std::size_t len, inheritmode inherit, std::error_code& err) noexcept
{
	security_attributes_t attributes{
				.nLength = sizeof(security_attributes_t),
				.lpSecurityDescriptor = nullptr,
				.bInheritHandle = static_cast< bool >(inherit)
			};
	dword_t flags = PAGE_READONLY;
	if (mode & filemode::write)
		if (mode & filemode::execute) flags = PAGE_EXECUTE_READWRITE;
		else flags = PAGE_READWRITE;
	else if (mode & filemode::execute) flags = PAGE_EXECUTE_READ;
	dword_t max_size_high = static_cast< dword_t >(len >> 32);
	dword_t max_size_low = static_cast< dword_t >(len);
	unique_handle h{winapi::calls::CreateFileMappingA(INVALID_HANDLE_VALUE, &attributes, flags, max_size_high, max_size_low, nullptr)};
	process_error(h, err);
	return file_mapping{std::move(h)};
}
#endif
