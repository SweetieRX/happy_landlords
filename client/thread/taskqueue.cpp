#include "taskqueue.h"
#include <QDebug>
#include <qthread.h>
TaskQueue TaskQueue::m_taskQ;
TaskQueue::TaskQueue(QObject *parent)
    : QObject{parent}
{}

TaskQueue *TaskQueue::getInstance()
{
    return &m_taskQ;
}

void TaskQueue::add(Task &t)
{
    m_mutex.lock();
    m_queue.enqueue(t);
     qDebug() << "++++++++++放入的地主分数：" << t.bet;
    m_cond.wakeAll();
    m_mutex.unlock();

}

Task TaskQueue::take()
{
    Task t;
    qDebug() <<"Thread ID:" << QThread::currentThreadId() << "队列元素个数:" << size();
    m_mutex.lock();
    while (m_queue.isEmpty())
    {
        m_cond.wait(&m_mutex);
    }

    t = m_queue.dequeue();
    m_mutex.unlock();
     qDebug() << "++++++++++取出的地主分数：" << t.bet;

    return t;
}

int TaskQueue::size()
{
    m_mutex.lock();
    int size = m_queue.size();
    m_mutex.unlock();
    return size;
}

void TaskQueue::clear()
{
    m_mutex.lock();
    m_queue.clear();
    m_mutex.unlock();
}
