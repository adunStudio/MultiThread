#include <iostream>

using namespace std;

int sum;

int main()
{
    for(auto i = 0; i < 500000000; ++i)
    {
        sum += 2;
    }

    cout << "Sum: " << sum << endl;
}