#include <iostream>

#include "simple_thread_pool.h"

int main()
{
	thread_pool pool;
	std::cout << "Testing thread pool" << std::endl;

	for (int i = 0; i < 100; i++)
	{
		pool.submit([=] {
#pragma warning(disable: 4477)
			// ReSharper disable CppPrintfBadFormat
			printf(" %d printed by thread - %d \n", i, std::this_thread::get_id());  // NOLINT(clang-diagnostic-format)
		});
	}

	system("pause");  // NOLINT(concurrency-mt-unsafe)
}