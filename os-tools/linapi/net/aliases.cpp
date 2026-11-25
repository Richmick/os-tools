module os.linapi;

#ifdef __linux__
linapi::net::address_any::operator linapi::net::address_link() & noexcept
{
	return {reinterpret_cast< socket_address_t* >(&native), sizeof(native_type)};
}
linapi::net::address_any::operator linapi::net::const_address_link() const& noexcept
{
	return {reinterpret_cast< const socket_address_t* >(&native), sizeof(native_type)};
}

linapi::net::address_ipv4::operator linapi::net::address_link() & noexcept
{
	return {reinterpret_cast< socket_address_t* >(&native_), sizeof(native_type)};
}
linapi::net::address_ipv4::operator linapi::net::const_address_link() const& noexcept
{
	return {reinterpret_cast< const socket_address_t* >(&native_), sizeof(native_type)};
}

linapi::net::address_ipv6::operator linapi::net::address_link() & noexcept
{
	return {reinterpret_cast< socket_address_t* >(&native_), sizeof(native_type)};
}
linapi::net::address_ipv6::operator linapi::net::const_address_link() const& noexcept
{
	return {reinterpret_cast< const socket_address_t* >(&native_), sizeof(native_type)};
}
#endif
