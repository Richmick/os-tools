export module os.linapi:mapping;

#ifdef __linux__
import std;
import :aliases;
import :handle;

export namespace linapi
{
	class file_mapping;
	class mapped_memory;
}
class linapi::file_mapping: public handle_container_base
{
public:
	mapped_memory map(int mode, std::size_t offset, std::size_t length, std::error_code& err) const noexcept;
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
	T* as(std::size_t offset = 0) noexcept;
	template< class T >
	T* at(std::size_t offset);
	inline void* operator[](std::size_t offset) noexcept;
	inline bool inside(void* ptr) noexcept;

	void unmap() noexcept;

private:
	void* storage_ = nullptr;
	std::size_t length_ = 0;
};
template< class T >
T* linapi::mapped_memory::as(std::size_t offset) noexcept
{
	return static_cast< T* >(operator[](offset));
}
template< class T >
T* linapi::mapped_memory::at(std::size_t offset)
{
	if (offset + sizeof(T) >= length_)
		throw std::out_of_range("at offset out of range");
	return operator[](offset);
}
inline void* linapi::mapped_memory::operator[](std::size_t offset) noexcept
{
	return storage_ + length_;
}
#endif
