#include <iostream>
#include <chrono>
#include <vector>
#include <thread>
#include <mutex>

using namespace std;
using namespace std::chrono;

// # 낙천적 동기화(Optimistic synchronization)
// ## 세밀한 동기화의 경우 잠금의 획득과 해제가 너무 빈번하다.
// ### 리스트가 길어지는 경우 성능이 매우 떨어진다.
// ## 해결
// ### 이동 시 잠금을 하지 않는다.
// #### 잠금을 획득하지 않고 검색한 후, pre와 cur을 잠그고, 잠긴 노드가 정확한지 확인한다.
// ##### 드문 경우이지만, 만일 엉뚱한 노드가 잠길 경우 잠금을 해제하고 다시 시작한다.
// ##### 2개의 노드를 잠그므로 DeadLock 주의 !!

// ## 불안요쇼
// ### 잠금을 획득하지 않고 이동할 경우 삭제된 노드로 이동하는 경우가 생기지 않을까?
// ## 해결책
// ### 제거된 노드를 통해서 이동하는 것을 허용하라.
// #### 제거: remove, 삭제: delete
// ### Remove()시 Node를 delete하지 않는다!!!
// #### -delete하면 next가 어떤 값으로 바꾸리지 알 수 없다.
// #### - 정확성은 보장할 수 없지만 안정성은 보장할 수 있다.
// ### 이동 종료 후, pre와 next가 제대로 자리를 잡았는지 검사해야 한다.
// #### - remove된 node를 거친 이동은 잘못된 검색 결과를 야기할 수 있다.
// #### 아니면? 처음부터 다시;;;;

// ## validate() : 유효성 검사
// ### 다시 처음부터 이동해서 원래 pre, cur로 다시 올 수 있는지 확인한다.
// ### pre->next == curr인 것을 확인한다.

struct Node
{
    int key;
    Node* next;

    mutex lock;

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


class OSet
{
private:
    Node head, tail;

    //mutex glock;
    //null_mutex glock;

public:
    OSet()
    {
        head.key =  INT_MIN; //0x80000000; // -MAXINT
        tail.key =  INT_MAX; //0x7FFFFFFF; //  MAXINT
        head.next = &tail;
    }

    ~OSet() {};

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

    bool Validate(Node* pre, Node* cur)
    {
        Node* node = &head;

        while(node->key <= pre->key)
        {
            if(node == pre)
                return pre->next == cur;

            node = node->next;
        }

        return false;
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

        cur = pre->next;

        while(cur->key < key)
        {
            pre = cur;
            cur = cur->next;
        }

        // 락
        pre->lock.lock();
        cur->lock.lock();

        if(Validate(pre, cur))
        {
            if(key == cur->key)
            {
                pre->lock.unlock();
                cur->lock.unlock();

                return false;
            }
            else
            {
                Node* node = new Node(key);

                node->next = cur;
                pre->next = node;

                pre->lock.unlock();
                cur->lock.unlock();

                return true;
            }
        }

        pre->lock.unlock();
        cur->lock.unlock();

        return false;

    }

    bool Remove(int key)
    {
        Node* pre, * cur;

        pre = &head;

        cur = pre->next;

        while(cur->key < key)
        {
            pre = cur;
            cur = cur->next;
        }

        // 락
        pre->lock.lock();
        cur->lock.lock();

        if(Validate(pre, cur))
        {
            if(key == cur->key)
            {
                pre->next = cur->next;

                pre->lock.unlock();
                cur->lock.unlock();

                return true;
            }
            else
            {
                pre->lock.unlock();
                cur->lock.unlock();

                return false;
            }
        }

        pre->lock.unlock();
        cur->lock.unlock();

        return false;
    }

    bool Contains(int key)
    {
        Node* pre, * cur;

        pre = &head;

        cur = pre->next;


        while(cur->key < key)
        {
            pre = cur;
            cur = cur->next;
        }

        // 락
        pre->lock.lock();
        cur->lock.lock();

        if(Validate(pre, cur))
        {
            if(key == cur->key)
            {
                pre->lock.unlock();
                cur->lock.unlock();

                return true;
            }
            else
            {
                pre->lock.unlock();
                cur->lock.unlock();

                return false;
            }
        }

        pre->lock.unlock();
        cur->lock.unlock();

        return false;
    }


};


static const int NUM_TEST   =  4000000;
static const int KEY_RANGE  =     1000;

OSet my_set;

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