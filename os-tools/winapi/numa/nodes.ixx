module;
	#ifdef _WIN32
		#define WIN32_LEAN_AND_MEAN
		#include <windows.h>
	#endif
export module os.winapi:numa_nodes;

#ifdef _WIN32
import std;
import :aliases;

export namespace winapi::numa
{
	struct node
	{
		using id = std::uint16_t;
	};
	/// logical processor (part of real cpu core)
	class processor
	{
	public:
		using id = std::uint32_t;
		using group_id = std::uint16_t;
		using number_id = std::uint8_t;

		static constexpr inline group_id all_groups = ALL_PROCESSOR_GROUPS;

		group_id group = 0;
		number_id number = 0;

		processor() noexcept = default;
		processor(id new_id) noexcept;
		processor(processor_number number) noexcept;
		processor(group_id new_group, number_id new_number) noexcept;

		static processor current() noexcept;
		static std::size_t available_groups() noexcept;
		static std::size_t available() noexcept;
		static std::size_t available(group_id group) noexcept;
		static std::size_t maximum_groups() noexcept;
		static std::size_t maximum() noexcept;
		static std::size_t maximum(group_id group) noexcept;

		id get_id() const noexcept;
		node::id get_numa_node_id() const;
		/// Returns -1 if processor not exists
		node::id get_numa_node_id(std::error_code& err) const noexcept;
		operator processor_number() const noexcept;
	};
	struct cpu_set
	{
		using id = dword_t;

		id id_ = 0;
		processor home_cpu;
		/// Independable  core id
		std::uint8_t core = 0;
		/// Id of processor group having at least 1 shared cache layer (e.g. L3)
		std::uint8_t cache_share = 0;
		/// Numa node id
		node::id numa = 0;
		/// Higher value => less effective but faster
		std::uint8_t power_consumption = 0;
		/// Was not running when info collected (e.g. for energy efficiency)
		bool parked : 1;
		bool allocated : 1;
		/// Process uses this set
		bool allocated_to_process : 1;
		/// Realtime scheduler
		bool realtime : 1;

		std::uint8_t scheduler_class = 0;
		std::uint64_t allocation_tag = 0;
	};
	std::vector< cpu_set > cpu_set_info(); ///< WinApi guarantees to succeed on valid requests, but std::bad_alloc
}
#endif
