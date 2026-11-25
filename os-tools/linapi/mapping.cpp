module;
	#if __linux__
		#include <sys/mman.h>
		#include <os-tools/os_macro.hpp>
	#endif
module os.linapi;

#if __linux__
import std;

linapi::mapped_memory linapi::file_mapping::map(filemode mode, std::uint64_t offset, std::size_t length) const
{
	THROW_IF_ERR_PROXY(map, "failed to apply file mapping", mode, offset, length);
}
linapi::mapped_memory linapi::file_mapping::map(filemode mode, std::uint64_t offset, std::size_t length, std::error_code& err) const noexcept
{
	mapped_memory result{linapi::calls::mmap(nullptr, length, mode, MAP_SHARED, native_.get(), offset), length};
	process_error(result.data() != nullptr, err);
	return result;
}

linapi::mapped_memory::mapped_memory(mapped_memory&& rhs) noexcept:
	storage_(std::exchange(rhs.storage_, nullptr)),
	length_(std::exchange(rhs.length_, 0))
{}
linapi::mapped_memory::mapped_memory(void* start, std::size_t len) noexcept:
	storage_(start),
	length_(len)
{}
linapi::mapped_memory::~mapped_memory()
{
	std::error_code err; // ignore
	unmap(err);
}
linapi::mapped_memory& linapi::mapped_memory::operator=(mapped_memory&& rhs) noexcept
{
	mapped_memory temp(std::move(*this));
	storage_ = std::exchange(rhs.storage_, nullptr);
	length_ = std::exchange(rhs.length_, 0);
	return *this;
}
bool linapi::mapped_memory::inside(void* ptr) const noexcept
{
	return (ptr >= storage_) &&(ptr < static_cast< char* >(storage_) + length_);
}
void* linapi::mapped_memory::data() const noexcept
{
	return storage_;
}
std::size_t linapi::mapped_memory::size() const noexcept
{
	return length_;
}
linapi::mapped_memory::operator bool() const noexcept
{
	return storage_ != nullptr;
}
void* linapi::mapped_memory::operator[](std::size_t offset) const noexcept
{
	return static_cast< char* >(storage_) + offset;
}
std::pair< void*, std::size_t > linapi::mapped_memory::release() noexcept
{
	return {std::exchange(storage_, nullptr), std::exchange(length_, 0)};
}
void linapi::mapped_memory::unmap()
{
	THROW_IF_ERR0(unmap, "failed to unmap file mapping");
}
bool linapi::mapped_memory::unmap(std::error_code& err) noexcept
{
	if (!*this) return true;
	return process_error(linapi::calls::munmap(storage_, length_) == 0, err);
}
#endif
