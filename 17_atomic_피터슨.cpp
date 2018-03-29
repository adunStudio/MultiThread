#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>

using namespace std;
using namespace std::chrono;

atomic<int>  victim;
atomic<bool> flag[2];// = { false, false };

volatile int sum = 0;

std::mutex my_lock;

int t_count = 0;

void Lock(int myID)
{
    int other = 1 - myID;

    flag[myID] = true;

    victim = myID;

    while(flag[other] && victim == myID);
}

void UnLock(int myID)
{
    flag[myID] = false;
}

void ThreadFunc1(int myID)
{
    for(int i = 0; i < 50000000 / t_count; ++i)
    {
        // my_lock.lock();
        // this_thread::get_id(); // 이런게 있긴한데 괴상한 값으로 운영체제가 관리한다.

        Lock(myID);
        sum += 2;
        UnLock(myID);

        // my_lock.unlock();
    }
}

void ThreadFunc2(int myID)
{
    for(int i = 0; i < 50000000 / t_count; ++i)
    {
        Lock(myID);
        sum += 2;
        UnLock(myID);
    }
}

int main()
{
    vector<thread*> t_v;

    for(t_count = 1; t_count <= 16; t_count *= 2)
    {
        sum = 0;
        flag[0] = false; flag[1] = false;

        auto start_t = high_resolution_clock::now();

        for(int i = 0; i < t_count; ++i)
        {
            t_v.push_back(new thread {ThreadFunc1, i});
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

    return 1;
}
