module;
	#ifdef __linux__
		#include <system_error>
		#include <signal.h>
	#endif
module os.linapi;

#ifdef __linux__
void linapi::ipc::signal::register_callback(callback cb, type t)
{
	std::error_code err;
	register_callback(cb, t, err);
	throw_on_error(err, "failed to register callback");
}
bool linapi::ipc::signal::register_callback(callback cb, type t, std::error_code& err) noexcept
{
	sigaction_t description{};
	description.sa_sigaction = cb;
	process_error(linapi::calls::sigaction(t, &description, nullptr), err);
	return static_cast< bool >(err);
}
void linapi::ipc::signal::send(process::group_id process_group_id, type t)
{
	std::error_code err;
	send(process_group_id, t, err);
	throw_on_error(err, "failed to send signal to process group");
}
bool linapi::ipc::signal::send(process::group_id process_group_id, type t, std::error_code& err) noexcept
{
	process_error(linapi::calls::killpg(process_group_id, static_cast< int >(t)) != -1, err);
	return static_cast< bool >(err);
}
#endif
