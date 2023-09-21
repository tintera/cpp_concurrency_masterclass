#include <future>
#include <chrono>
#include <execution>

#include "parallel_find.h"
#include "utils.h"

constexpr int testSize = 100'000'000;

using std::chrono::high_resolution_clock;
using std::milli;

int main()
{
	std::vector<int> ints(testSize);
	for (int i = 0; i < testSize; i++)
	{
		ints[i] = i;
	}

	constexpr int looking_for = 50'000'000;

	auto startTime = high_resolution_clock::now();
	auto value = parallel_find_pt(ints.begin(), ints.end(), looking_for);
	auto endTime = high_resolution_clock::now();
	print_results("Parallel-package_task_impl :", startTime, endTime);
#pragma warning(disable: 4834)
	startTime = high_resolution_clock::now();
	std::find(ints.begin(), ints.end(), looking_for);  // NOLINT(bugprone-unused-return-value)
	endTime = high_resolution_clock::now();
	print_results("STL sequential :", startTime, endTime);

	startTime = high_resolution_clock::now();
	std::find(std::execution::par,ints.begin(), ints.end(), looking_for);  // NOLINT(bugprone-unused-return-value)
	endTime = high_resolution_clock::now();
	print_results("STL parallel-par :", startTime, endTime);

	startTime = high_resolution_clock::now();
	std::find(std::execution::seq, ints.begin(), ints.end(), looking_for);  // NOLINT(bugprone-unused-return-value)
	endTime = high_resolution_clock::now();
	print_results("STL parallel-seq :", startTime, endTime);

	return 0;
}