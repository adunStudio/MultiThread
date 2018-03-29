#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>

// No Lock
//  1: 116
//  2: 62
//  4: 49
// 16: 43

// With Lock
//  1: 3,226
//  2: 15,642
//  4: 11,245
// 16: 14,191

// With Atomic
//  1: 379
//  2: 555
//  4: 568
// 16: 566

using namespace std;
using namespace std::chrono;

std::mutex m;

std::atomic<int> sum;

int t_count;

void ThreadFunc()
{

    for(auto i = 0; i < 500000 / t_count; ++i)
    {
        m.lock();

        sum += 2;

        m.unlock();
    }


}

int main()
{
    vector<thread*> t_v;

    for(t_count = 1; t_count <= 16; t_count *= 2)
    {
        sum = 0;

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

// No Lock
//  1: 116
//  2: 62
//  4: 49
// 16: 43

// With Lock
//  1: 3,226
//  2: 15,642
//  4: 11,245
// 16: 14,191

// With Atomic
//  1: 379
//  2: 555
//  4: 568
// 16: 566