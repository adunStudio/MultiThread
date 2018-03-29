#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>

using namespace std;
using namespace std::chrono;

volatile bool flag = false;

int input_data = 0;

volatile int* volatile a;

void ThreadFunc1()
{
    while(flag == false);

    int my_data = input_data;

    cout << "I received [" << input_data << "]\n";
}

void ThreadFunc2()
{
    input_data = 1;

    flag = true;

    cout << "I sent [" << input_data << "]\n";
    cout << "My flag is [" << flag << "]\n";
}

int main()
{
    thread t1 {ThreadFunc1};
    thread t2 {ThreadFunc2};

    t1.join();
    t2.join();

    return 1;
}