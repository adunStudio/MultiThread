#include <iostream>
#include <thread>

using namespace std;

void f()
{
    std::cout << "f()\n" << std::endl;
}

struct F
{
    void operator()()
    {
        std::cout << "F()\n" << std::endl;
    }
};

int main()
{
    thread t1 {f};
    thread t2 {F()};
    thread t3 {[]() {
        std::cout << "Lambda\n" << std::endl;
    }};

    t1.join();
    t2.join();
    t3.join();

    return 1;
}