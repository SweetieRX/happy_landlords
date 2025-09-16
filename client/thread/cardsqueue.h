#ifndef CARDSQUEUE_H
#define CARDSQUEUE_H

#include "cards.h"
#include <QMutex>
#include <QObject>
#include <QQueue>
#include <QWaitCondition>

// 任务类型

// 饿汉模式
class CardsQueue : public QObject
{
    Q_OBJECT
public:
    CardsQueue(const CardsQueue& t) = delete;
    CardsQueue& operator=(const CardsQueue& t) = delete;
    static CardsQueue* getInstance();
    // 1. 添加任务
    void add(Cards &cards);
    // 2. 取出一个任务
    bool take(Cards &cards);

    // 3. 得到任务队列中任务的数量
    int size();
    // 4. 清空任务队列
    void clear();
    // 5. 释放条件锁
    void release();

private:
    explicit CardsQueue(QObject *parent = nullptr);

private:
    static CardsQueue m_taskQ;
    QQueue<Cards> m_queue;
    QMutex m_mutex;
    QWaitCondition m_cond;
    bool m_flag = true;

signals:
};

#endif
