#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>

#include "Bakery.h"

using namespace std;
using namespace std::chrono;

std::mutex m;

int sum;

int t_count;

Bakery* bakery;

void ThreadFunc(int id)
{
    for (auto i = 0; i < 50000000 / t_count; ++i)
    {
        //m.lock();
        bakery->Lock(id);

        sum += 2;

        bakery->UnLock(id);
        //m.unlock();
    }
}

int main()
{
    vector<thread*> t_v;

    for (t_count = 1; t_count <= 16; t_count *= 2)
    {
        sum = 0;

        bakery = new Bakery(t_count);

        auto start_t = high_resolution_clock::now();

        for (int i = 0; i < t_count; ++i)
        {
            t_v.push_back(new thread{ ThreadFunc, i });
        }

        for (auto th : t_v)
        {
            th->join();
            delete th;
        }

        t_v.clear();
        delete bakery;

        auto duration = high_resolution_clock::now() - start_t;

        cout << "Thread num: " << t_count << endl;
        cout << "Sum: " << sum << endl;
        cout << "Time: " << duration_cast<milliseconds>(duration).count() << endl;
    }

    return 1;
}