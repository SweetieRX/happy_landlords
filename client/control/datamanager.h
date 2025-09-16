#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QByteArray>
#include "cards.h"
#include "communication.h"

// 饿汉模式
class DataManager
{
public:
    enum GameMode{Single, Network};
    enum RoomMode{Auto, Manual};
    DataManager(const DataManager&) = delete;
    DataManager& operator=(const DataManager&) = delete;
    static DataManager* getInstance();

    // 设置数据
    void setUserName(QByteArray name);
    void setIP(QByteArray ip);
    void setPort(QByteArray port);
    void setCommunication(Communication* comm);
    void setRoomName(QByteArray roomName);
    void setCards(Cards cs, Cards last);
    void setGameMode(GameMode mode);
    void setRoomMode(RoomMode mode);
    void setIsRestart(bool flag);
    // 获取数据
    QByteArray getUserName();
    QByteArray getIP();
    QByteArray getPort();
    QByteArray getRoomName();
    Communication* getCommunication();
    Cards getCards();
    Cards getLast3Cards();
    bool isNetworkMode();
    bool isManualMode();
    bool isRestart();
private:
    DataManager() = default;
    static DataManager m_data;

    QByteArray m_userName;
    QByteArray m_roomName;
    //QByteArray m_ip = "119.27.173.73";
    QByteArray m_ip = "47.101.154.215";
    QByteArray m_port = "8888";
    Communication* m_commun;
    Cards m_cs;
    Cards m_last;
    GameMode m_mode = Single;
    RoomMode m_roomMode;
    bool m_isRestart = false;
};

#endif // DATAMANAGER_H
