#include <boost/thread.hpp>
#include <boost/thread/barrier.hpp>
#include <boost/atomic.hpp>
#include <iostream>

void func(boost::barrier& cur_barrier, boost::atomic<int>& counter)
{
    ++counter;
    //std::cout  << counter << "\n";
    cur_barrier.wait();
    std::cout << "Current value after barrier :" << counter << "\n";
}

int main()
{
    boost::barrier bar(3);
    boost::atomic<int> current(0);
    boost::thread thr1([&bar, &current] { return func(bar, current); });
    boost::thread thr2([&bar, &current] { return func(bar, current); });
    boost::thread thr3([&bar, &current] { return func(bar, current); });
    boost::thread thr4([&bar, &current] { return func(bar, current); });
    boost::thread thr5([&bar, &current] { return func(bar, current); });
    boost::thread thr6([&bar, &current] { return func(bar, current); });
    thr1.join();
    thr2.join();
    thr3.join();
    thr4.join();
    thr5.join();
    thr6.join();
}