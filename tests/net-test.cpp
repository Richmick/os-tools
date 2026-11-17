#include <boost/test/unit_test.hpp>

import std;
import os;

BOOST_AUTO_TEST_SUITE(net_tests)

os::net::lib_anchor lib;
constexpr const char test_msg[] = "allo";

int client_func(os::net::const_address_link link) noexcept
{
	std::error_code err;
	os::net::socket client{os::net::type_of(link), os::net::protocol::tcp, err};
	BOOST_TEST(!!(client));
	BOOST_TEST(client.connect(link, err));
	std::string res = client.read(err);
	BOOST_TEST(res == test_msg);
	return static_cast< bool >(err);
}

BOOST_AUTO_TEST_CASE(ipv4_test)
{
	os::net::socket server{os::net::address_type::ipv4, os::net::protocol::tcp};
	BOOST_TEST((server.handle_inheritance() == os::inheritmode::none));
	server.enable_handle_inheritance();
	BOOST_TEST((server.handle_inheritance() == os::inheritmode::all));
	server.disable_handle_inheritance();
	BOOST_TEST((server.handle_inheritance() == os::inheritmode::none));

	auto addr = os::net::address_traits< os::net::address_ipv4 >::loopback;
	server.bind(addr);
	addr = server.address().as< os::net::address_ipv4 >();
	std::println("Server ipv4 bind on port {}", addr.port());
	server.listen();
	os::thread reader{client_func, addr};

	std::error_code err;
	os::net::socket connection = server.accept(err);
	BOOST_TEST(!static_cast< bool >(err));
	connection.write(test_msg, err);
	BOOST_TEST(!static_cast< bool >(err));
	connection.shutdown(false, true, err);
	connection.close_handle();

	BOOST_TEST(reader.join() == 0);
}
BOOST_AUTO_TEST_CASE(ipv6_test)
{
	os::net::socket server{os::net::address_type::ipv6, os::net::protocol::tcp};

	auto addr = os::net::address_traits< os::net::address_ipv6 >::loopback;
	server.bind(addr);
	addr = server.address().as< os::net::address_ipv6 >();
	std::println("Server ipv6 bind on port {}", addr.port());
	server.listen();
	os::thread reader{client_func, addr};

	std::error_code err;
	os::net::socket connection = server.accept(err);
	BOOST_TEST(!static_cast< bool >(err));
	connection.write(test_msg, err);
	BOOST_TEST(!static_cast< bool >(err));
	connection.shutdown(false, true, err);
	connection.close_handle();

	BOOST_TEST(reader.join() == 0);
}

BOOST_AUTO_TEST_SUITE_END()
