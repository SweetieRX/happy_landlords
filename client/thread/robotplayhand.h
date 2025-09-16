#ifndef ROBOTPLAYHAND_H
#define ROBOTPLAYHAND_H
#include "cards.h"
#include <QObject>
#include <QRunnable>

class RobotPlayHand : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit RobotPlayHand(QObject *parent = nullptr);

protected:
    void run() override;
signals:
    void PlayHandRequested(const Cards& cards);

};

#endif // ROBOTPLAYHAND_H
