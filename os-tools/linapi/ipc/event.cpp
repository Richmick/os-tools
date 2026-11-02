module;
	#ifdef __linux__
		#include <sys/eventfd.h>
	#endif
module os.linapi;

#ifdef __linux__
import std;

linapi::ipc::event::event(make_empty_t) noexcept
{}
linapi::ipc::event::event(native_handle_type h) noexcept:
	native_(h)
{}
linapi::ipc::event::event(unique_handle&& h) noexcept:
	native_(std::move(h))
{}
linapi::ipc::event::event():
	event(inheritmode::none)
{}
linapi::ipc::event::event(inheritmode inherit)
{
	std::error_code err;
	*this = event{inherit, err};
	throw_on_error(err, "failed to create event");
}
linapi::ipc::event::event(const initial_params& params)
{
	std::error_code err;
	*this = event{params, err};
	throw_on_error(err, "failed to create event");
}
linapi::ipc::event::event(std::error_code& err) noexcept:
	event(inheritmode::none, err)
{}
linapi::ipc::event::event(inheritmode inherit, std::error_code& err) noexcept
{
	initial_params params{.inherit_handle = inherit};
	*this = event{params, err};
}
linapi::ipc::event::event(const initial_params& params, std::error_code& err) noexcept
{
	int flags = 0;
	if (!static_cast< bool >(params.inherit_handle)) flags |= EFD_CLOEXEC;
	if (!params.full_reset) flags |= EFD_SEMAPHORE;
	if (params.nonblock) flags |= EFD_NONBLOCK;

	native_.reset(linapi::calls::eventfd(params.initial_value, flags));
	process_error(native_, err);
}

linapi::ipc::event::native_handle_type linapi::ipc::event::native_handle() const noexcept
{
	return native_.get();
}
linapi::ipc::event::native_handle_type linapi::ipc::event::release_handle() noexcept
{
	return native_.release();
}
linapi::ipc::event::operator bool() const noexcept
{
	return native_;
}
void linapi::ipc::event::signal()
{
	std::error_code err;
	signal(err);
	throw_on_error(err, "failed to signal event");
}
void linapi::ipc::event::signal(std::error_code& err) noexcept
{
	std::uint64_t value = 1;
	if (linapi::calls::write(native_.get(), &value, sizeof(value)) < 0)
		err = linapi::last_error_code();
	else err.clear();
}
std::uint64_t linapi::ipc::event::wait()
{
	std::error_code err;
	std::uint64_t res = wait(err);
	throw_on_error(err, "failed to wait for event");
	return res;
}
std::uint64_t linapi::ipc::event::wait(std::error_code& err) noexcept
{
	std::uint64_t value = 1;
	if (linapi::calls::read(native_.get(), &value, sizeof(value)) >= 0)
		err.clear();
	else
	{
		std::error_code error = linapi::last_error_code();
		if (error == std::errc::resource_unavailable_try_again) return false;
		err = error;
	}
	return value;
}
#endif
