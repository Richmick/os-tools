#include <boost/test/unit_test.hpp>

import std;
import os;

BOOST_AUTO_TEST_SUITE(net_tests)

os::net::lib_anchor lib;

BOOST_AUTO_TEST_CASE(basic_test)
{
	os::net::socket server{os::net::address::type::ipv4, os::net::protocol::tcp};
	BOOST_TEST((server.handle_inheritance() == os::inheritmode::none));
	server.enable_handle_inheritance();
	BOOST_TEST((server.handle_inheritance() == os::inheritmode::all));
	server.disable_handle_inheritance();
	BOOST_TEST((server.handle_inheritance() == os::inheritmode::none));

	os::net::address addr(0, 0, 0, 0, 23849);
	server.bind(addr);
	server.listen();

	//auto [connection, conn_addr] = server.accept();
}

BOOST_AUTO_TEST_SUITE_END()
