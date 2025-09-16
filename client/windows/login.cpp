#include "login.h"
#include "communication.h"
#include "datamanager.h"
#include "ui_login.h"
#include "gamemode.h"
#include <QCryptographicHash>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QThreadPool>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QToolButton>
#include <QStyle>
#include <QAction>

Login::Login(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Login)
{
    ui->setupUi(this);
    setFixedSize(480, 350);
    // ui->stackedWidget->setFixedSize(460, 360);

    ui->stackedWidget->setCurrentIndex(0);

    connect(ui->homeBtn, &QPushButton::clicked, this, [=](){
        ui->stackedWidget->setCurrentIndex(0);
    });
    connect(ui->regUser, &QPushButton::clicked, this, [=](){
        ui->stackedWidget->setCurrentIndex(1);
    });
    connect(ui->netSetBtn, &QPushButton::clicked, this, [=](){
        ui->stackedWidget->setCurrentIndex(2);
    });




    // 数据校验
    QRegularExpression expreg("^[a-zA-Z0-9_]{3,16}$");
    QRegularExpressionValidator *val1 = new QRegularExpressionValidator(expreg, this);
    ui->userName->setValidator(val1);
    ui->regUserName->setValidator(val1);

    /*
     * 密码:
     *  1. 长度为4到12个字符
     *  2. 包含至少一个大写字母、小写字母、数字和特殊字符
    */
    expreg.setPattern("^(?=.*[A-Z])(?=.*[a-z])(?=.*\\d)(?=.*[@$!%*?&])[A-Za-z\\d@$!%*?&]{4,20}$");
    QRegularExpressionValidator *val2 = new QRegularExpressionValidator(expreg, this);
    ui->password->setValidator(val2);
    ui->regPassword->setValidator(val2);

    // 手机号校验
    expreg.setPattern("^1[3456789]\\d{9}$");
    QRegularExpressionValidator *val3 = new QRegularExpressionValidator(expreg, this);
    ui->phone->setValidator(val3);

    // IP校验
    expreg.setPattern("^((\\d|[1-9]\\d|1\\d{2}|2[0-4]\\d|25[0-5])\\.){3}(\\d|[1-9]\\d|1\\d{2}|2[0-4]\\d|25[0-5])$");
    QRegularExpressionValidator *val4 = new QRegularExpressionValidator(expreg, this);
    ui->ipAddr->setValidator(val4);

    // 端口校验
    expreg.setPattern("^(0|([1-9]\\d{0,3})|([1-5]\\d{4})|(6[0-4]\\d{3})|(65[0-4]\\d{2})|(655[0-2]\\d)|(6553[0-5]))$");
    QRegularExpressionValidator *val5 = new QRegularExpressionValidator(expreg, this);
    ui->port->setValidator(val5);

    // 用户名提示
    ui->userName->setToolTip(
        "📝 用户名规则：\n"
        "• 允许：字母/数字/下划线\n"
        "• 长度：3~16字符\n"
        "• 示例：John_123"
        );

    ui->regUserName->setToolTip(ui->userName->toolTip());

    // 密码提示（带校验清单和示例）
    ui->password->setToolTip("🔒 密码规则：\n"
                             "• 长度：4 ~ 20 个字符\n"
                             "• 必须包含：\n"
                             "  ✅ 1 个大写字母\n"
                             "  ✅ 1 个小写字母\n"
                             "  ✅ 1 个数字\n"
                             "  ✅ 1 个特殊符号 (@$!%*?&)\n"
                             "• 示例：Pass@123");
    ui->regPassword->setToolTip(ui->password->toolTip());

    // 手机号提示（简洁+示例）
    ui->phone->setToolTip("📱 手机号规则：\n"
                          "• 11 位数字\n"
                          "• 开头：1（3/4/5/6/7/8/9）\n"
                          "• 示例：13812345678");

    // IP地址提示（技术风格）
    ui->ipAddr->setToolTip("🌐 IP地址规则：\n"
                           "• 格式：XXX.XXX.XXX.XXX\n"
                           "• 每段范围：0 ~ 255\n"
                           "• 示例：192.168.1.1");

    // 端口提示（带常用端口说明）
    ui->port->setToolTip("🔌 端口号规则：\n"
                         "• 范围：0 ~ 65535\n");

    // 处理按钮点击事件
    connect(ui->loginBtn, &QPushButton::clicked, this, &Login::onLogin);
    connect(ui->registerBtn, &QPushButton::clicked, this, &Login::onRegister);
    connect(ui->netOkBtn, &QPushButton::clicked, this, &Login::onNetOK);

    // 设置线程池最大的线程数量
    QThreadPool::globalInstance()->setMaxThreadCount(8);


    loadUserInfo();
    setPasswdMode(ui->password);
    setPasswdMode(ui->regPassword);
}

bool Login::verifyData(QLineEdit* edit)
{
    if(edit->hasAcceptableInput() == false)
    {
        edit->setStyleSheet("border: 2px solid red;");
        return false;
    }
    else
    {
        edit->setStyleSheet("none");
    }
    return true;
}

