#pragma once

class Bakery
{
private:
    int m_threadCount;

    bool m_flag[16]; // 최대 쓰레드 개수 16

    int m_label[16];

private:
    int Max()
    {
        int max = m_label[0];

        for (int i = 0; i < m_threadCount; ++i)
            if (m_label[i] > max) max = m_label[i];

        return max;
    }

public:
    Bakery(volatile int threadCount)
            : m_threadCount(threadCount)
    {
        for (int i = 0; i < m_threadCount; ++i)
        {
            m_flag[i] = false;
            m_label[i] = 0;
        }
    }

    void Lock(int id)
    {
        m_flag[id] = true;

        // 표 뽑기
        m_label[id] = Max() + 1;

        m_flag[id] = false;

        // 표를 뽑은 후
        for (int i = 0; i < m_threadCount; ++i)
        {
            while (m_flag[i]);

            while ((m_label[i] != 0) && ((m_label[i] < m_label[id]) || (m_label[i] == m_label[id] && i < id)));
        }
    }

    void UnLock(int id)
    {
        m_label[id] = 0;
    }
};