module os.linapi;

#ifdef __linux__
import std;

void linapi::ipc::signal::send(process::group_id process_group_id, type t)
{
	std::error_code err;
	send(process_group_id, t, err);
	throw_on_error(err, "failed to remove callback");
}
bool linapi::ipc::signal::send(process::group_id process_group_id, type t, std::error_code& err) noexcept
{
	process_error(linapi::calls::killpg(process_group_id, static_cast< int >(t)) != -1, err);
	return static_cast< bool >(err);
}
#endif
