#include <iostream>
#include <future>
#include <chrono>
#include <string>

constexpr int testSize = 1000;

using std::chrono::high_resolution_clock;
using std::milli;

void function3()
{   // NOLINT(clang-diagnostic-missing-noreturn)
	// with [[ noreturn ]] we would have unreachable code warning in the function1
	throw;
}


std::string function1(int i)
{
	function3();

	if (i < 100)
	{
		return "Hello";
	}

	return "Hi";
}



double function2( int i)
{
	std::string str = function1(i);
	
	if (i < 200)
	{
		return 3.98;
	}

	return 9.45;
}

int init_number(int i)
{
	function2(i);

	if (i < 1000)
	{
		// ReSharper disable once CppIdenticalOperandsInBinaryExpression
		i = i;  // NOLINT(clang-diagnostic-self-assign) // intentional
	}
	else
	{
		i = i * 2;
	}

	return i;
}

int main()
{

	std::vector<int> ints(testSize);
	for (int i = 0; i < testSize; i++)
	{
		ints[i] = init_number(i);
		std::cout << init_number(i) << " " << function1(i) << " " << function2(i);
	}

	return 0;
}