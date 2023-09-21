#include <iostream>
#include <mutex>
#include <thread>
#include <string>

using namespace std::string_literals;


/***************************************************Example 1 ******************************************/
class bank_account {  // NOLINT(cppcoreguidelines-special-member-functions)
	double balance;
	std::string name;
	std::mutex m;

public:
	bank_account() = default;

	bank_account(const double _balance, std::string _name) :balance(_balance), name(std::move(_name)) {}

	bank_account(bank_account& ) = delete;
	bank_account& operator=(bank_account& ) = delete;

	void withdraw(const double amount)
	{
		std::lock_guard<std::mutex> lg(m);
		balance += amount;
	}

	void deposit(const double amount)
	{
		std::lock_guard<std::mutex> lg(m);
		balance += amount;
	}

	// ReSharper disable once CppMemberFunctionMayBeStatic // static function can not be run in a thread
	void transfer(bank_account& from, bank_account& to, const double amount)
	{

		std::cout << std::this_thread::get_id() << " hold the lock for both mutex \n";

		std::unique_lock<std::mutex> ul_1(from.m, std::defer_lock);
		std::unique_lock<std::mutex> ul_2(to.m, std::defer_lock);
		std::lock(ul_1, ul_2);

		from.balance -= amount;
		to.balance += amount;
		std::cout << "transfer to - " << to.name << "   from - " << from.name << "  end \n";
	}
};

void run_code1()
{

	bank_account account;

	bank_account account_1(1000, "James"s);
	bank_account account_2(2000, "Mathew"s);

	std::thread thread_1(&bank_account::transfer, &account, std::ref(account_1), std::ref(account_2), 500);
	std::thread thread_2(&bank_account::transfer, &account, std::ref(account_2), std::ref(account_1), 200);

	thread_1.join();
	thread_2.join();
}



/***************************************************Example 2 ******************************************/

void x_operations()
{
	std::cout << "this is some operations \n";
}

void y_operations()
{
	std::cout << "this is some other operations \n";
}

std::unique_lock<std::mutex> get_lock()
{
	std::mutex m;
	std::unique_lock<std::mutex> lk(m);
	x_operations();
	return lk;
}

void run_code2()
{
	std::unique_lock<std::mutex> lk(get_lock());
	y_operations();
}

int main()
{
	run_code1();
	run_code2();

	return 0;
}