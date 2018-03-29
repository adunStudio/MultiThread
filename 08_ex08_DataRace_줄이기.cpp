#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>

// 정답
//  1: 113
//  2: 61
//  4: 30
// 16: 33

using namespace std;
using namespace std::chrono;

mutex mylock;

std::atomic<int> sum;

int t_count = 0;

void thread_func()
{
    // volatile 쓰는 이유: 컴파일러가 코드 맘대로 수정하는 것을 막는다.

    volatile int local_sum = 0;

    for(auto i = 0; i < 500000000 / t_count; ++i)
        local_sum += 2;

    mylock.lock();
    sum += local_sum;
    mylock.unlock();
}

int main()
{
    vector<thread*> t_v;

    for(t_count = 1; t_count <= 16; t_count *= 2)
    {
        sum = 0;

        auto start_t = high_resolution_clock::now();

        for(auto i = 0; i < t_count; ++i)
            t_v.push_back(new thread {thread_func});

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