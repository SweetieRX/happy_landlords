#include "robotplayhand.h"
#include "cardsqueue.h"
#include <QDebug>
#include <QThread>

RobotPlayHand::RobotPlayHand(QObject *parent) : QObject(parent), QRunnable()
{
    setAutoDelete(true);
}

void RobotPlayHand::run()
{
    Cards cards;
    if (CardsQueue::getInstance()->take(cards))
    {
        qDebug() << QThread::currentThreadId()<< "线程中取出了Cards++++++++++++++++++" ;
        emit PlayHandRequested(cards);
    }
}
