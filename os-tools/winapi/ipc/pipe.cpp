module;
	#include <os-tools/winapi/deps.hpp>
module os.winapi;

#ifdef _WIN32
import std;

std::pair< winapi::ipc::pipe, winapi::ipc::pipe > winapi::ipc::create_pipe()
{
	return create_pipe(inheritmode::none);
}
std::pair< winapi::ipc::pipe, winapi::ipc::pipe >
		winapi::ipc::create_pipe(std::error_code& err) noexcept
{
	return create_pipe(inheritmode::none, err);
}
std::pair< winapi::ipc::pipe, winapi::ipc::pipe >
		winapi::ipc::create_pipe(inheritmode inherit)
{
	pipe::initial_params params{.inherit = inherit};
	return create_pipe(params);
}
std::pair< winapi::ipc::pipe, winapi::ipc::pipe >
		winapi::ipc::create_pipe(inheritmode inherit, std::error_code& err) noexcept
{
	pipe::initial_params params{.inherit = inherit};
	return create_pipe(params, err);
}
std::pair< winapi::ipc::pipe, winapi::ipc::pipe >
		winapi::ipc::create_pipe(const pipe::initial_params& params)
{
	std::error_code err;
	std::pair< pipe, pipe > result = create_pipe(params, err);
	throw_on_error(err, "failed to create anonymous pipe");
	return result;
}
std::pair< winapi::ipc::pipe, winapi::ipc::pipe >
		winapi::ipc::create_pipe(const pipe::initial_params& params, std::error_code& err) noexcept
{
	security_attributes_t attributes{
				.nLength = sizeof(security_attributes_t),
				.lpSecurityDescriptor = params.security_descriptor,
				.bInheritHandle = static_cast< bool >(params.inherit)
			};
	handle_t read, write;
	if (!winapi::calls::CreatePipe(&read, &write, &attributes, static_cast< dword_t >(params.size_hint)))
	{
		err = winapi::last_error_code();
		return {};
	}
	err.clear();
	return {pipe{read}, pipe{write}};
}

void winapi::ipc::pipe::write(std::string_view data)
{
	write(std::as_bytes(std::span< const char >(data.data(), data.size())));
}
std::size_t winapi::ipc::pipe::write_once(std::string_view data)
{
	return write_once(std::as_bytes(std::span< const char >(data.data(), data.size())));;
}
std::size_t winapi::ipc::pipe::write(std::string_view data, std::error_code& err) noexcept
{
	return write(std::as_bytes(std::span< const char >(data.data(), data.size())), err);
}
std::size_t winapi::ipc::pipe::write_once(std::string_view data, std::error_code& err) noexcept
{
	return write_once(std::as_bytes(std::span< const char >(data.data(), data.size())), err);;
}


void winapi::ipc::named_pipe::flush()
{
	std::error_code err;
	flush(err);
	throw_on_error(err, "failed to connect named pipe");
}
bool winapi::ipc::named_pipe::flush(std::error_code& err) noexcept
{
	process_error(winapi::calls::FlushFileBuffers(native_.get()), err);
	return static_cast< bool >(err);
}
void winapi::ipc::named_pipe::connect()
{
	std::error_code err;
	connect(err);
	throw_on_error(err, "failed to connect named pipe");
}
bool winapi::ipc::named_pipe::connect(std::error_code& err) noexcept
{
	process_error(winapi::calls::ConnectNamedPipe(native_.get(), nullptr), err);
	if (err.value() == ERROR_PIPE_CONNECTED) err.clear();
	return static_cast< bool >(err);
}

winapi::ipc::named_pipe winapi::ipc::create_server_pipe(string name, openmode mode)
{
	return create_server_pipe(named_pipe::initial_params{.mode = mode}, std::move(name));
}
winapi::ipc::named_pipe winapi::ipc::create_server_pipe(string name, openmode mode,
			std::error_code& err) noexcept
{
	return create_server_pipe(named_pipe::initial_params{.mode = mode}, std::move(name), err);
}
winapi::ipc::named_pipe winapi::ipc::create_server_pipe(string name, openmode mode, inheritmode inherit)
{
	return create_server_pipe(named_pipe::initial_params{.mode = mode, .inherit = inherit},
				std::move(name));
}
winapi::ipc::named_pipe winapi::ipc::create_server_pipe(string name, openmode mode, inheritmode inherit,
			std::error_code& err) noexcept
{
	return create_server_pipe(named_pipe::initial_params{.mode = mode, .inherit = inherit},
				std::move(name), err);
}
winapi::ipc::named_pipe winapi::ipc::create_server_pipe(const named_pipe::initial_params& params, string name)
{
	std::error_code err;
	named_pipe result = create_server_pipe(params, std::move(name), err);
	throw_on_error(err, "failed to create named pipe");
	return result;
}
winapi::ipc::named_pipe winapi::ipc::create_server_pipe(const named_pipe::initial_params& params, string name,
			std::error_code& err) noexcept
{
	dword_t mode = 0;
	switch (params.mode)
	{
	case openmode::in:
		mode = PIPE_ACCESS_INBOUND;
		break;
	case openmode::out:
		mode = PIPE_ACCESS_OUTBOUND;
		break;
	case openmode::inout:
		mode = PIPE_ACCESS_DUPLEX;
		break;
	}
	if (params.prevent_open_existing) mode |= FILE_FLAG_FIRST_PIPE_INSTANCE;
	if (params.overlapped) mode |= FILE_FLAG_OVERLAPPED;

	dword_t packetmode = 0;
	if (params.packet) packetmode = PIPE_TYPE_MESSAGE;
	if (params.packet && !params.read_bytes) packetmode |= PIPE_READMODE_MESSAGE;
	if (params.nonblock) packetmode |= PIPE_NOWAIT;

	const dword_t max_instances = 1;
	security_attributes_t attributes{
				.nLength = sizeof(security_attributes_t),
				.lpSecurityDescriptor = params.security_descriptor,
				.bInheritHandle = static_cast< bool >(params.inherit)
			};

	name.insert(0, L"\\\\.\\pipe\\LOCAL\\");
	unique_handle h{winapi::calls::CreateNamedPipeW(name.c_str(), mode, packetmode, max_instances,
					params.write_size_hint, params.read_size_hint, params.default_timeout.count(),
					&attributes)};
	process_error(h, err);
	return {std::move(h)};
}

