export module os.utils;

import std;

export namespace os
{
	template< std::integral T >
	constexpr T byte_order_example();
	template< std::endian Target, std::endian Source = std::endian::native, std::integral T >
	constexpr T endian_cast(T value);
}
template< std::integral T >
constexpr T os::byte_order_example()
{
	T res{};
	constexpr int byte_width = std::numeric_limits< unsigned char >::digits;
	for (std::size_t i = 0; i < sizeof(T); i++)
		res |= i << (byte_width * i);
	return res;
}

template< std::endian Target, std::endian Source, std::integral T >
constexpr T os::endian_cast(T value)
{
	if constexpr (Source == Target)
		return value;
	else if constexpr (((Source == std::endian::little) && (Target == std::endian::big))
				|| ((Source == std::endian::big) && (Target == std::endian::little)))
		return std::byteswap(value);
	else if constexpr (sizeof(T) == 1)
		return value;
	else
	{
		constexpr auto order = std::bit_cast< std::array< std::byte, sizeof(T) > >(byte_order_example< T >());
		auto value_bytes = std::bit_cast< std::array< std::byte, sizeof(T) > >(value);
		decltype(value_bytes) res_bytes;
		for (int i = 0; i < sizeof(T); i++)
		{
			if constexpr (Target == std::endian::little)
				res_bytes[static_cast< unsigned >(order[i])] = value_bytes[i];
			else if constexpr (Target == std::endian::big)
				res_bytes[sizeof(T) - 1 - static_cast< unsigned >(order[i])] = value_bytes[i];
			else if constexpr (Source == std::endian::little)
				res_bytes[i] = value_bytes[static_cast< unsigned >(order[i])];
			else if constexpr (Source == std::endian::big)
				res_bytes[i] = value_bytes[sizeof(T) - 1 - static_cast< unsigned >(order[i])];
			else
				static_assert(false, "got invalid endian values");
		}
		return std::bit_cast< T >(res_bytes);
	}
}
