#pragma once

#include <vector>
#include <thread>
#include <future>

#include "common_objs.h"


/* This is the parallel version of for_each function implementation with package tasks and futures */
template<typename Iterator, typename Func>
void parallel_for_each_pt( Iterator first, Iterator last, Func f )
{
	size_t const length = std::distance(first, last);

	if (!length)
		return;

	/*	Calculate the optimized number of threads to run the algorithm	*/

	constexpr size_t min_per_thread = 25;
	size_t const max_threads = (length + min_per_thread - 1) / min_per_thread;

	size_t const hardware_threads = std::thread::hardware_concurrency();
	size_t const num_threads = std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);
	size_t const block_size = length / num_threads;

	/*	Declare the needed data structures	*/
	
	std::vector<std::future<void>> futures(num_threads - 1);
	std::vector<std::thread> threads(num_threads - 1);
	join_threads joiner(threads);

	/*	Partition of data between threads	*/
	
	Iterator block_start = first;
	for ( unsigned long i =0; i <= ( num_threads -2 ); i++ )
	{
		Iterator block_end = block_start;
		std::advance(block_end, block_size);
		
		std::packaged_task<void()> task(
			[=]()
			{
				std::for_each(block_start, block_end, f);
			}
		);

		futures[i] = task.get_future();
		threads[i] = std::thread(std::move(task));

		block_start = block_end;
	}

	// call the function for last block from this thread
	std::for_each(block_start, last, f);

	/*	wait until futures are ready	*/
	for (unsigned long i = 0; i < (num_threads - 1); ++i)
		futures[i].get();

}

/* This is the parallel version of for_each function implementation with std::async */
template<typename Iterator, typename Func>
void parallel_for_each_async(Iterator first, Iterator last, Func f)
{
	size_t const length = std::distance(first,last);
	
	if (!length)
		return;

	if (constexpr size_t min_per_thread = 25; length < 2 * min_per_thread)
	{
		std::for_each(first, last, f);
	}
	else
	{
		Iterator const mid_point = first + length / 2;
		std::future<void> first_half = 
			std::async(&parallel_for_each_async<Iterator, Func>, first, mid_point, f);

		parallel_for_each_async(mid_point, last, f);
		first_half.get();
	}

}