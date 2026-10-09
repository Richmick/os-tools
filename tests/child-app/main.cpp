import std;
import os;

int main(int argc, const char*const* argv)
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
	os::argv_parser parser;
	parser.add_flag("short-test", 's');
	parser.add_key("mutex-handle", 'm');
	os::flags flags;
	try
	{
		flags = parser.parse(argc, argv);
	}
	catch (const std::invalid_argument& e)
	{
		std::println(std::cerr, "<-child->: failed to parse argv: {}", e.what());
		return 2;
	}
	if (flags.has("short-test"))
	{
		return 0;
	}
	std::println(std::cerr, "<-child->: unready functional");
	return -1;
}
