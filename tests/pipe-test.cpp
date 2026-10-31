#include <boost/test/unit_test.hpp>

import std;
import os;

BOOST_AUTO_TEST_SUITE(pipe_tests)

BOOST_AUTO_TEST_CASE(traits_test)
{
	os::ipc::pipe read, write = read.release_write();

	BOOST_TEST(read.can_read());
	BOOST_TEST(!read.can_write());
	BOOST_TEST((read.mode() == os::openmode::in));

	BOOST_TEST(!write.can_read());
	BOOST_TEST(write.can_write());
	BOOST_TEST((write.mode() == os::openmode::out));
}
BOOST_AUTO_TEST_CASE(read_write_test)
{
	constexpr const char msg[] = "allo";
	os::ipc::pipe read{std::size(msg) + 1}, write = read.release_write();
	write.write(msg);

	char recv[std::size(msg) - 1];
	std::span span{recv};
	read.read(std::as_writable_bytes(span));
	BOOST_TEST(std::memcmp(msg, recv, std::size(msg) - 1) == 0);
}

BOOST_AUTO_TEST_SUITE_END()
