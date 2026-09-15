export module os.linapi:handle;

import std;

#ifdef __linux__
import :aliases;

namespace linapi
{
	class unique_handle;
	export template< class CharT, class Traits >
	std::basic_ostream< CharT, Traits >& operator<<(std::basic_ostream< CharT, Traits >& out,
				const linapi::unique_handle& rhs);
	export bool is_blank(handle_t h) noexcept;
}
namespace std
{
	export template<>
	struct hash< linapi::unique_handle >;
	export void swap(linapi::unique_handle& lhs, linapi::unique_handle& rhs) noexcept;
}

class linapi::unique_handle
{
public:
	unique_handle() noexcept = default;
	unique_handle(const unique_handle& rhs) = delete;
	unique_handle(unique_handle&& rhs) noexcept;
	explicit unique_handle(handle_t handle) noexcept;
	unique_handle(std::nullptr_t) noexcept;
	~unique_handle();

	unique_handle& operator=(const unique_handle& rhs) = delete;
	unique_handle& operator=(unique_handle&& rhs) noexcept;

	handle_t get() const noexcept;
	handle_t release() noexcept;
	void reset(handle_t new_handle = -1) noexcept;
	void swap(unique_handle& rhs) noexcept;
	void close() noexcept;

	handle_t* operator&() noexcept;
	operator bool() const noexcept;

	std::strong_ordering operator<=>(const unique_handle& rhs) noexcept;
	std::strong_ordering operator<=>(handle_t h) noexcept;
	bool operator==(std::nullptr_t) noexcept;

private:
	handle_t native_ = -1;
};
template<>
struct std::hash< linapi::unique_handle >: hash< linapi::handle_t >
{
	std::size_t operator()(const linapi::unique_handle& rhs);
};

template< class CharT, class Traits >
std::basic_ostream< CharT,Traits >& linapi::operator<<(std::basic_ostream< CharT,Traits >& out,
			const linapi::unique_handle& rhs)
{
	return out << rhs.get();
}

#endif