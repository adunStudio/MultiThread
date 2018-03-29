#include <iostream>
#include <thread>
#include <chrono>

using namespace std;
using namespace std::chrono;

volatile bool flag = false;

int data_all;

void ThreadFunc1()
{
    while(flag == false);

    int my_data = data_all;

    cout << "받았다. " << my_data << endl;
}

void ThreadFunc2()
{
    data_all = 1;

    flag = true;

    cout << "보냈다. " << data_all << endl;
}

int main()
{
    // auto start_time = high_resolution_clock::now();

    thread t1 {ThreadFunc1};
    thread t2 {ThreadFunc2};

    t1.join();
    t2.join();
}