#include "login.h"
#include "cards.h"
#include <QApplication>
#include <QFile>
#include <QResource>
#include <QDebug>

int main(int argc, char *argv[])
{
    // 提供高分辨率适配 - Qt5需要手动设置, 在Qt6中这个属性默认已经生效无效自己设置
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);
    qRegisterMetaType<Cards>("Cards&");
    qRegisterMetaType<Cards>("Cards");
    //QResource::registerResource("./resource.rcc");
    // 加载资源文件 - 文件必须是utf8编码
    QFile file(":/conf/style.qss");
    file.open(QFile::ReadOnly);
    QByteArray all = file.readAll();
    a.setStyleSheet(all);
    file.close();

    Login w;
    int ret = w.exec();
    if(ret == QDialog::Accepted)
    {
        return a.exec();
    }
    return 0;
}
