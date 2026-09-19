import std;
import os;
import os.argv_parser;

using namespace std::chrono_literals;

void thread_func(std::chrono::system_clock::time_point deadline)
{
	std::println("[{}] running", os::this_thread::get_id());
	while (std::chrono::system_clock::now() < deadline)
	{
		std::this_thread::sleep_for(1s);
		std::println("[{}] running", os::this_thread::get_id());
	}
}

int main(int argc, const char*const* argv)
{
	std::println("{} {} priority proc_{}::thread_{}",
				os::this_process::get_id(),
				os::this_process::executable(),
				static_cast< int >(os::this_process::get_priority()),
				static_cast< int >(os::this_thread::get_priority()));

	std::optional test_var = os::this_process::get_env("test_var");
	if (test_var) std::println("test_var = \"{}\"", *test_var);
	else std::println("test_var is not set");

	os::argv_parser parser;
	parser.add_flag("slave", 's');
	parser.add_key("nthreads", 'n');
	parser.add_key("lifetime", 'l');

	std::size_t nthreads = 1;
	std::chrono::seconds lifetime = 5s;
	os::flags flags;
	try
	{
		flags = parser.parse(argc, argv);
	}
	catch (const std::exception& e)
	{
		std::println(std::cerr, "error: {}", e.what());
		return 1;
	}
	if (flags.has("nthreads")) nthreads = std::stoull(flags["nthreads"].data());
	if (flags.has("lifetime")) lifetime = std::chrono::seconds{std::stoull(flags["lifetime"].data())};

	if (flags.has("slave"))
	{
		std::vector< os::thread > threads;
		threads.reserve(nthreads);
		std::chrono::time_point deadline = std::chrono::system_clock::now() + lifetime;
		while (nthreads-- > 0)
		{
			threads.emplace_back(thread_func, deadline);
		}
		for (os::thread& thread: threads)
		{
			thread.join();
		}
		std::println("{} exited", os::this_process::get_id());
		return 1;
	}
	os::this_process::set_env("test_var", "env was inherited");
	std::vector< std::string > args = {"-s", std::format("--nthreads={}", nthreads),
				std::format("--lifetime={}", lifetime.count())};
	os::process proc{os::call_self, args};
	proc.set_priority(os::process::priority::idle);
	std::println("child is alive - {}", proc.alive());
	std::println("child wait got result - {}", proc.wait());
	std::println("child is alive - {}", proc.alive());
	std::println("child join got result - {}", proc.join());
	std::println("{} exited", os::this_process::get_id());
	for (const auto& keyval : os::this_process::environment())
	{
		std::println("\"{}\": \"{}\"", keyval.first, keyval.second);
	}
}
