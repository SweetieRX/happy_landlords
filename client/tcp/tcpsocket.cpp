#include "tcpsocket.h"
#include <QDebug>

TcpSocket::TcpSocket(QObject *parent) : QObject(parent)
{
#ifdef Q_OS_WIN
    WSADATA data;
    WSAStartup(MAKEWORD(2,2), &data);
#endif
}

TcpSocket::TcpSocket(QByteArray ip, unsigned short port, QObject *parent) : TcpSocket(parent)
{
    connectToHost(ip, port);
}

TcpSocket::~TcpSocket()
{
#ifdef Q_OS_WIN
    WSACleanup();
#endif
}

bool TcpSocket::connectToHost(QByteArray ip, unsigned short port)
{
    assert(port > 0 && port <= 65535);
    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    assert(m_socket);

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip.data());
    int ret = ::connect(m_socket, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr));
    bool flag = ret == 0 ? true : false;
    return flag;
}

// QByteArray TcpSocket::recvMsg(int timeout)
// {
//     while(true)
//     {
//         if (readTimeout(timeout))
//         {

//         }
//     }
//     bool flag = readTimeout(timeout);
//     QByteArray arry;
//     if(flag)
//     {
//         // 接收数据 = 数据头 + 数据块
//         int headLen = 0;
//         int ret = readn((char*)&headLen, sizeof(int));
//         if(ret < sizeof(int))
//         {
//             return QByteArray();
//         }
//         // qDebug() << "接收的数据块长度: " << headLen;
//         headLen = ntohl(headLen);
//         // qDebug() << "接收的数据块长度: " << headLen;

//         // 申请一块新内存
//         char* data = new char[headLen];
//         assert(data);

//         ret = readn(data, headLen);
//         if(ret == headLen)
//         {
//             // 数据正常
//             arry = QByteArray(data, headLen);
//         }
//         else
//         {
//             // 数据异常
//             arry = QByteArray();
//         }

//         delete[]data;
//     }
//     return arry;
// }

QByteArray TcpSocket::recvMsg(int timeout)
{
    if (m_recvBuffer.size() >= static_cast<int>(sizeof(int)))
    {
        int headLen;
        memcpy(&headLen, m_recvBuffer.constData(), sizeof(int));
        headLen = ntohl(headLen);

        // 3. 检查包长合法性
        if (headLen < 0)
        {
            qDebug() << "Invalid package size:" << headLen << ", clearing buffer.";
            m_recvBuffer.clear();
        }

        // 4. 检查是否有完整数据体
        if (m_recvBuffer.size() >= static_cast<int>(sizeof(int)) + headLen)
        {
            // 5. 提取并消费完整包
            QByteArray body = m_recvBuffer.mid(sizeof(int), headLen);
            m_recvBuffer = m_recvBuffer.mid(sizeof(int) + headLen);
            return body;
        }
    }
    if (readTimeout(timeout))
    {
        char tempBuf[40960];
        int ret = recv(m_socket, tempBuf, sizeof(tempBuf), 0);

        if (ret <= 0)
        {
            qDebug() << "recv failed";
        }

        // 2. 累积数据
        m_recvBuffer.append(tempBuf, ret);
    }

    return QByteArray();
}

bool TcpSocket::sendMsg(QByteArray msg, int timeout)
{
    bool flag = writeTimeout(timeout);
    if(flag)
    {
        // 发送数据 = 数据头 + 数据块
        int headLen = htonl(msg.size());

        // 申请一块新内存
        int length = sizeof(int) + msg.size();
        char* data = new char[length];
        assert(data);
        memcpy(data, &headLen, sizeof(int));
        memcpy(data + sizeof(int), msg.data(), msg.size());

        int ret = writen(data, length);
        flag = ret == length ? true : false;

        delete[]data;
    }
    // 发送数据 = 数据头 + 数据块
    // int headLen = htonl(msg.size());

    // // 申请一块新内存
    // int length = sizeof(int) + msg.size();
    // char* data = new char[length];
    // assert(data);
    // memcpy(data, &headLen, sizeof(int));
    // memcpy(data + sizeof(int), msg.data(), msg.size());

    // int ret = writen(data, length);

    // delete[]data;
    // return ret == length ? true : false;
    return flag;
}

void TcpSocket::disConnect()
{
    if(m_socket)
    {
#ifdef Q_OS_WIN
        closesocket(m_socket);
#endif
#ifdef Q_OS_LINUX
        close(m_socket);
#endif
    }
}

bool TcpSocket::readTimeout(int timeout)
{
    if(timeout == -1)
    {
        return true;    // 阻塞读数据
    }

    // 检测读缓冲区
#ifdef Q_OS_WIN
    int nfds = 0;
#endif
#ifdef Q_OS_LINUX
    int nfds = m_socket + 1;
#endif
    fd_set rdset;
    FD_ZERO(&rdset);
    FD_SET(m_socket, &rdset);
    struct timeval tmout;
    tmout.tv_sec = timeout;
    tmout.tv_usec = 0;
    int ret = select(nfds, &rdset, NULL, NULL, &tmout);
    bool flag = ret > 0 ? true : false;
    return flag;
}

bool TcpSocket::writeTimeout(int timeout)
{
    if(timeout == -1)
    {
        return true;    // 阻塞写数据
    }

// 检测读写冲区
#ifdef Q_OS_WIN
    int nfds = 0;
#endif
#ifdef Q_OS_LINUX
    int nfds = m_socket + 1;
#endif
    fd_set wset;
    FD_ZERO(&wset);
    FD_SET(m_socket, &wset);
    struct timeval tmout;
    tmout.tv_sec = timeout;
    tmout.tv_usec = 0;
    int ret = select(nfds, NULL, &wset, NULL, &tmout);
    bool flag = ret > 0 ? true : false;
    return flag;
}

int TcpSocket::readn(char *buf, int count)
{
    int last = count;   // 剩余的字节数
    int size = 0;       // 每次读出的字节数
    char* pt = buf;
    while(last > 0)
    {
        if((size = recv(m_socket, pt, last, 0)) < 0)
        {
#ifdef Q_OS_WIN
            int err = WSAGetLastError();
            qDebug() << "recv error:" << err;
#else
            perror("recv");
#endif
            return -1;
        }
        else if(size == 0)
        {
            break;
        }
        pt += size;
        last -= size;
    }
    return count - last;
}

int TcpSocket::writen(const char *buf, int count)
{
    int last = count;   // 剩余的字节数
    int size = 0;       // 每次写入的字节数
    const char* pt = buf;
    while(last > 0)
    {
        if((size = send(m_socket, pt, last, 0)) < 0)
        {
            perror("send");
            return -1;
        }
        else if(size == 0)
        {
            continue;
        }
        pt += size;
        last -= size;
    }
    return count - last;
}
