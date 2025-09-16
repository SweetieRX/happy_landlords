#include "cardsqueue.h"
#include <QDebug>
#include <qthread.h>
CardsQueue CardsQueue::m_taskQ;
CardsQueue::CardsQueue(QObject *parent)
    : QObject{parent}
{}

CardsQueue *CardsQueue::getInstance()
{
    return &m_taskQ;
}

void CardsQueue::add(Cards &t)
{
    m_mutex.lock();
    m_queue.enqueue(t);
    m_cond.wakeAll();
    m_mutex.unlock();
    qDebug("+++++++++=从队列加入了牌++++++++++++");


}

bool CardsQueue::take(Cards &cards)
{
    m_flag = true;
    m_mutex.lock();
    while (m_queue.isEmpty() && m_flag)
    {
        m_cond.wait(&m_mutex);
    }
    if(!m_queue.isEmpty()) { cards = m_queue.dequeue(); qDebug("+++++++++=从队列取出牌++++++++++++");}
    m_mutex.unlock();
    qDebug("++++++++执行完take命令++++++++++++");
    return m_flag;
}

int CardsQueue::size()
{
    m_mutex.lock();
    int size = m_queue.size();
    m_mutex.unlock();
    return size;
}

void CardsQueue::clear()
{
    m_mutex.lock();
    m_queue.clear();
    m_mutex.unlock();
}

void CardsQueue::release()
{
    m_flag = false;
    m_cond.wakeAll();
     qDebug("释放锁");
}