winapi::ipc::named_pipe winapi::ipc::open_client_pipe(string name, openmode mode)
{
	return open_client_pipe(named_pipe::open_params{.mode = mode}, std::move(name));
}
winapi::ipc::named_pipe winapi::ipc::open_client_pipe(string name, openmode mode,
			std::error_code& err) noexcept
{
	return open_client_pipe(named_pipe::open_params{.mode = mode}, std::move(name), err);
}
winapi::ipc::named_pipe winapi::ipc::open_client_pipe(string name, openmode mode, inheritmode inherit)
{
	return open_client_pipe(named_pipe::open_params{.mode = mode, .inherit = inherit},
				std::move(name));
}
winapi::ipc::named_pipe winapi::ipc::open_client_pipe(string name, openmode mode, inheritmode inherit,
			std::error_code& err) noexcept
{
	return open_client_pipe(named_pipe::open_params{.mode = mode, .inherit = inherit},
				std::move(name), err);
}
winapi::ipc::named_pipe winapi::ipc::open_client_pipe(const named_pipe::open_params& params, string name)
{
	std::error_code err;
	named_pipe result = open_client_pipe(params, std::move(name), err);
	throw_on_error(err, "failed to open named pipe");
	return result;
}
winapi::ipc::named_pipe winapi::ipc::open_client_pipe(const named_pipe::open_params& params, string name,
			std::error_code& err) noexcept
{
	dword_t mode = 0;
	switch (params.mode)
	{
	case openmode::in:
		mode = GENERIC_READ;
		break;
	case openmode::out:
		mode = GENERIC_WRITE;
		break;
	case openmode::inout:
		mode = GENERIC_READ | GENERIC_WRITE;
		break;
	}
	dword_t attr_flags = (params.overlapped ? FILE_FLAG_OVERLAPPED : FILE_ATTRIBUTE_NORMAL);
	if (params.overlapped) mode |= FILE_FLAG_OVERLAPPED;

	dword_t packetmode = 0;
	if (params.packet) packetmode |= PIPE_READMODE_MESSAGE;
	if (params.nonblock) packetmode |= PIPE_NOWAIT;

	security_attributes_t attributes{
				.nLength = sizeof(security_attributes_t),
				.lpSecurityDescriptor = params.security_descriptor,
				.bInheritHandle = static_cast< bool >(params.inherit)
			};

	name.insert(0, L"\\\\.\\pipe\\LOCAL\\");
	unique_handle h{winapi::calls::CreateFileW(name.c_str(), mode, 0, &attributes, OPEN_EXISTING,
					attr_flags, nullptr)};
	process_error(h, err);
	if (h && (packetmode != 0))
	{
		process_error(winapi::calls::SetNamedPipeHandleState(h.get(), &packetmode, nullptr, nullptr), err);
	}
	return {std::move(h)};
}

