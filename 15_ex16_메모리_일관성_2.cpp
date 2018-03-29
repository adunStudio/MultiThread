#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>

using namespace std;
using namespace std::chrono;

volatile bool done = false;

volatile long* bound = nullptr;

int error;

void ThreadFunc1()
{
    for(int j = 0; j <= 25000000; ++j)
    {
        *bound = -(1 + *bound);
        //std::cout << *bound << std::endl;
    }

    done = true;
}

void ThreadFunc2()
{
    while(!done)
    {
        int v = *bound;

        if((v != 0) && (v != -1))
        {
            error++;

            std::cout << hex << "[" << v << "]" << std::endl;
        }
    }
}

int main()
{
    int arr[32];

    long temp = reinterpret_cast<long>(&arr[16]);

    temp = (temp / 64) * 64;
    temp -= 2;

    bound = reinterpret_cast<long *>(temp);

    *bound = 0;

    thread t1 { ThreadFunc1 };
    thread t2 { ThreadFunc2 };

    t1.join();
    t2.join();

    cout << "Error count : " << error << "\n";

    /*
    bound = new long {0};

    thread t1 { ThreadFunc1 };
    thread t2 { ThreadFunc2 };

    t1.join();
    t2.join();

    cout << "Error count : " << error << "\n";
    */

    return 1;
}