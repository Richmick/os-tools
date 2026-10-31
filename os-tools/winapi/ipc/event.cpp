module os.winapi;

#ifdef _WIN32
import std;

winapi::ipc::event::event(make_empty_t) noexcept
{}
winapi::ipc::event::event(native_handle_type h) noexcept:
	native_(h)
{}
winapi::ipc::event::event(unique_handle&& h) noexcept:
	native_(std::move(h))
{}
winapi::ipc::event::event():
	event(inheritmode::none)
{}
winapi::ipc::event::event(inheritmode inherit)
{
	std::error_code err;
	*this = event{inherit, err};
	throw_on_error(err, "failed to create event");
}
winapi::ipc::event::event(const initial_params& params)
{
	std::error_code err;
	*this = event{params, err};
	throw_on_error(err, "failed to create event");
}
winapi::ipc::event::event(std::error_code& err) noexcept:
	event(inheritmode::none, err)
{}
winapi::ipc::event::event(inheritmode inherit, std::error_code& err) noexcept
{
	initial_params params{.inherit_handle = inherit};
	*this = event{params, err};
}
winapi::ipc::event::event(const initial_params& params, std::error_code& err) noexcept
{
	security_attributes_t attributes{
				.nLength = sizeof(security_attributes_t),
				.lpSecurityDescriptor = params.security_descriptor,
				.bInheritHandle = static_cast< bool >(params.inherit_handle)
			};

	native_.reset(winapi::calls::CreateEventA(&attributes, params.manual_reset, false, nullptr));
	if (native_) err.clear();
	else err = winapi::last_error_code();
}

winapi::ipc::event::native_handle_type winapi::ipc::event::native_handle() const noexcept
{
	return native_.get();
}
winapi::ipc::event::native_handle_type winapi::ipc::event::release_handle() noexcept
{
	return native_.release();
}
winapi::ipc::event::operator bool() const noexcept
{
	return native_;
}
void winapi::ipc::event::signal()
{
	std::error_code err;
	signal(err);
	throw_on_error(err, "failed to signal event");
}
void winapi::ipc::event::signal(std::error_code& err) noexcept
{
	if (!winapi::calls::SetEvent(native_.get()))
		err = winapi::last_error_code();
	else err.clear();
}
void winapi::ipc::event::wait()
{
	std::error_code err;
	wait(err);
	throw_on_error(err, "failed to wait for event");
}
void winapi::ipc::event::wait(std::error_code& err) noexcept
{
	if (winapi::calls::WaitForSingleObject(native_.get(), std::numeric_limits< dword_t >::max())
				!= winapi::wait_success_0)
		err = winapi::last_error_code();
	else err.clear();
}
#endif