winapi::ipc::named_pipe winapi::ipc::create_server_pipe(std::string name, openmode mode)
{
	return create_server_pipe(named_pipe::initial_params{.mode = mode}, std::move(name));
}
winapi::ipc::named_pipe winapi::ipc::create_server_pipe(std::string name, openmode mode,
			std::error_code& err) noexcept
{
	return create_server_pipe(named_pipe::initial_params{.mode = mode}, std::move(name), err);
}
winapi::ipc::named_pipe winapi::ipc::create_server_pipe(std::string name, openmode mode, inheritmode inherit)
{
	return create_server_pipe(named_pipe::initial_params{.mode = mode, .inherit = inherit},
				std::move(name));
}
winapi::ipc::named_pipe winapi::ipc::create_server_pipe(std::string name, openmode mode, inheritmode inherit,
			std::error_code& err) noexcept
{
	return create_server_pipe(named_pipe::initial_params{.mode = mode, .inherit = inherit},
				std::move(name), err);
}
winapi::ipc::named_pipe winapi::ipc::create_server_pipe(const named_pipe::initial_params& params, std::string name)
{
	std::error_code err;
	named_pipe result = create_server_pipe(params, std::move(name), err);
	throw_on_error(err, "failed to create named pipe");
	return result;
}
winapi::ipc::named_pipe winapi::ipc::create_server_pipe(const named_pipe::initial_params& params, std::string name,
			std::error_code& err) noexcept
{
	dword_t mode = 0;
	switch (params.mode)
	{
	case openmode::in:
		mode = PIPE_ACCESS_INBOUND;
		break;
	case openmode::out:
		mode = PIPE_ACCESS_OUTBOUND;
		break;
	case openmode::inout:
		mode = PIPE_ACCESS_DUPLEX;
		break;
	}
	if (params.prevent_open_existing) mode |= FILE_FLAG_FIRST_PIPE_INSTANCE;
	if (params.overlapped) mode |= FILE_FLAG_OVERLAPPED;

	dword_t packetmode = 0;
	if (params.packet) packetmode = PIPE_TYPE_MESSAGE;
	if (params.packet && !params.read_bytes) packetmode |= PIPE_READMODE_MESSAGE;
	if (params.nonblock) packetmode |= PIPE_NOWAIT;

	const dword_t max_instances = 1;
	security_attributes_t attributes{
				.nLength = sizeof(security_attributes_t),
				.lpSecurityDescriptor = params.security_descriptor,
				.bInheritHandle = static_cast< bool >(params.inherit)
			};

	name.insert(0, "\\\\.\\pipe\\LOCAL\\");
	unique_handle h{winapi::calls::CreateNamedPipeA(name.c_str(), mode, packetmode, max_instances,
					params.write_size_hint, params.read_size_hint, params.default_timeout.count(),
					&attributes)};
	process_error(h, err);
	return {std::move(h)};
}

winapi::ipc::named_pipe winapi::ipc::open_client_pipe(std::string name, openmode mode)
{
	return open_client_pipe(named_pipe::open_params{.mode = mode}, std::move(name));
}
winapi::ipc::named_pipe winapi::ipc::open_client_pipe(std::string name, openmode mode,
			std::error_code& err) noexcept
{
	return open_client_pipe(named_pipe::open_params{.mode = mode}, std::move(name), err);
}
winapi::ipc::named_pipe winapi::ipc::open_client_pipe(std::string name, openmode mode, inheritmode inherit)
{
	return open_client_pipe(named_pipe::open_params{.mode = mode, .inherit = inherit},
				std::move(name));
}
winapi::ipc::named_pipe winapi::ipc::open_client_pipe(std::string name, openmode mode, inheritmode inherit,
			std::error_code& err) noexcept
{
	return open_client_pipe(named_pipe::open_params{.mode = mode, .inherit = inherit},
				std::move(name), err);
}
winapi::ipc::named_pipe winapi::ipc::open_client_pipe(const named_pipe::open_params& params, std::string name)
{
	std::error_code err;
	named_pipe result = open_client_pipe(params, std::move(name), err);
	throw_on_error(err, "failed to open named pipe");
	return result;
}
winapi::ipc::named_pipe winapi::ipc::open_client_pipe(const named_pipe::open_params& params, std::string name,
			std::error_code& err) noexcept
{
	dword_t mode = 0;
	switch (params.mode)
	{
	case openmode::in:
		mode = GENERIC_READ | FILE_WRITE_ATTRIBUTES;
		break;
	case openmode::out:
		mode = GENERIC_WRITE;
		break;
	case openmode::inout:
		mode = GENERIC_READ | GENERIC_WRITE;
		break;
	}
	dword_t attr_flags = (params.overlapped ? FILE_FLAG_OVERLAPPED : FILE_ATTRIBUTE_NORMAL);

	dword_t packetmode = 0;
	if (params.packet) packetmode |= PIPE_READMODE_MESSAGE;
	if (params.nonblock) packetmode |= PIPE_NOWAIT;

	security_attributes_t attributes{
				.nLength = sizeof(security_attributes_t),
				.lpSecurityDescriptor = params.security_descriptor,
				.bInheritHandle = static_cast< bool >(params.inherit)
			};

	name.insert(0, "\\\\.\\pipe\\LOCAL\\");
	unique_handle h{winapi::calls::CreateFileA(name.c_str(), mode, 0, &attributes, OPEN_EXISTING,
					attr_flags, nullptr)};
	process_error(h, err);
	if (h && (packetmode != 0))
	{
		process_error(winapi::calls::SetNamedPipeHandleState(h.get(), &packetmode, nullptr, nullptr), err);
	}
	return {std::move(h)};
}
#endif
