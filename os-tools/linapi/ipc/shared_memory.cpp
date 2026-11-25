module;
	#ifdef __linux__
		#include <sys/mman.h>
		#include <os-tools/os_macro.hpp>
	#endif
module os.linapi;

#ifdef __linux__
import std;

linapi::file_mapping linapi::ipc::create_shared_memory(filemode mode, std::size_t len)
{
	THROW_IF_ERR_PROXY(create_shared_memory, "failed to create anonymous shared memory", mode, len);
}
linapi::file_mapping linapi::ipc::create_shared_memory(filemode mode, std::size_t len, std::error_code& err) noexcept
{
	return create_shared_memory(mode, len, inheritmode::none, err);
}
linapi::file_mapping linapi::ipc::create_shared_memory(filemode mode, std::size_t len, inheritmode inherit)
{
	THROW_IF_ERR_PROXY(create_shared_memory, "failed to create anonymous shared memory", mode, len, inherit);
}
linapi::file_mapping linapi::ipc::create_shared_memory(filemode mode, std::size_t len, inheritmode inherit, std::error_code& err) noexcept
{
	unique_handle h{linapi::calls::memfd_create("", (inherit == inheritmode::all ? 0 : MFD_CLOEXEC))};
	if (process_error(h, err))
	{
		process_error(linapi::calls::ftruncate(h.get(), len) == 0, err);
	}
	return file_mapping{std::move(h)};
}
#endif
