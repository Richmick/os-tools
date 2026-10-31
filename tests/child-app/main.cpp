import std;
import os;

#ifdef _WIN32
int __stdcall sig_handler(winapi::dword_t type)
{
	if (type == os::ipc::signal::type::sigkill)
	{
		os::this_process::_exit(2);
	}
	return true;
}
#endif


int main(int argc, const char*const* argv)
{
	os::argv_parser parser;
	parser.add_flag("env-test", 'e');
	parser.add_flag("sig-test", 's');
	parser.add_flag("interactive-test", 'i');
	parser.add_key("event");
	parser.add_key("mutex", 'm');
	os::flags flags;
	os::ipc::event init_event{os::make_empty};
	try
	{
		flags = parser.parse(argc, argv);
		if (flags.has("event"))
		{
			init_event = os::ipc::event{os::to_handle(flags["event"])};
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
		winapi::ipc::signal::register_callback(sig_handler);
		if (init_event) init_event.signal();
		while (true)
		{
			std::this_thread::sleep_for(std::chrono::seconds::max());
		}
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
