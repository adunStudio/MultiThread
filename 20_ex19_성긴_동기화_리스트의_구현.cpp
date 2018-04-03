#include <iostream>
#include <chrono>
#include <vector>
#include <thread>
#include <mutex>

using namespace std;
using namespace std::chrono;

// - SET
// 아이템의 중복을 허용하지 않는다.
// 검색의 효율성을 위해 아이템은 정렬되어 저장된다.
// 삽입 삭제의 효율성을 위해 링크드리스트로 구현된다.

// - 필드
// key: 리스트에 저장 되는 값
// next: 다음 노드의 포인터

// - 구현할 METHOD
// add(x)      : 집합에 x 추가, 성공시 true 반환
// remove(x)   : 집합에서 x 제거, 성송시 true 반환
// contains(x) : 집합에 x가 있다면 true 반환

// - 추가적인 구현
// 보초 노드(-MAXINT ~ MAXINT)
// 검색의 효율성을 위해 항상 존재하는 Head와 Tail노드를 갖도록 한다.
// Head는 -MAXINT, Tail은 MAXINT를 키로 갖는다.



// 1. 성긴 동기화 (coarse-grained synchronization)
// - Lock 하나로 동기화 객체 전체를 감싸는 경우
// - 구현 :
// 리스트는 하나의 잠금을 갖고 있으며, 모든 메서드 호출은 이 잠금을 통해 Critical Section으로 진행된다.

struct Node
{
    int key;
    Node* next;

    Node()
    {
        next = nullptr;
    }
    Node(int key_value)
    {
        next = nullptr;
        key = key_value;
    }

    ~Node() {};
};

class null_mutex
{
public:
    void lock() {};
    void unlock() {};
};
class CSet
{
private:
    Node head, tail;

    mutex glock;
    //null_mutex glock;

public:
    CSet()
    {
        head.key =  INT_MIN; //0x80000000; // -MAXINT
        tail.key =  INT_MAX; //0x7FFFFFFF; //  MAXINT
        head.next = &tail;
    }

    ~CSet() {};

    void Dump()
    {
        Node* ptr = head.next;

        cout << "Result Contains : ";
        for(int i  = 0; i < 20; ++i)
        {
            cout << ptr->key << ". ";
            if(&tail == ptr) break;
            ptr = ptr->next;
        }
        cout << endl;
    }

    void Init()
    {
        while(head.next != &tail)
        {
            Node* temp = head.next;
            head.next = temp->next;

            delete temp;
        }
    }

    bool Add(int key)
    {
        Node* pre, * cur;

        pre = &head;

        glock.lock();

        cur = pre->next;

        while(cur->key < key)
        {
            pre = cur;
            cur = cur->next;
        }

        if(key == cur->key)
        {
            glock.unlock();

            return false;
        }
        else
        {
            Node* node = new Node(key);

            node->next = cur;
            pre->next = node;

            glock.unlock();

            return true;
        }
    }

    bool Remove(int key)
    {
        Node* pre, * cur;

        pre = &head;

        glock.lock();

        cur = pre->next;

        while(cur->key < key)
        {
            pre = cur;
            cur = cur->next;
        }

        if(key == cur->key)
        {
            pre->next = cur->next;

            delete cur;

            glock.unlock();

            return true;
        }
        else
        {
            glock.unlock();

            return false;
        }
    }

    bool Contains(int key)
    {
        Node* pre, * cur;

        pre = &head;

        glock.lock();

        cur = pre->next;

        while(cur->key < key)
        {
            pre = cur;
            cur = cur->next;
        }

        if(key == cur->key)
        {
            glock.unlock();

            return true;
        }
        else
        {
            glock.unlock();

            return false;
        }
    }


};


static const int NUM_TEST   =  400000;
static const int KEY_RANGE  =    1000;

CSet my_set;

void benchmark(int num_thread)
{
    for(int i = 0; i < NUM_TEST / num_thread; ++i)
    {
        switch(rand() % 3)
        {
            case 0: my_set.Add     (rand() % KEY_RANGE); break;
            case 1: my_set.Remove  (rand() % KEY_RANGE); break;
            case 2: my_set.Contains(rand() % KEY_RANGE); break;
            default: cout << "Error!!!\n"; exit(-1);
        }
    }
}

int main()
{
    vector<thread> worker;

    for(int num_thread = 1; num_thread <= 16; num_thread *= 2)
    {
        my_set.Init();

        worker.clear();

        auto start_t =  high_resolution_clock::now();

        for(int i = 0; i < num_thread; ++i)
            worker.push_back(thread {benchmark, num_thread});

        for(auto& th : worker)
            th.join();

        auto du = high_resolution_clock::now() - start_t;

        my_set.Dump();

        cout << num_thread << " Threads, Time: ";
        cout << duration_cast<milliseconds>(du).count() << endl;
    }
}