module;
	#ifdef _WIN32
		#define WIN32_LEAN_AND_MEAN
		#include <windows.h>
	#endif
export module os.winapi:signal;

#ifdef _WIN32
import :aliases;

export namespace winapi::ipc::signal
{
	using callback = PHANDLER_ROUTINE;
	/// Signal type enum
	enum type: dword_t;

	/// Add callback to process' list
	void register_callback(callback cb);
	/// Remove callback from process' list
	void remove_callback(callback cb);

	/// Add callback to process' list
	bool register_callback(callback cb, std::error_code& err) noexcept;
	/// Remove callback from process' list
	bool remove_callback(callback cb, std::error_code& err) noexcept;

	void send(dword_t process_group_id, type t);
	bool send(dword_t process_group_id, type t, std::error_code& err) noexcept;
}

enum winapi::ipc::signal::type: winapi::dword_t
{
	sigint = CTRL_C_EVENT, ///< Sigint (ctrl+c), sendable
	sigkill = CTRL_BREAK_EVENT, ///< Sigbreak (ctrl+break), sendable
	close = CTRL_CLOSE_EVENT,
	logoff = CTRL_LOGOFF_EVENT,
	shutdown = CTRL_SHUTDOWN_EVENT
};
#endif
