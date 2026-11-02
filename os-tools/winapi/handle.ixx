export module os.winapi:handle;

#ifdef _WIN32
import std;
import :aliases;

export namespace winapi
{
	class unique_handle;
	template< class CharT, class Traits >
	std::basic_ostream< CharT, Traits >& operator<<(std::basic_ostream< CharT, Traits >& out,
				const winapi::unique_handle& rhs);
	bool is_blank(handle_t h) noexcept;
	void set_inheritance(inheritmode inherit, handle_t h);
	bool set_inheritance(inheritmode inherit, handle_t h, std::error_code& err) noexcept;
}
namespace std
{
	export template<>
	struct hash< winapi::unique_handle >;
	export void swap(winapi::unique_handle& lhs, winapi::unique_handle& rhs) noexcept;
}

class winapi::unique_handle
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
	void reset(handle_t new_handle = nullptr) noexcept;
	void swap(unique_handle& rhs) noexcept;
	void close() noexcept;

	operator bool() const noexcept;

	std::strong_ordering operator<=>(const unique_handle& rhs) const noexcept;
	std::strong_ordering operator<=>(handle_t h) const noexcept;
	bool operator==(const unique_handle& rhs) const noexcept;
	bool operator==(handle_t rhs) const noexcept;
	bool operator==(std::nullptr_t) const noexcept;

private:
	handle_t native_ = nullptr;
};
template<>
struct std::hash< winapi::unique_handle >: hash< winapi::handle_t >
{
	std::size_t operator()(const winapi::unique_handle& rhs);
};

template< class CharT, class Traits >
std::basic_ostream< CharT,Traits >& winapi::operator<<(std::basic_ostream< CharT,Traits >& out,
			const winapi::unique_handle& rhs)
{
	return out << rhs.get();
}
#endif
