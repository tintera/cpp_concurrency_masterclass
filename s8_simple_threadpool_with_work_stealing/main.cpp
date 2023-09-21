#include <iostream>
#include <future>

#include "simple_threadpool_with_work_stealing.h"

int main()
{
	constexpr int size = 800;
	std::list<int> my_array;

	srand(0);  // NOLINT(cert-msc51-cpp)

	for (size_t i = 0; i < size; i++)
	{
		my_array.push_back(rand());  // NOLINT(concurrency-mt-unsafe)
	}

	my_array = parallel_quick_sort(my_array);

	for (size_t i = 0; i < size; i++)
	{
		std::cout << my_array.front() << std::endl;
		my_array.pop_front();
	}
}