#include <iostream>
#include <thread>

using namespace std;

int sum;

void thread_func()
{
    for(auto i = 0; i < 250000000; ++i)
    {
        sum += 2;
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