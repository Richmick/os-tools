export module os.winapi:event;

#ifdef _WIN32
import :aliases;
import :handle;

export namespace winapi::ipc
{
	class event;
}
class winapi::ipc::event
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
	void wait();
	void wait(std::error_code& err) noexcept;

private:
	unique_handle native_;
};
struct winapi::ipc::event::initial_params
{
	void* security_descriptor = nullptr;
	inheritmode inherit_handle;
	bool manual_reset = false;
};
#endif
