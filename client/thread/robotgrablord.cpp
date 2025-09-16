#include "robotgrablord.h"
#include "betqueue.h"
#include <QThread>

RobotGrabLord::RobotGrabLord(QObject *parent) : QObject(parent), QRunnable()
{
    setAutoDelete(true);
}

void RobotGrabLord::run()
{
    int bet;
    if(BetQueue::getInstance()->take(bet))
    {
        emit callLordRequested(bet);
    }

}
