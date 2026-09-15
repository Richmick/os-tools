export module os;

#ifdef _WIN32
export import os.winapi;

export namespace os
{
	using winapi::handle_t;
	using winapi::unique_handle;
	using winapi::call_self_t;
	using winapi::call_self;

	using winapi::thread;
	using winapi::process;
	namespace this_thread
	{
		using namespace winapi::this_thread;
	}
	namespace this_process
	{
		using namespace winapi::this_process;
	}
}
#elifdef __linux__
export import os.linapi;

export namespace os
{
	using linapi::handle_t;
	using linapi::unique_handle;
	using linapi::call_self_t;
	using linapi::call_self;

	using linapi::thread;
	using linapi::process;
	namespace this_thread
	{
		using namespace linapi::this_thread;
	}
	namespace this_process
	{
		using namespace linapi::this_process;
	}
}
#endif
