#include <print>
#include <vector>
#include <boost/test/unit_test.hpp>

import os;

BOOST_AUTO_TEST_SUITE(numa_tests)

BOOST_AUTO_TEST_CASE(proc_id_consistency)
{
	os::numa::processor lhs{10, 2}, rhs{lhs.get_id()};
	BOOST_TEST(lhs.get_id() == rhs.get_id());
}
BOOST_AUTO_TEST_CASE(ideal_proc)
{
	os::numa::processor cur = os::numa::processor::current();
	os::numa::processor ideal = os::this_thread::get_ideal_processor();
	std::println("running on processor {} (numa {})", cur.get_id(), cur.get_numa_node_id());
	std::println("ideal processor {} (numa {})", ideal.get_id(), ideal.get_numa_node_id());
	BOOST_TEST(os::this_thread::set_ideal_processor(cur).get_id() == ideal.get_id());
	BOOST_TEST(os::this_thread::get_ideal_processor().get_id() == cur.get_id());
}
BOOST_AUTO_TEST_CASE(proc_scan)
{
	std::vector< os::numa::cpu_set > sets = os::numa::cpu_set_info();
	std::println("concurrency {}", os::thread::hardware_concurrency());
	std::println("available {} processors within {} group(s)", os::numa::processor::maximum(),
				os::numa::processor::maximum_groups());
	std::println("maximum {} processors within {} group(s)", os::numa::processor::available(),
				os::numa::processor::available_groups());
	std::println("cpu-sets ({})", sets.size());
	for (const os::numa::cpu_set& set: sets)
	{
		std::println("\tid#{}:", set.id_);
		std::println("\t\thome#{} {{group#{}, id#{}}}",
					set.home_cpu.get_id(), set.home_cpu.group, set.home_cpu.number);
		std::println("\t\tcore#{} cache-sharing-id#{} numa#{} energy-consumption#{}",
					set.core, set.cache_share, set.numa, set.power_consumption);
		std::println("\t\tparked {} allocated{{system {} process {}}} realtime {}",
					set.parked, set.allocated, set.allocated_to_process, set.realtime);
		std::println("\t\tschedule - {} allocation - {}",
					set.scheduler_class, set.allocation_tag);
	}
}

BOOST_AUTO_TEST_SUITE_END()
