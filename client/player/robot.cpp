#include "robot.h"
#include "datamanager.h"
#include "strategy.h"
#include "robotgrablord.h"
#include "robotplayhand.h"
#include <QDebug>
#include <QThreadPool>
#include <QTimer>

Robot::Robot(QObject *parent) : Player(parent)
{
    m_type = Player::Robot;
}

void Robot::prepareCallLord()
{
    if(DataManager::getInstance()->isNetworkMode())
    {
        RobotGrabLord* task = new RobotGrabLord();
        connect(task, &RobotGrabLord::callLordRequested,
                this, &Robot::netThinkCallLord, Qt::QueuedConnection);
        QThreadPool::globalInstance()->start(task);
    }
    else
    {
        QTimer::singleShot(2000, this, [=]() {
            thinkCallLord();
        });
    }
}

void Robot::preparePlayHand()
{
    qDebug() << this->getName() <<"准备出牌" << "==================";
    if(DataManager::getInstance()->isNetworkMode())
    {
        RobotPlayHand* task = new RobotPlayHand();
        connect(task, &RobotPlayHand::PlayHandRequested,
                this, &Robot::netThinkPlayHand, Qt::QueuedConnection);
        QThreadPool::globalInstance()->start(task);
    }
    else
    {
        QTimer::singleShot(2000, this, [=]() {
            thinkPlayHand();
        });
    }
}


void Robot::thinkCallLord()
{

    /*
     * 基于手中的牌计算权重
     * 大小王: 6
     * 顺子/炸弹: 5
     * 三张点数相同的牌: 4
     * 2的权重: 3
     * 对儿牌: 1
    */
    int weight = 0;
    Strategy st(this, m_cards);
    weight += st.getRangeCards(Card::Card_SJ, Card::Card_BJ).cardCount() * 6;

    QVector<Cards> optSeq = st.pickOptimalSeqSingles();
    weight += optSeq.size() * 5;

    QVector<Cards> bombs = st.findCardsByCount(4);
    weight += bombs.size() * 5;
    // 排除2是炸弹的情况, 避免重复计算权重
    if(m_cards.pointCount(Card::Card_2) < 4)
    {
        weight += m_cards.pointCount(Card::Card_2) * 3;
    }

    Cards tmp = m_cards;
    tmp.remove(optSeq);
    tmp.remove(bombs);
    Cards card2 = st.getRangeCards(Card::Card_2, Card::Card_2);
    tmp.remove(card2);
    QVector<Cards> triples = Strategy(this, tmp).findCardsByCount(3);
    weight += triples.size() * 4;

    tmp.remove(triples);
    QVector<Cards> pairs = Strategy(this, tmp).findCardsByCount(2);
    weight += pairs.size() * 1;

    if(weight >= 22)
    {
        grabLordBet(3);
    }
    else if(weight < 22 && weight >=18)
    {
        grabLordBet(2);
    }
    else if(weight < 18 && weight >= 10)
    {
        grabLordBet(1);
    }
    else
    {
        grabLordBet(0);
    }

}

void Robot::thinkPlayHand()
{

    Strategy st(this, m_cards);
    Cards cs = st.makeStrategy();
    playHand(cs);
}
