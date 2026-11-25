export module os.linapi:shared_memory;

#ifdef __linux__
import std;
import :aliases;
import :handle;
import :mapping;

export namespace linapi::ipc
{
	file_mapping create_shared_memory(filemode mode, std::size_t len);
	file_mapping create_shared_memory(filemode mode, std::size_t len, std::error_code& err) noexcept;
	file_mapping create_shared_memory(filemode mode, std::size_t len, inheritmode inherit);
	file_mapping create_shared_memory(filemode mode, std::size_t len, inheritmode inherit, std::error_code& err) noexcept;
}
#endif