void Login::startConnect(Message *msg)
{
    if(!m_isConnected)
    {
        Communication *task = new Communication(*msg);
        connect(task, &Communication::connectFailed, this, [=](){
            QMessageBox::critical(this, "连接服务器", "连接服务器失败");
            m_isConnected = false;
        });
        connect(task, &Communication::loginOk, this, [=](){
            m_isConnected = true;
            // 将用户名保存到单例对象
            DataManager::getInstance()->setUserName(ui->userName->text().toUtf8());
            // 保存用户名和密码
            saveUserInfo();
            // 显示游戏模式窗口-> 单机版, 网络版
            GameMode* mode = new GameMode;
            mode->show();
            accept();
        });
        connect(task, &Communication::registerOk, this, [=](){
            m_isConnected = true;
            ui->stackedWidget->setCurrentIndex(0);
        });
        connect(task, &Communication::loginFailed, this, [=](QByteArray msg){
            m_isConnected = false;
            QMessageBox::critical(this, "登录", msg);
        });
        connect(task, &Communication::registerFailed, this, [=](QByteArray msg){
            m_isConnected = false;
            QMessageBox::critical(this, "注册", msg);
        });
        connect(task, &Communication::aesFailed, this, [=](QByteArray msg){
            m_isConnected = false;
            QMessageBox::critical(this, "ERROR", msg);
        });
        QThreadPool::globalInstance()->start(task);
        DataManager::getInstance()->setCommunication(task);
    }
    else
    {
        // 和服务器进行通信
        DataManager::getInstance()->getCommunication()->sendMessage(msg);
    }
}

void Login::onLogin()
{
    bool flag1 = verifyData(ui->userName);
    bool flag2 = verifyData(ui->password);
    if(flag1 && flag2)
    {
        Message msg;
        msg.userName = ui->userName->text().toUtf8();
        msg.reqcode = RequestCode::UserLogin;
        QByteArray passwd = ui->password->text().toUtf8();
        passwd = QCryptographicHash::hash(passwd, QCryptographicHash::Sha224).toHex();
        msg.data1 = passwd;
        // 连接服务器
        startConnect(&msg);
    }
}

void Login::onRegister()
{
    bool flag1 = verifyData(ui->regUserName);
    bool flag2 = verifyData(ui->regPassword);
    bool flag3 = verifyData(ui->phone);
    if(flag1 && flag2 && flag3)
    {
        Message msg;
        msg.userName = ui->regUserName->text().toUtf8();
        msg.reqcode = RequestCode::Register;
        QByteArray passwd = ui->regPassword->text().toUtf8();
        passwd = QCryptographicHash::hash(passwd, QCryptographicHash::Sha224).toHex();
        msg.data1 = passwd;
        msg.data2 = ui->phone->text().toUtf8();
        // 连接服务器
        startConnect(&msg);
    }
}

void Login::onNetOK()
{
    bool flag1 = verifyData(ui->ipAddr);
    bool flag2 = verifyData(ui->port);
    if(flag1 && flag2)
    {
        DataManager* instance = DataManager::getInstance();
        instance->setIP(ui->ipAddr->text().toUtf8());
        instance->setPort(ui->port->text().toUtf8());
        QMessageBox::information(this, "设置网络", "设置网络成功");
    }
}

void Login::saveUserInfo()
{
    if(ui->savePwd->isChecked())
    {
        QJsonObject obj;
        obj.insert("user", ui->userName->text());
        obj.insert("passwd", ui->password->text());
        QJsonDocument doc(obj);
        QByteArray json = doc.toJson();
        // aes 加密
        AesCrypto aes(AesCrypto::AES_CBC_128, KEY.left(16));
        json = aes.encrypt(json);
        // 写文件
        QFile file("passwd.bin");
        file.open(QFile::WriteOnly);
        file.write(json);
        file.close();
    }
    else
    {
        QFile file("passwd.bin");
        file.remove();
    }
}

void Login::loadUserInfo()
{
    QFile file("passwd.bin");
    bool flag  = file.open(QFile::ReadOnly);
    if(flag)
    {
        ui->savePwd->setChecked(true);
        QByteArray all = file.readAll();
        AesCrypto aes(AesCrypto::AES_CBC_128, KEY.left(16));
        all = aes.decrypt(all);
        QJsonDocument doc = QJsonDocument::fromJson(all);
        QJsonObject obj = doc.object();
        QString name = obj.value("user").toString();
        QString passwd = obj.value("passwd").toString();
        ui->userName->setText(name);
        ui->password->setText(passwd);
    }
    else
    {
        ui->savePwd->setChecked(false);
    }
}

void Login::setPasswdMode(QLineEdit *edit)
{
    edit->setEchoMode(QLineEdit::Password);
    edit->setSizePolicy(
        QSizePolicy::Preferred, // 水平策略
        QSizePolicy::Fixed      // 垂直固定
        );

    // 尝试使用系统主题图标，回退到标准图标
    QIcon visibleIcon = QIcon::fromTheme("view-visible",
                                         style()->standardIcon(QStyle::SP_TitleBarNormalButton));
    QIcon hiddenIcon = QIcon::fromTheme("view-hidden",
                                        style()->standardIcon(QStyle::SP_TitleBarShadeButton));

    QAction *toggleAction = edit->addAction(
        hiddenIcon,
        QLineEdit::TrailingPosition
        );
    toggleAction->setCheckable(true);
    toggleAction->setToolTip("显示密码");

    connect(toggleAction, &QAction::toggled, this, [=](bool checked) {
        edit->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::Password);
        toggleAction->setIcon(checked ? visibleIcon : hiddenIcon);
        // toggleAction->setToolTip(checked ? "隐藏密码" : "显示密码");
    });
}

Login::~Login()
{
    delete ui;
}
