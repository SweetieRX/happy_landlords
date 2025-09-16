#ifndef BETQUEUE_H
#define BETQUEUE_H

#include <QMutex>
#include <QObject>
#include <QQueue>
#include <QWaitCondition>

// 任务类型

// 饿汉模式
class BetQueue : public QObject
{
    Q_OBJECT
public:
    BetQueue(const BetQueue& t) = delete;
    BetQueue& operator=(const BetQueue& t) = delete;
    static BetQueue* getInstance();
    // 1. 添加任务
    void add(int &bet);
    // 2. 取出一个任务
    bool take(int &bet);

    // 3. 得到任务队列中任务的数量
    int size();
    // 4. 清空任务队列
    void clear();
    // 5. 释放条件锁
    void release();

private:
    explicit BetQueue(QObject *parent = nullptr);

private:
    static BetQueue m_taskQ;
    QQueue<int> m_queue;
    QMutex m_mutex;
    QWaitCondition m_cond;
    bool m_flag = true;

signals:
};

#endif
