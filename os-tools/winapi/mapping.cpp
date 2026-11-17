module;
	#ifdef _WIN32
		#include <os-tools/winapi/deps.hpp>
		#include <os-tools/os_macro.hpp>
	#endif
module os.winapi;

#if _WIN32
import std;

winapi::mapped_memory winapi::file_mapping::map(filemode mode, std::uint64_t offset, std::size_t length) const
{
	THROW_IF_ERR_PROXY(map, "failed to apply file mapping", mode, offset, length);
}
winapi::mapped_memory winapi::file_mapping::map(filemode mode, std::uint64_t offset, std::size_t length, std::error_code& err) const noexcept
{
	dword_t flags = FILE_MAP_READ;
	if (mode & filemode::write) flags = FILE_MAP_WRITE;
	if (mode & filemode::execute) flags |= FILE_MAP_EXECUTE;
	dword_t high_offset = static_cast< dword_t >(offset >> 32);
	dword_t low_offset = static_cast< dword_t >(offset);
	mapped_memory result{winapi::calls::MapViewOfFile(native_.get(), flags, high_offset, low_offset, length), length};
	return result;
}

winapi::mapped_memory::mapped_memory(mapped_memory&& rhs) noexcept:
	storage_(std::exchange(rhs.storage_, nullptr)),
	length_(std::exchange(rhs.length_, 0))
{}
winapi::mapped_memory::mapped_memory(void* start, std::size_t len) noexcept:
	storage_(start),
	length_(len)
{}
winapi::mapped_memory::~mapped_memory()
{
	std::error_code err; // ignore
	unmap(err);
}
winapi::mapped_memory& winapi::mapped_memory::operator=(mapped_memory&& rhs) noexcept
{
	mapped_memory temp(std::move(*this));
	storage_ = std::exchange(rhs.storage_, nullptr);
	length_ = std::exchange(rhs.length_, 0);
	return *this;
}
bool winapi::mapped_memory::inside(void* ptr) const noexcept
{
	return (ptr >= storage_) &&(ptr < static_cast< char* >(storage_) + length_);
}
void* winapi::mapped_memory::data() const noexcept
{
	return storage_;
}
std::size_t winapi::mapped_memory::size() const noexcept
{
	return length_;
}
winapi::mapped_memory::operator bool() const noexcept
{
	return storage_ != nullptr;
}
void* winapi::mapped_memory::operator[](std::size_t offset) const noexcept
{
	return static_cast< char* >(storage_) + length_;
}
std::pair< void*, std::size_t > winapi::mapped_memory::release() noexcept
{
	return {std::exchange(storage_, nullptr), std::exchange(length_, 0)};
}
void winapi::mapped_memory::unmap()
{
	THROW_IF_ERR0(unmap, "failed to unmap file mapping");
}
bool winapi::mapped_memory::unmap(std::error_code& err) noexcept
{
	if (!*this) return true;
	return process_error(winapi::calls::UnmapViewOfFile(storage_), err);
}
#endif
