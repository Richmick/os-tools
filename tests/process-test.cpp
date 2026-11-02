#include <boost/test/unit_test.hpp>
#include <os-tools/os_macro.hpp>

import std;
import os;

#undef stdin
#undef stdout
#undef stderr

BOOST_AUTO_TEST_SUITE(process_tests)

os::path child_path = os::this_process::executable().parent_path() / OS_STR("os-tools-test_child" EXE_EXTENSION);
BOOST_AUTO_TEST_CASE(empty_test)
{
	os::process proc;
	BOOST_TEST(!proc.joinable());
}
BOOST_AUTO_TEST_CASE(startup_test)
{
	os::process proc{child_path};
	BOOST_TEST(proc.joinable());
	BOOST_TEST(proc.wait() == 0);
	BOOST_TEST(!proc.alive());
	BOOST_TEST(proc.exit_code() == 0);
	BOOST_TEST(proc.join() == 0);
}
BOOST_AUTO_TEST_CASE(env_test,
			* boost::unit_test::depends_on{"process_tests/startup_test"})
{
	os::this_process::set_env("CHILD_ENV_TEST_VAR", "value 2026");
	os::process proc{child_path, std::initializer_list{OS_STR("-e")}};
	BOOST_TEST(proc.join() == 0);
}
BOOST_AUTO_TEST_CASE(event_test,
			* boost::unit_test::depends_on{"process_tests/startup_test"})
{
	os::ipc::event event{os::inheritmode::all};
	os::string event_h = os::to_string(event.native_handle());
	os::process::initial_params params = {.inherit_handles = os::inheritmode::all, .create_group = true};
	os::process proc{params, child_path, std::initializer_list{OS_STR("--event"), event_h.c_str()}};
	event.wait();
	BOOST_TEST(proc.join() == 0);
}
BOOST_AUTO_TEST_CASE(sig_ignore_test,
			* boost::unit_test::depends_on{"process_tests/startup_test"}
			* boost::unit_test::depends_on{"process_tests/event_test"})
{
	os::ipc::event event{os::inheritmode::all};
	os::string event_h = os::to_string(event.native_handle());
	os::process::initial_params params = {.inherit_handles = os::inheritmode::all, .create_group = true};
	os::process proc{params, child_path, std::initializer_list{OS_STR("-s"), OS_STR("--event"), event_h.c_str()}};
	event.wait();

	proc.signal(os::ipc::signal::interrupt);
	std::this_thread::sleep_for(std::chrono::seconds{1});
	BOOST_TEST(proc.alive());
	proc.terminate();
	BOOST_TEST(proc.join() == 2);
}
/*BOOST_AUTO_TEST_CASE(pipe_test,
			* boost::unit_test::depends_on{"process_tests/startup_test"}
			* boost::unit_test::depends_on{"pipe_tests"})
{
	constexpr char msg1[] = "msg1\n";
	constexpr char msg2[] = "msg2";
	constexpr char resp1[] = "resp1 ";
	constexpr char resp2[] = "resp2";
	os::ipc::pipe downlink{os::inheritmode::all, std::size(msg1) + std::size(msg2)};
	os::ipc::pipe uplink{os::inheritmode::all, std::size(resp1) + std::size(resp2)};
	os::set_inheritance(os::inheritmode::none, downlink.native_write_handle());
	os::set_inheritance(os::inheritmode::none, uplink.native_read_handle());

	os::process::initial_params params = {
				.inherit_handles = os::inheritmode::all,
				.stdin = downlink.native_read_handle(),
				.stdout = uplink.native_write_handle(),
				.stderr = os::this_process::stderr()
			};
	os::process proc{params, child_path, std::initializer_list{OS_STR("-i")}};
	downlink.close_read_handle();
	uplink.close_write_handle();

	downlink.write(msg1);
	downlink.write(msg2);
	downlink.close_write_handle(); // send eol
	BOOST_TEST(proc.join() == 0);
}*/

BOOST_AUTO_TEST_SUITE_END()
