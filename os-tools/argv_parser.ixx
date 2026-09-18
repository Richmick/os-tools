export module os.argv_parser;

import std;

namespace os
{
	export class argv_parser;
	export class flags;
}

/// Parser for argv
class os::argv_parser
{
public:
	/// Flag match type
	enum class match: char
	{
		flag, key, none
	};

	/// Parse argv
	flags parse(int argc, const char*const* argv);
	/// Parse argv with custom override policy
	flags parse(int argc, const char*const* argv,
				std::invocable< std::span< const char* const > > auto&& hint);
	/// Parse argv
	flags parse(std::span< const char*const > args);
	/// Parse argv with custom override policy
	flags parse(std::span< const char*const > args,
				std::invocable< std::span< const char* const > > auto&& hint);

	/// Expect arguments like "--<key>=value", "--<key> value", "-<alias> value"
	argv_parser& add_key(std::string_view name, char alias = '\0')&;
	/// Expect flags like "--<name>" with alias "-<alias>"
	argv_parser& add_flag(std::string_view name, char alias = '\0')&;

	/// Test if flag is registered
	match test(std::string_view flag) const;
	/// Test if flag is registered
	match test(char flag) const;

private:
	struct option
	{
		std::string_view long_flag;
		char flag = '\0';
		match type = match::flag;

		bool operator==(const option& rhs) const;
		bool operator==(std::string_view rhs) const;
	};
	struct hash: public std::hash< std::string_view >
	{
		using is_transparent = int;
		std::size_t operator()(const option& rhs) const noexcept;
		std::size_t operator()(std::string_view rhs) const noexcept;
	};
	std::unordered_set< option, hash, std::equal_to<> > options_;
	std::unordered_map< char, std::string_view > extend_map_;
	std::size_t parse_arg(std::span< const char* const > args, flags& result);
};
/// Result of parsing containing flags & keys
class os::flags
{
public:
	/// Add key-value pair
	void add(std::string_view name, std::string_view value);
	/// Test if flag/key was found
	bool has(std::string_view flag) const;
	/// Get value associated with key
	std::string_view operator[](std::string_view key) const;
	/// Accessor to unparsed arguments
	std::vector< std::string_view >& plain() &;
	/// Accessor to unparsed arguments
	const std::vector< std::string_view >& plain() const&;
	/// Clear already saved data
	void clear() &;

private:
	std::unordered_map< std::string_view, std::string_view > key_values_;
	std::vector< std::string_view > plain_args_;
};

os::flags os::argv_parser::parse(int argc, const char* const* argv,
			std::invocable< std::span< const char* const > > auto&& hint)
{
	return parse(std::span{argv, static_cast<std::size_t>(argc)}, std::forward< decltype(hint) >(hint));
}
os::flags os::argv_parser::parse(std::span< const char* const > args,
			std::invocable< std::span< const char* const > > auto&& hint)
{
	flags result;
	for (std::size_t i = 0; i < args.size();)
	{
		std::size_t offset = hint(args.subspan(i));
		if (offset == 0) offset = parse_arg(args.subspan(i), result);
		if (offset == static_cast<std::size_t>(-1))
		{
			result.plain().append_range(args.subspan(i + 1));
			return result;
		}
		i += offset;
	}
	return result;
}
