#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>

using namespace std;
using namespace std::chrono;

mutex myLock;

volatile int sum;

int t_count = 0;

void thread_func()
{
    for(auto i = 0; i < 500000000 / t_count; ++i)
    {
        sum = sum + 2;
        //_asm add sum, 2;
    }
}

int main()
{
    char ch;
    cin >> ch;

    while(true)
    {
        sum = 0;

        thread t1 { thread_func };
        thread t2 { thread_func };

        t1.join();
        t2.join();

        std::cout << "Sum: " << sum << std::endl;
    }
    return 1;
}