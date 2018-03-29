#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>

using namespace std;
using namespace std::chrono;

mutex mylock;

//volatile int sum;

std::atomic <int> sum;

int t_count = 0;

void thread_func()
{
    for(auto i = 0; i < 500000000; ++i)
    {
        // mylock.lock();
        // _adm lock add sum, 2;
        sum += 2;
        //sum = sum + 2;   // <atomic> + <atomic> != <atomic>
        // mylock.lock();
    }
}

int main()
{
    vector<thread*> t_v;

    for(t_count = 1; t_count <= 16; t_count *= 2)
    {
        sum = 0;

        auto start_t = high_resolution_clock::now();

        for(auto i = 0; i < t_count; ++i)
        {
            t_v.push_back(new thread {thread_func} );
        }

        for(auto th : t_v)
        {
            th->join();
            delete th;
        }

        t_v.clear();

        auto du = high_resolution_clock::now() - start_t;

        cout << "Time: " << duration_cast<milliseconds>(du).count();
        cout << " Sum: " << sum << endl;
    }
}