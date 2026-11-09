#include <boost/test/unit_test.hpp>

import std;
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

	std::error_code err;
	was_read = read.read_once(std::as_writable_bytes(span), err);
	BOOST_TEST(was_read == 0);
	BOOST_TEST(static_cast< bool >(err));
	BOOST_TEST((err == std::errc::broken_pipe));
}
BOOST_AUTO_TEST_CASE(eol_test2)
{
	constexpr const char msg[] = "allo";
	auto [read, write] = os::ipc::create_pipe();
	write.write(msg);
	write.close_handle();

	char recv[std::size(msg) + 1];
	std::span span{recv};
	std::error_code err;
	std::size_t was_read = read.read(std::as_writable_bytes(span), err);
	BOOST_TEST(static_cast< bool >(err));
	BOOST_TEST((err == std::errc::broken_pipe));
	BOOST_TEST(was_read == std::size(msg) - 1);
	BOOST_TEST(std::memcmp(msg, recv, std::size(msg) - 1) == 0);

	was_read = read.read(std::as_writable_bytes(span), err);
	BOOST_TEST(was_read == 0);
	BOOST_TEST((err == std::errc::broken_pipe));
}
BOOST_AUTO_TEST_CASE(named_rw_test)
{
	constexpr const char msg[] = "allo";
	os::ipc::named_pipe::initial_params init{.mode = os::openmode::out, .nonblock = true};
	os::ipc::named_pipe write = os::ipc::create_server_pipe(init, "name");
	os::ipc::named_pipe::open_params open{.mode = os::openmode::in};
	os::ipc::named_pipe read = os::ipc::open_client_pipe(open, "name");
	write.connect();
	//write.write(msg);
	write.close_handle();

	//char recv[std::size(msg) - 1];
	//std::span span{recv};
	//read.read(std::as_writable_bytes(span));
	//BOOST_TEST(std::memcmp(msg, recv, std::size(msg) - 1) == 0);
}

BOOST_AUTO_TEST_SUITE_END()
