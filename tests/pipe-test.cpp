#include <span>
#include <cstring>

#include <boost/test/unit_test.hpp>

import os;

BOOST_AUTO_TEST_SUITE(pipe_tests)

BOOST_AUTO_TEST_CASE(traits_test)
{
	auto [read, write] = os::ipc::create_pipe();
	BOOST_TEST(read);
	BOOST_TEST(write);
}
BOOST_AUTO_TEST_CASE(read_write_test)
{
	constexpr const char msg[] = "allo";
	auto [read, write] = os::ipc::create_pipe();
	write.write(msg);

	char recv[std::size(msg) - 1];
	std::span span{recv};
	read.read(std::as_writable_bytes(span));
	BOOST_TEST(std::memcmp(msg, recv, std::size(msg) - 1) == 0);
}
BOOST_AUTO_TEST_CASE(eol_test)
{
	constexpr const char msg[] = "allo";
	auto [read, write] = os::ipc::create_pipe();
	write.write(msg);
	write.close_handle();

	char recv[std::size(msg) + 1];
	std::span span{recv};
	std::size_t was_read = read.read_once(std::as_writable_bytes(span));
	BOOST_TEST(was_read == std::size(msg) - 1);
	BOOST_TEST(std::memcmp(msg, recv, std::size(msg) - 1) == 0);

	was_read = read.read_once(std::as_writable_bytes(span));
	BOOST_TEST(was_read == 0); // eol
}
BOOST_AUTO_TEST_CASE(eol_test2)
{
	constexpr const char msg[] = "allo";
	auto [read, write] = os::ipc::create_pipe();
	write.write(msg);
	write.close_handle();

	char recv[std::size(msg) + 1];
	std::span span{recv};
	std::size_t was_read = read.read(std::as_writable_bytes(span));
	BOOST_TEST(was_read == std::size(msg) - 1);
	BOOST_TEST(std::memcmp(msg, recv, std::size(msg) - 1) == 0);

	was_read = read.read(std::as_writable_bytes(span));
	BOOST_TEST(was_read == 0); // eol
}
BOOST_AUTO_TEST_CASE(named_rw_test)
{
	os::path p = "./name";
	std::cout << p.native() << ' ' << p.is_relative() << '\n';
	constexpr const char msg[] = "allo";
	auto [read, write] = os::ipc::create_pipe();
	write.write(msg);

	char recv[std::size(msg) - 1];
	std::span span{recv};
	read.read(std::as_writable_bytes(span));
	BOOST_TEST(std::memcmp(msg, recv, std::size(msg) - 1) == 0);
}

BOOST_AUTO_TEST_SUITE_END()
