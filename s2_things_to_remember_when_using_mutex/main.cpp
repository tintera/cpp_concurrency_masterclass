#include <iostream>
#include <mutex>
#include <list>

/*********************** example 1 *********************/
class list_wrapper {
	std::list<int> my_list;
	std::mutex m;

public:
	void add_to_list(int const& x)
	{
		std::lock_guard<std::mutex> lg(m);
		my_list.push_front(x);
	}

	void size()
	{
		std::lock_guard<std::mutex> lg(m);
		const size_t size = my_list.size();
		std::cout << "size of the list is : " << size << std::endl;
	}

	std::list<int>* get_data()
	{
		return &my_list;
	}
};

/*********************** example 2 *********************/
class data_object {

public:
	// ReSharper disable once CppMemberFunctionMayBeStatic
	void some_operation()
	{
		std::cout << "this is some operation \n";
	}
};

class data_wrapper {

	data_object protected_data;
	std::mutex m;

public:
	template <typename function>
	void do_some_work(function f)
	{
		std::lock_guard<std::mutex> lg(m);
		f(protected_data);
	}
};

data_object* unprotected_data;

void malicious_function(data_object& data)
{
	unprotected_data = &data;
}

void run_code()
{
	data_wrapper wrapper;
	wrapper.do_some_work(malicious_function);
}


int main()
{
	run_code();
}