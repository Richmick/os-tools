export module os;

#ifdef _WIN32
export import os.winapi;

export namespace os
{
	using winapi::handle_t;
	using winapi::unique_handle;
}
#elifdef __linux__
export import os.linapi;

export namespace os
{
	using linapi::handle_t;
	using linapi::unique_handle;
}
#endif
