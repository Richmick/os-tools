module;
	#ifdef __linux__
		#include <sys/mman.h>
	#endif
export module os.linapi:mapping;

#ifdef __linux__
import std;
import :aliases;
import :handle;

export namespace linapi
{
	enum filemode
	{
		read = PROT_READ, write = PROT_WRITE, execute = PROT_EXEC
	};
	class file_mapping;
	class mapped_memory;
}
class linapi::file_mapping: public handle_container_base
{
public:
	using handle_container_base::handle_container_base;

	mapped_memory map(filemode mode, std::uint64_t offset, std::size_t length) const;
	mapped_memory map(filemode mode, std::uint64_t offset, std::size_t length, std::error_code& err) const noexcept;
};
class linapi::mapped_memory
{
public:
	mapped_memory() noexcept = default;
	mapped_memory(mapped_memory&& rhs) noexcept;
	mapped_memory(const mapped_memory&) = delete;
	mapped_memory(void* start, std::size_t len) noexcept;
	~mapped_memory();
	mapped_memory& operator=(mapped_memory&& rhs) noexcept;
	mapped_memory& operator=(const mapped_memory&) = delete;

	template< class T >
	T* as(std::size_t offset = 0) const noexcept;
	template< class T >
	T* at(std::size_t offset) const;
	void* operator[](std::size_t offset) const noexcept;
	bool inside(void* ptr) const noexcept;

	void* data() const noexcept;
	std::size_t size() const noexcept;
	operator bool() const noexcept;

	void unmap();
	bool unmap(std::error_code& err) noexcept;
	std::pair< void*, std::size_t > release() noexcept;

private:
	void* storage_ = nullptr;
	std::size_t length_ = 0;
};
template< class T >
T* linapi::mapped_memory::as(std::size_t offset) const noexcept
{
	return static_cast< T* >(operator[](offset));
}
template< class T >
T* linapi::mapped_memory::at(std::size_t offset) const
{
	if (offset + sizeof(T) >= length_)
		throw std::out_of_range("at offset out of range");
	return operator[](offset);
}
#endif
