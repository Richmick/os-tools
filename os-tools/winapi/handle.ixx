export module os.winapi:handle;

#ifdef _WIN32
import std;
import :aliases;

export namespace winapi
{
	class unique_handle;
	class handle_container_base;
	template< class CharT, class Traits >
	std::basic_ostream< CharT, Traits >& operator<<(std::basic_ostream< CharT, Traits >& out,
				const winapi::unique_handle& rhs);
	bool is_blank(handle_t h) noexcept;
	void set_inheritance(inheritmode inherit, handle_t h);
	bool set_inheritance(inheritmode inherit, handle_t h, std::error_code& err) noexcept;
	inheritmode get_inheritance(handle_t h);
	inheritmode get_inheritance(handle_t h, std::error_code& err) noexcept;
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

	void enable_inheritance() const;
	bool enable_inheritance(std::error_code& err) const noexcept;
	void disable_inheritance() const;
	bool disable_inheritance(std::error_code& err) const noexcept;
	inheritmode inheritance() const;
	inheritmode inheritance(std::error_code& err) const noexcept;

	operator bool() const noexcept;

	std::strong_ordering operator<=>(const unique_handle& rhs) const noexcept;
	std::strong_ordering operator<=>(handle_t h) const noexcept;
	bool operator==(const unique_handle& rhs) const noexcept;
	bool operator==(handle_t rhs) const noexcept;
	bool operator==(std::nullptr_t) const noexcept;

private:
	handle_t native_ = nullptr;
};
class winapi::handle_container_base
{
public:
	using native_handle_type = handle_t;

	handle_container_base() noexcept = default;
	handle_container_base(unique_handle&& rhs) noexcept;
	handle_container_base(native_handle_type rhs) noexcept;

	native_handle_type native_handle() const noexcept;
	native_handle_type release_handle() noexcept;
	void close_handle() noexcept;
	operator bool() const noexcept;

	void enable_handle_inheritance() const;
	bool enable_handle_inheritance(std::error_code& err) const noexcept;
	void disable_handle_inheritance() const;
	bool disable_handle_inheritance(std::error_code& err) const noexcept;

	inheritmode handle_inheritance() const;
	inheritmode handle_inheritance(std::error_code& err) const noexcept;

protected:
	unique_handle native_;
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
