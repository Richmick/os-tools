#pragma once

import std;
import os;

namespace example
{
	template< class T, class Alloc = std::allocator< T > >
	class blocking_queue
	{
	public:
		using value_type = T;
		using allocator_type = Alloc;
		using size_type = std::size_t;

		blocking_queue(size_type cap);
		~blocking_queue();

		void emplace(auto&&... args);
		void push(T&& value);
		void push(const T& value);
		value_type pop();
		void stop();
		bool stopped();

	private:
		using allocator_traits = std::allocator_traits< allocator_type >;

		os::mutex mutex_;
		os::condition_variable not_empty_, not_full_;
		allocator_type alloc_;
		value_type* buf_ = nullptr;
		bool stopped_ = false;
		const size_type capacity_ = 0;
		size_type head_ = 0, size_ = 0;
	};
}
template< class T, class Alloc >
example::blocking_queue< T, Alloc >::blocking_queue(size_type cap):
	alloc_(),
	buf_(allocator_traits::allocate(alloc_, cap)),
	capacity_(cap)
{}
template< class T, class Alloc >
example::blocking_queue<T, Alloc>::~blocking_queue()
{
	std::unique_lock lock{mutex_};
	while (size_ != 0)
	{
		value_type* ptr = buf_ + ((head_ + --size_) % capacity_);
		allocator_traits::destroy(alloc_, ptr);
	}
	allocator_traits::deallocate(alloc_, std::exchange(buf_, nullptr), capacity_);
}
template< class T, class Alloc >
void example::blocking_queue< T, Alloc >::emplace(auto&&... args)
{
	std::unique_lock lock{mutex_};
	while (!stopped_ && (size_ >= capacity_))
		not_full_.wait(lock);
	if (stopped_) throw std::runtime_error("queue was stopped");

	allocator_traits::construct(alloc_, buf_ + ((head_ + size_++) % capacity_),
				std::forward< decltype(args) >(args)...);
	lock.unlock();
	not_empty_.notify_one();
}
template< class T, class Alloc >
void example::blocking_queue< T, Alloc >::push(T&& value)
{
	emplace(std::move(value));
}
template< class T, class Alloc >
void example::blocking_queue< T, Alloc >::push(const T& value)
{
	emplace(value);
}
template< class T, class Alloc >
T example::blocking_queue< T, Alloc >::pop()
{
	std::unique_lock lock{mutex_};
	while (!stopped_ && (size_ == 0))
		not_empty_.wait(lock);

	if (stopped_) throw std::runtime_error("queue was stopped");
	value_type* ptr = buf_ + ((head_ + --size_) % capacity_);
	value_type result = std::move(*ptr);
	allocator_traits::destroy(alloc_, ptr);
	lock.unlock();
	not_full_.notify_one();
	return result;
}
template< class T, class Alloc >
void example::blocking_queue< T, Alloc >::stop()
{
	std::unique_lock lock{mutex_};
	stopped_ = true;
	lock.unlock();
	not_empty_.notify_all();
	not_full_.notify_all();
}
template< class T, class Alloc >
bool example::blocking_queue< T, Alloc >::stopped()
{
	return stopped_;
}
