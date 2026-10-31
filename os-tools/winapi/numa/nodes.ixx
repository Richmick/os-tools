export module os.winapi:nodes;

#ifdef _WIN32
import std;
import :aliases;

export namespace winapi::numa
{
	struct cpu_set
	{
		using id = dword_t;
		id id_ = 0;
		std::uint8_t power_consumption = 0; ///< Higher val¸ue => less effective but faster
	};
	struct cpu_group
	{
		using id = std::uint16_t;
		id id_ = 0;
		std::vector< cpu_set > sets;
	};
	struct node
	{
		std::vector< cpu_group > groups;
	};
	struct info
	{
		std::vector< node > nodes;
	};
	info get_info(); ///< WinApi guarantees to succeed on valid requests, but std::bad_alloc
}
#endif
