#include <boost/test/unit_test.hpp>

import std;
import os;

BOOST_AUTO_TEST_SUITE(pipe_tests)

constexpr const char test_msg[] = "allo";

BOOST_AUTO_TEST_CASE(traits_test)
{
	auto [read, write] = os::ipc::create_pipe();
	BOOST_TEST(read);
	BOOST_TEST(write);

	BOOST_TEST(!read.nonblock());
	BOOST_TEST(!read.packet());
	BOOST_TEST(!write.nonblock());
	BOOST_TEST(!write.packet());

	read.set_nonblock(true);
	BOOST_TEST(read.nonblock());
	BOOST_TEST(!read.packet());
	write.set_nonblock(true);
	BOOST_TEST(write.nonblock());
	BOOST_TEST(!write.packet());
}
BOOST_AUTO_TEST_CASE(read_write_test)
{
	auto [read, write] = os::ipc::create_pipe();
	write.write(test_msg);

	char recv[std::size(test_msg) - 1];
	std::span span{recv};
	read.read(std::as_writable_bytes(span));
	BOOST_TEST(std::memcmp(test_msg, recv, std::size(test_msg) - 1) == 0);

	write.write(test_msg);
	write.close_handle();
	//BOOST_TEST(read.read() == test_msg);
}
BOOST_AUTO_TEST_CASE(eol_test)
{
	auto [read, write] = os::ipc::create_pipe();
	write.write(test_msg);
	write.close_handle();

	char recv[std::size(test_msg) + 1];
	std::span span{recv};
	std::size_t was_read = read.read_once(std::as_writable_bytes(span));
	BOOST_TEST(was_read == std::size(test_msg) - 1);
	BOOST_TEST(std::memcmp(test_msg, recv, std::size(test_msg) - 1) == 0);

	std::error_code err;
	was_read = read.read_once(std::as_writable_bytes(span), err);
	BOOST_TEST(was_read == 0);
	BOOST_TEST(static_cast< bool >(err));
	BOOST_TEST((err == std::errc::broken_pipe));
}
BOOST_AUTO_TEST_CASE(eol_test2)
{
	auto [read, write] = os::ipc::create_pipe();
	write.write(test_msg);
	write.close_handle();

	char recv[std::size(test_msg) + 1];
	std::span span{recv};
	std::error_code err;
	std::size_t was_read = read.read(std::as_writable_bytes(span), err);
	BOOST_TEST(static_cast< bool >(err));
	BOOST_TEST((err == std::errc::broken_pipe));
	BOOST_TEST(was_read == std::size(test_msg) - 1);
	BOOST_TEST(std::memcmp(test_msg, recv, std::size(test_msg) - 1) == 0);

	was_read = read.read(std::as_writable_bytes(span), err);
	BOOST_TEST(was_read == 0);
	BOOST_TEST((err == std::errc::broken_pipe));
}
int reader_func(std::string pipename) noexcept
{
	std::error_code err;
	os::ipc::named_pipe read = os::ipc::open_client_pipe("name", os::openmode::in, err);
	BOOST_TEST((!read.nonblock(err) && !static_cast< bool >(err)));
	BOOST_TEST((!read.packet(err) && !static_cast< bool >(err)));
	char recv[std::size(test_msg) - 1];
	std::span span{recv};
	read.read(std::as_writable_bytes(span), err);
	BOOST_TEST(std::memcmp(test_msg, recv, std::size(test_msg) - 1) == 0);
	return static_cast< bool >(err);
}
BOOST_AUTO_TEST_CASE(named_rw_test)
{
	os::ipc::named_pipe write = os::ipc::create_server_pipe("name", os::openmode::out);
	os::thread reader{reader_func, "name"};
	std::error_code err;
	BOOST_TEST(write.connect(err));
	write.write(test_msg, err);
	BOOST_TEST(!static_cast< bool >(err));
	write.close_handle();

	BOOST_TEST(reader.join() == 0);
}
int reader_packet_func(std::string pipename) noexcept
{
	std::error_code err;
	os::ipc::named_pipe::open_params params{.mode = os::openmode::in};
	params.packet = true;
	os::ipc::named_pipe read = os::ipc::open_client_pipe(params, "name", err);
	BOOST_TEST((!read.nonblock(err) && !static_cast<bool>(err)));
	BOOST_TEST(read.packet(err));
	BOOST_TEST(!static_cast<bool>(err));
	BOOST_TEST(read.read(err) == test_msg);
	return static_cast< bool >(err);
}
BOOST_AUTO_TEST_CASE(packet_test)
{
	os::ipc::named_pipe::initial_params params{.mode = os::openmode::out};
	params.packet = true;
	os::ipc::named_pipe write = os::ipc::create_server_pipe(params, "name");
	os::thread reader{reader_packet_func, "name"};
	std::error_code err;
	BOOST_TEST(write.connect(err));
	write.write(test_msg, err);
	BOOST_TEST(!static_cast< bool >(err));
	write.close_handle();

	BOOST_TEST(reader.join() == 0);

}

BOOST_AUTO_TEST_SUITE_END()
