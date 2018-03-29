#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>

using namespace std;
using namespace std::chrono;

int NUM_OF_THREAD = 1;

volatile int sum;

mutex locker;

void AddFunc()
{
    volatile int sum_buf = 0;

    for(int i = 0; i < 500000000 / NUM_OF_THREAD; ++i)
    {
        sum_buf += 2;
    }

    locker.lock();
    sum += sum_buf;
    locker.unlock();
}

int main()
{

    // Non Thread
    sum = 0;
    cout << "Non Thread" << std::endl;
    cout << "////////////////////////////////////" << endl;
    NUM_OF_THREAD = 1;
    auto start = high_resolution_clock::now();
    AddFunc();
    auto end = high_resolution_clock::now() - start;
    cout << "Result: " << sum;
    cout << " Time: " << duration_cast<milliseconds>(end).count() << " millised " << endl;
    cout << "////////////////////////////////////" << endl;


    // Thread with Pointer
    sum = 0;
    NUM_OF_THREAD = 8;
    vector<thread*> thread_vec_pointer;
    start = high_resolution_clock::now();
    cout << "Thread with Pointer" << std::endl;
    cout << "////////////////////////////////////" << endl;
    for(int i = 0; i < NUM_OF_THREAD; ++i)
    {
        thread_vec_pointer.push_back(new thread(AddFunc));
    }
    for(int i = 0; i < NUM_OF_THREAD; ++i)
    {
        thread_vec_pointer[i]->join();
        delete thread_vec_pointer[i];
    }
    thread_vec_pointer.clear();
    end = high_resolution_clock::now() - start;
    cout << "Result: " << sum;
    cout << " Time: " << duration_cast<milliseconds>(end).count() << " millised " << endl;
    cout << "////////////////////////////////////" << endl;


    // Thread with Value
    sum = 0;
    NUM_OF_THREAD = 8;
    vector<thread> thread_vec;
    start = high_resolution_clock::now();
    cout << "Thread with Value" << std::endl;
    cout << "////////////////////////////////////" << endl;
    for(int i = 0; i < NUM_OF_THREAD; ++i)
    {
        thread_vec.push_back(thread(AddFunc));
    }
    for(int i = 0; i < NUM_OF_THREAD; ++i)
    {
        thread_vec[i].join();
    }
    thread_vec.clear();
    end = high_resolution_clock::now() - start;
    cout << "Result: " << sum;
    cout << " Time: " << duration_cast<milliseconds>(end).count() << " millised " << endl;
    cout << "////////////////////////////////////" << endl;

    return 1;
}