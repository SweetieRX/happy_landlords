#include "betqueue.h"
#include <QDebug>
#include <qthread.h>
BetQueue BetQueue::m_taskQ;
BetQueue::BetQueue(QObject *parent)
    : QObject{parent}
{}

BetQueue *BetQueue::getInstance()
{
    return &m_taskQ;
}

void BetQueue::add(int &t)
{
    m_mutex.lock();
    m_queue.enqueue(t);
    m_cond.wakeAll();
    m_mutex.unlock();
}

bool BetQueue::take(int &bet)
{
    m_flag = true;
    m_mutex.lock();
    while (m_queue.isEmpty() && m_flag)
    {
        m_cond.wait(&m_mutex);
    }
    if (!m_queue.isEmpty()) bet = m_queue.dequeue();

    m_mutex.unlock();
    return m_flag;
}

int BetQueue::size()
{
    m_mutex.lock();
    int size = m_queue.size();
    m_mutex.unlock();
    return size;
}

void BetQueue::clear()
{
    m_mutex.lock();
    m_queue.clear();
    m_mutex.unlock();
}

void BetQueue::release()
{
    m_flag = false;
    m_cond.wakeAll();
}
