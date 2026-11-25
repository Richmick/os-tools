import std;
import os;

#ifdef _WIN32
int __stdcall sig_handler(winapi::dword_t type)
{
	if (type == os::ipc::signal::type::terminate)
	{
		os::this_process::_exit(2);
	}
	return true;
}
#elifdef __linux__
void sig_handler(int type, linapi::siginfo_t* info, void* context)
{
	if (type == os::ipc::signal::type::terminate)
	{
		os::this_process::_exit(2);
	}
}
#endif


int main(int argc, const char*const* argv)
{
	os::argv_parser parser;
	parser.add_flag("env-test", 'e');
	parser.add_flag("sig-test", 's');
	parser.add_flag("interactive-test", 'i');
	parser.add_flag("shm-test", 'i');
	parser.add_key("event");
	parser.add_key("mutex", 'm');
	parser.add_key("map");
	os::flags flags;
	os::ipc::event init_event{os::make_empty};
	os::file_mapping map;
	try
	{
		flags = parser.parse(argc, argv);
		if (flags.has("event"))
		{
			init_event = os::ipc::event{os::to_handle(flags["event"])};
		}
		if (flags.has("map"))
		{
			map = os::file_mapping{os::to_handle(flags["map"])};
		}
	}
	catch (const std::invalid_argument& e)
	{
		std::println(std::cerr, "<-child->: failed to parse argv: {}", e.what());
		return 2;
	}
	if (flags.has("env-test"))
	{
		std::optional env_var = os::this_process::get_env("CHILD_ENV_TEST_VAR");
		if (!env_var)
		{
			std::println(std::cerr, "<-child->: no env var \"CHILD_ENV_TEST_VAR\" set");
			return 1;
		}
		if (*env_var != "value 2026")
		{
			std::println(std::cerr, "<-child->: env var \"CHILD_ENV_TEST_VAR\" expected to be \"value 2026\"");
			return 1;
		}
		return 0;
	}
	if (flags.has("sig-test"))
	{
#ifdef _WIN32
		winapi::ipc::signal::register_callback(sig_handler);
#elifdef __linux__
		linapi::ipc::signal::register_callback(sig_handler, os::ipc::signal::terminate);
		linapi::ipc::signal::register_callback(sig_handler, os::ipc::signal::interrupt);
#else
		if (init_event) init_event.signal();
		return 0;
#endif
		if (init_event) init_event.signal();
		while (true)
		{
			std::this_thread::sleep_for(std::chrono::seconds::max());
		}
		return 0;
	}
	if (flags.has("shm-test"))
	{
		struct mem_struct
		{
			std::uint32_t len = 0;
			std::uint32_t arr_base;
		};
		os::filemode mode = static_cast< os::filemode >(os::filemode::read | os::filemode::write);
		os::mapped_memory mem = map.map(mode, 0, 4096);
		mem_struct* typed = mem.as< mem_struct >();
		init_event.wait();

		if (typed->len == 0) return 3;
		for (std::size_t i = 0; i < typed->len; i++)
		{
			if ((&typed->arr_base)[i] != (1 << i)) return 3;
			(&typed->arr_base)[i] <<= 1;
		}
		typed->len--;
		return 0;
	}
	if (init_event) init_event.signal();
	if (flags.has("interactive-test"))
	{
		std::cerr << "Child started\n";
		std::string in;
		if (!(std::cin >> in) || (in != "msg1")) return 3;
		std::cerr << "Child recieved msg1\n";
		std::cout << "resp1 ";
		if (!(std::cin >> in) || (in != "msg2")) return 3;
		std::cerr << "Child exited\n";
		std::cout << "resp2";
		return 0;
	}
}
