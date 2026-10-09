#include <boost/test/unit_test.hpp>

import std;
import os;

using namespace std::chrono_literals;

BOOST_AUTO_TEST_SUITE(thread_tests)

BOOST_AUTO_TEST_CASE(empty_test)
{
	os::thread thread;
	BOOST_TEST(!thread.joinable());
}
BOOST_AUTO_TEST_CASE(single_test)
{
	bool finished = false;
	os::thread thread([&finished]{ finished = true; });
	BOOST_TEST(thread.joinable());
	BOOST_TEST(thread.get_process_id() == os::this_process::get_id());
	BOOST_TEST(thread.join() == 0);
	BOOST_TEST(finished);
}
BOOST_AUTO_TEST_CASE(detach_test)
{
	os::thread thread([]{ std::this_thread::sleep_for(100ms); });
	thread.detach();
	BOOST_TEST(!thread.joinable());
}
int priority_ret_test_thread(int arg)
{
	std::this_thread::sleep_for(100ms);
	BOOST_TEST((os::this_thread::get_priority() == os::thread::priority::idle));
	//os::this_thread::exit(arg + 1);
	return arg + 1;
}
BOOST_AUTO_TEST_CASE(priority_ret_test)
{
	os::thread thread(priority_ret_test_thread, 77);
	thread.set_priority(os::thread::priority::idle);
	BOOST_TEST(thread.join() == 78);
}
BOOST_AUTO_TEST_CASE(initial_params_test)
{
	os::thread::initial_params params{.priority = os::thread::priority::idle};
	BOOST_TEST((os::thread{params, priority_ret_test_thread, 77}.join()) == 78);
	BOOST_TEST((os::thread{static_cast< const os::thread::initial_params& >(params),
					priority_ret_test_thread, 77}.join()) == 78);
	BOOST_TEST((os::thread{os::thread::initial_params{.priority = os::thread::priority::idle},
					priority_ret_test_thread, 77}.join()) == 78);
	BOOST_TEST((os::thread{static_cast< const os::thread::initial_params&& >(params),
					priority_ret_test_thread, 77}.join()) == 78);
}

BOOST_AUTO_TEST_SUITE_END()
