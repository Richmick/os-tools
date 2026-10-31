module os.winapi;

#ifdef _WIN32
import std;

winapi::numa::processor::processor(id new_id) noexcept:
	group(new_id >> std::numeric_limits< decltype(number) >::digits),
	number(new_id & ~(~static_cast< id >(0) << std::numeric_limits< decltype(number) >::digits))
{}
winapi::numa::processor::processor(processor_number number) noexcept:
	group(number.Group),
	number(number.Number)
{}
winapi::numa::processor::processor(std::uint16_t new_group, std::uint8_t new_number) noexcept:
	group(new_group),
	number(new_number)
{}

winapi::numa::processor winapi::numa::processor::current() noexcept
{
	processor_number res;
	winapi::calls::GetCurrentProcessorNumberEx(&res);
	return res;
}
std::size_t winapi::numa::processor::available_groups() noexcept
{
	return winapi::calls::GetActiveProcessorGroupCount();
}
std::size_t winapi::numa::processor::available() noexcept
{
	return winapi::calls::GetActiveProcessorCount(all_groups);
}
std::size_t winapi::numa::processor::available(group_id group) noexcept
{
	return winapi::calls::GetActiveProcessorCount(group);
}
std::size_t winapi::numa::processor::maximum_groups() noexcept
{
	return winapi::calls::GetMaximumProcessorGroupCount();
}
std::size_t winapi::numa::processor::maximum() noexcept
{
	return winapi::calls::GetMaximumProcessorCount(all_groups);
}
std::size_t winapi::numa::processor::maximum(group_id group) noexcept
{
	return winapi::calls::GetMaximumProcessorCount(group);
}

winapi::numa::processor::id winapi::numa::processor::get_id() const noexcept
{
	return (static_cast< id >(group) << std::numeric_limits< decltype(number) >::digits) | number;
}
winapi::numa::node::id winapi::numa::processor::get_numa_node_id() const
{
	std::error_code err;
	node::id res = get_numa_node_id(err);
	throw_on_error(err, "failed to obtain numa id of processor");
	return res;
}
winapi::numa::node::id winapi::numa::processor::get_numa_node_id(std::error_code& err) const noexcept
{
	processor_number proc = *this;
	unsigned short res;
	if (!winapi::calls::GetNumaProcessorNodeEx(&proc, &res))
	{
		err = winapi::last_error_code();
		return -1;
	}
	err.clear();
	return res;
}
winapi::numa::processor::operator winapi::processor_number() const noexcept
{
	return {.Group = group, .Number = number};
}

std::vector< winapi::numa::cpu_set > winapi::numa::cpu_set_info()
{
	handle_t proc = this_process::native_handle();

	unsigned long size;
	winapi::calls::GetSystemCpuSetInformation(nullptr, 0, &size, proc, 0);
	if (size == 0)
	{
		return {};
	}
	std::vector< std::byte > raw(size);
	do // hotplug inconsistency avoiding
	{
		raw.resize(size);
		winapi::calls::GetSystemCpuSetInformation(reinterpret_cast< cpuset_info* >(raw.data()), raw.size(), &size, proc, 0);
		if (size == 0)
		{
			return {};
		}
	}
	while(raw.size() < size);

	std::byte* caret = raw.data();
	std::byte* const end = raw.data() + size;
	std::size_t nsets = 0;
	while (caret < end)
	{
		cpuset_info& metadata = *reinterpret_cast<cpuset_info*>(caret);
		caret += metadata.Size;
		if (metadata.Type != cpuset_info_type::CpuSetInformation) continue;
		nsets++;
	}

	std::vector< cpu_set > result;
	result.reserve(nsets);
	caret = raw.data();
	while (caret < end)
	{
		cpuset_info& metadata = *reinterpret_cast< cpuset_info* >(caret);
		caret += metadata.Size;
		if (metadata.Type != cpuset_info_type::CpuSetInformation) continue;

		auto& set = metadata.CpuSet; // unnamed struct
		result.push_back({
						.id_ = set.Id,
						.home_cpu = {set.Group, set.LogicalProcessorIndex},
						.core = set.CoreIndex,
						.cache_share = set.LastLevelCacheIndex,
						.numa = set.NumaNodeIndex,
						.power_consumption = set.EfficiencyClass,
						.parked = static_cast< bool >(set.Parked),
						.allocated = static_cast< bool >(set.Allocated),
						.allocated_to_process = static_cast< bool >(set.AllocatedToTargetProcess),
						.realtime = static_cast< bool >(set.RealTime),
						.scheduler_class = set.SchedulingClass,
						.allocation_tag = set.AllocationTag
					});
	}
	return result;
}
#endif
