export module os.winapi:shared_memory;

#ifdef _WIN32
import std;
import :aliases;
import :handle;
import :mapping;

export namespace winapi::ipc
{
	mapped_memory create_shared_memory(filemode mode, std::size_t len);
	mapped_memory create_shared_memory(filemode mode, std::size_t len, std::error_code& err) noexcept;
}
#endif
