#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>

using namespace std;
using namespace std::chrono;

const auto SIZE = 50000000;

volatile int x, y;

int trace_x[SIZE], trace_y[SIZE];

void ThreadFunc0()
{
    for(int i = 0; i < SIZE; ++i)
    {
        x = i;
        trace_y[i] = y;
    }
}

void ThreadFunc1()
{
    for(int i = 0; i < SIZE; ++i)
    {
        y = i;
        trace_x[i] = x;
    }
}

int main()
{
    thread t0 { ThreadFunc0 };
    thread t1 { ThreadFunc1 };

    t0.join();
    t1.join();

    int count = 0;

    for(int i = 0; i < SIZE; ++i)
    {
        if(trace_x[i] == trace_x[i + 1])
        {
            if(trace_y[trace_x[i]] == trace_y[trace_x[i] + 1])
            {
                if(trace_y[trace_x[i]] != i) continue;
                count++;
            }
        }
    }

    cout << "Total Memory Inconsistency: " << count << endl;

    return 1;
}