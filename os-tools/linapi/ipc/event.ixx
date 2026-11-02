export module os.linapi:event;

#ifdef __linux__
import :aliases;
import :handle;

export namespace linapi::ipc
{
	class event;
}
class linapi::ipc::event
{
public:
	using native_handle_type = handle_t;
	struct initial_params;

	explicit event();
	explicit event(make_empty_t) noexcept;
	explicit event(inheritmode inherit);
	explicit event(const initial_params& params);

	explicit event(std::error_code& err) noexcept;
	explicit event(const initial_params& params, std::error_code& err) noexcept;
	explicit event(inheritmode inherit, std::error_code& err) noexcept;
	explicit event(native_handle_type h) noexcept;
	explicit event(unique_handle&& h) noexcept;

	native_handle_type native_handle() const noexcept;
	native_handle_type release_handle() noexcept;
	operator bool() const noexcept;

	void signal();
	void signal(std::error_code& err) noexcept;
	std::uint64_t wait(); // if nonblock return false (0)
	std::uint64_t wait(std::error_code& err) noexcept;

private:
	unique_handle native_;
};
struct linapi::ipc::event::initial_params
{
	inheritmode inherit_handle;
	bool full_reset = false;
	bool nonblock = false;
	unsigned initial_value = 0;
};
#endif
