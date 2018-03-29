#include <iostream>
#include <thread>
#include <vector>
#include <mutex>

int t_count;

std::mutex m;


volatile bool flag[2] = {false, false};
volatile int victim;

int p_lock(int myid)
{
    int other = 1 - myid;

    flag[myid] = true;

    victim = myid;

    while(flag[other] && (victim == myid));
}

void p_unlock(int myid)
{
    flag[myid] = false;
}

void thread_func(int myid)
{
    for(int i = 0; i < 500000000 / t_count; ++i)
    {
        m.lock();
        p_lock(myid);
        sum = sum += 2;
        p_lock(myid);
        m.unlock();
    }
}

int main()
{
    std::cout << "Hello, World!" << std::endl;

    std::vector<std::thread*> t_v;






    return 0;
}