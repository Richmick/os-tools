module;
	#include <cassert>
module os.argv_parser;

import std;

namespace
{
	struct plug
	{
		static inline std::size_t operator()(std::span< const char* const > remaining) noexcept
		{
			return 0;
		}
	};
}

bool os::argv_parser::option::operator==(const option& rhs) const
{
	return (long_flag == rhs.long_flag) || (flag == rhs.flag);
}
bool os::argv_parser::option::operator==(std::string_view rhs) const
{
	return long_flag == rhs;
}
std::size_t os::argv_parser::hash::operator()(const option& rhs) const noexcept
{
	return std::hash< std::string_view >::operator()(rhs.long_flag);
}
std::size_t os::argv_parser::hash::operator()(std::string_view rhs) const noexcept
{
	return std::hash< std::string_view >::operator()(rhs);
}

os::flags os::argv_parser::parse(int argc, const char*const* argv)
{
	return parse(std::span{argv, static_cast< std::size_t >(argc)});
}
os::flags os::argv_parser::parse(std::span< const char* const > args)
{
	return parse(args, plug{});
}
std::size_t os::argv_parser::parse_arg(std::span< const char*const > args, flags& result)
{
	const char* arg = args[0];
	if (arg[0] != '-')
	{
		result.plain().push_back(arg);
		return 1;
	}
	if (arg[1] != '-')
	{
		auto key = extend_map_.find(*++arg);
		if (key == extend_map_.end())
			throw std::invalid_argument("unknown -<flag>");
		const auto& opt = *options_.find(key->second);
		if (opt.type == match::key)
		{
			if ((*arg != '\0') && (*++arg != '\0'))
				throw std::invalid_argument("-<flag> after -<key_flag>");
			result.add(opt.long_flag, args[1]);
			return 2;
		}
		result.add(opt.long_flag, {});
		if (*arg == '\0') return 1;

		for (arg++; *arg != '\0'; arg++)
		{
			auto key = extend_map_.find(*++arg);
			if (key == extend_map_.end())
				throw std::invalid_argument("unknown -<flag>");
			const auto& opt = *options_.find(key->second);
			if (opt.type != match::flag)
				throw std::invalid_argument("-<key_flag> inside flag batch");
			result.add(opt.long_flag, {});
		}
		return 1;
	}
	if (arg[2] == '\0')
	{
		return static_cast< std::size_t >(-1);
	}
	std::string_view key_value{arg + 2};
	std::string_view key = key_value;
	auto eq_sign = key_value.find_first_of('=');
	if (eq_sign != std::string_view::npos)
		key = key_value.substr(0, eq_sign);
	auto pos = options_.find(key);
	if (pos == options_.end())
		throw std::invalid_argument("unknown --<long_flag>");
	if (pos->type != match::key)
	{
		if (eq_sign != std::string_view::npos)
			throw std::invalid_argument("value with flag --<long_flag>=<...>");
		result.add(key, {});
		return 1;
	}
	if (eq_sign != std::string_view::npos)
	{
		result.add(key, {key_value.substr(eq_sign + 1)});
		return 1;
	}
	if (args.size() < 2)
		throw std::invalid_argument("missing value with --<key>");
	result.add(key, args[1]);
	return 2;
}
os::argv_parser& os::argv_parser::add_key(std::string_view name, char alias)&
{
	[[maybe_unused]] bool ins = options_.insert({name, alias, match::key}).second;
	assert(ins);
	if (alias != '\0') extend_map_.emplace(alias, name);
	return *this;
}
os::argv_parser& os::argv_parser::add_flag(std::string_view name, char alias)&
{
	[[maybe_unused]] bool ins = options_.insert({name, alias, match::flag}).second;
	assert(ins);
	if (alias != '\0') extend_map_.emplace(alias, name);
	return *this;
}
os::argv_parser::match os::argv_parser::test(std::string_view flag) const
{
	auto pos = options_.find(flag);
	return pos == options_.end() ? match::none : pos->type;
}
os::argv_parser::match os::argv_parser::test(char flag) const
{
	auto key = extend_map_.find(flag);
	return key == extend_map_.end() ? match::none : options_.find(key->second)->type;
}

void os::flags::add(std::string_view name, std::string_view value)
{
	std::pair res = key_values_.emplace(name, value);
	if (res.second) return;
	res.first->second = value;
}
bool os::flags::has(std::string_view flag) const
{
	return key_values_.contains(flag);
}
std::string_view os::flags::operator[](std::string_view key) const
{
	auto pos = key_values_.find(key);
	return pos != key_values_.end() ? pos->second : std::string_view{};
}
std::vector< std::string_view >& os::flags::plain() &
{
	return plain_args_;
}
const std::vector< std::string_view >& os::flags::plain() const&
{
	return plain_args_;
}
void os::flags::clear() &
{
	key_values_.clear();
	plain_args_.clear();
}
