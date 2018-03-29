#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>

using namespace std;
using namespace std::chrono;

volatile int sum = 0;

std::atomic<int> X;

int t_count;

bool CAS(atomic_int* addr, int expected, int new_val)
{
    return atomic_compare_exchange_strong(addr, &expected, new_val);
}
bool CAS(volatile int* addr, int expected, int new_val)
{
    return atomic_compare_exchange_strong( reinterpret_cast<volatile atomic_int*>(addr), &expected, new_val);
}

void Lock()
{
    while(!CAS(&X, 0, 1));
    return;
}

void UnLock()
{
    CAS(&X, 1, 0);
    //while(!CAS(&X, 1, 0));
    //return;
}

void ThreadFunc()
{

    for(auto i = 0; i < 50000000 / t_count; ++i)
    {
        Lock();
        sum += 2;
        UnLock();
    }
}

int main()
{
    vector<thread*> t_v;

    for(t_count = 1; t_count <= 16; t_count *= 2)
    {
        sum = 0;
        X = 0;

        auto start_t = high_resolution_clock::now();

        for(int i = 0; i < t_count; ++i)
        {
            t_v.push_back(new thread{ThreadFunc});
        }

        for(auto th : t_v)
        {
            th->join();
            delete th;
        }
        t_v.clear();

        auto duration = high_resolution_clock::now() - start_t;

        cout << "Thread num: " << t_count << endl;
        cout << "Sum: " << sum << endl;
        cout << "Time: " << duration_cast<milliseconds>(duration).count() << endl;
    }

    return 1;
}