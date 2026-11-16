export module os.utils;

import std;

export namespace os
{
	struct dangling_struct
	{};
	template < class DanglingParam, template < class... > class Check >
	struct evaluate_check
	{
		template< class U >
		static std::true_type operator()(Check< U >*);
		template< class U >
		static std::false_type operator()(...);

		static constexpr bool value = decltype(operator()< DanglingParam >(0))::value;
	};
	template < class DanglingParam, template < class... > class Check >
	constexpr bool evaluate_check_v = os::evaluate_check< DanglingParam, Check >::value;
}
