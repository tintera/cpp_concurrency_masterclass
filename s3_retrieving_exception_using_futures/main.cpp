#include <iostream>       
#include <thread>         
#include <future>         
#include <stdexcept>   

[[ noreturn ]] void throw_exception()
{
	throw  std::invalid_argument("input cannot be negative");
}

void calculate_square_root(std::promise<int>& prom)
{
	std::cout << "Please, enter an integer value: ";
	try
	{
		int x = 1;
		std::cin >> x;
		if (x < 0)
		{
			throw_exception();
		}
		prom.set_value(static_cast<int>(std::sqrt(x)));
	}
	catch (std::exception&)
	{
		prom.set_exception(std::current_exception());
	}
}

void print_result(std::future<int>& fut) {
	try
	{
		const int x = fut.get();
		std::cout << "value: " << x << '\n';
	}
	catch (std::exception& e) {
		std::cout << "[exception caught: " << e.what() << "]\n";
	}
}

int main()
{
	std::promise<int> prom;
	std::future<int> fut = prom.get_future();

	std::thread printing_thread(print_result, std::ref(fut));
	std::thread calculation_thread(calculate_square_root, std::ref(prom));

	printing_thread.join();
	calculation_thread.join();
}