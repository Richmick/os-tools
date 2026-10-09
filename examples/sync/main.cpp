import std;
import os.argv_parser;

#include "blocking_queue.hpp"

using namespace std::chrono_literals;

static void supplier(example::blocking_queue< std::size_t >& queue, unsigned seed)
{
	std::mt19937 eng(seed);
	while (true)
	{
		unsigned i = eng();
		try
		{
			queue.push(i);
			std::println("[{}, s] -> {}", os::this_thread::get_id(), i);
		}
		catch (const std::exception& e)
		{
			std::println("[{}, s] stopped", os::this_thread::get_id());
			break;
		}
		if (i % 100 == 0) queue.stop();
	}
}
static void consumer(example::blocking_queue< std::size_t >& queue)
{
	while (true)
	{
		try
		{
			std::println("[{}, c] <- {}", os::this_thread::get_id(), queue.pop());
		}
		catch (const std::exception& e)
		{
			std::println("[{}, c] stopped", os::this_thread::get_id());
			break;
		}
	}
}

int main()
{
	example::blocking_queue< std::size_t > queue{10};
	std::mt19937 eng;
	std::vector< os::thread > threads;
	for (std::size_t i = 0; i < 3; i++)
		threads.emplace_back(&supplier, std::ref(queue), eng());
	for (std::size_t i = 0; i < 2; i++)
		threads.emplace_back(&consumer, std::ref(queue));

	for (os::thread& thread: threads) thread.join();
}
