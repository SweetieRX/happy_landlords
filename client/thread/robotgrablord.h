#ifndef ROBOTGRABLORD_H
#define ROBOTGRABLORD_H

#include <QRunnable>
#include <QObject>

class RobotGrabLord : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit RobotGrabLord(QObject *parent = nullptr);

protected:
    void run();

signals:
    void callLordRequested(int bet);

};

#endif // ROBOTGRABLORD_H
