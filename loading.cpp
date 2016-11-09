#include "loading.h"
#include "ui_loading.h"
#include <QMessageBox>
#include "mysql.h"
#include <QDesktopWidget>
#include <QPainter>
#include "struct.h"
#include <QPixmap>

CLOADING::CLOADING(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CLOADING)
{
    ui->setupUi(this);
    setWindowTitle("登陆系统");
    this->setWindowIcon(QIcon(":/pic/OfficeParks.png"));
    setWindowFlags(Qt::Widget );    //设置窗口模型
    setWindowModality(Qt::ApplicationModal);
    ui->QlinePass->setEchoMode(QLineEdit::Password);//设置输入密码的显示模式
    ui->QLineUser->setPlaceholderText("用户名");
    ui->QlinePass->setPlaceholderText("密码");
    ui->QlineCity->setPlaceholderText("归属地");
    ui->QLineUser->setFocus();
    ui->QpbtnLoad->setShortcut( QKeySequence::InsertParagraphSeparator );
    m_nFlag = 0;
    ui->QlinePass->setMaxLength(6); //设置密码长度
    ui->QlineCity->setMaxLength(10);

    QDir dir;
    strPath = dir.currentPath() + "/Login.ini";
    _Interface();
    _ReadString();
    connect(this,SIGNAL(SigQuit()), qApp, SLOT(quit()));
}

/*********从文本文件中读取登录数据***********/
void CLOADING::_ReadString()
{
    //“记住密码”复选框
    int nIsCheckPassWord = ::GetPrivateProfileIntA("Flag","idex",0,strPath.toAscii().data());
    if(nIsCheckPassWord == 2)
    {
        ui->QchkPassWord->setCheckState(Qt::Checked);
        char buf1[20]={0};

        ::GetPrivateProfileStringA("Flag","User","00",buf1,20,strPath.toAscii().data());
        ui->QLineUser->setText(buf1);
        memset(buf1,0,20);

        ::GetPrivateProfileStringA("Flag","Pass","00",buf1,20,strPath.toAscii().data());
        _Decode(buf1);  //密码解密
        ui->QlinePass->setText(buf1);
        memset(buf1,0,20);

        ::GetPrivateProfileStringA("Flag","City","00",buf1,20,strPath.toAscii().data());
        ui->QlineCity->setText(buf1);
        memset(buf1,0,20);
    }

    //“自动登录”复选框
    int nIsCheckAutoLogin = ::GetPrivateProfileIntA("Flag","Login",0,strPath.toAscii().data());
    if(nIsCheckAutoLogin == 2)
    {
        ui->QchkAutoLogin->setCheckState(Qt::Checked);
        Timer1 = new QTimer();  //定时器
        connect(Timer1,SIGNAL(timeout()), this, SLOT(Login()));
        Timer1->start(3000);//3秒之后重新启动定时器
    }
}

//界面处理
void CLOADING::_Interface()
{
    QBitmap bmp(this->size());
    bmp.fill();
    QPainter p(&bmp);

    //如果在是否是真的，设置给定的渲染提示；否则清除渲染提示
    //setRenderHint ( RenderHint hint, bool on = true )
    //QPainter::Antialiasing ==> 消除混叠现象，消除走样，图形保真
    p.setRenderHint(QPainter::Antialiasing);
    int arcR = 10;
    QRect rect = this->rect();  //得到一个矩形
    QPainterPath path;
    //逆时针
    path.moveTo(arcR, 0);//移动到指定坐标点，并创建一个新的子路径，同时关闭当前路径
    path.arcTo(0, 0, arcR * 2, arcR * 2, 90.0f, 90.0f); //将当前矩形的角转变为圆弧
    arcR = 6 ;
    path.lineTo(0, rect.height()-arcR);//画一条从当前坐标到指点坐标的线
    path.arcTo(0, rect.height()-arcR*2, arcR * 2, arcR * 2, 180.0f, 90.0f);


    path.lineTo(rect.width()-arcR, rect.height());
    path.arcTo(rect.width()-arcR*2, rect.height()-arcR*2, arcR * 2, arcR * 2, 270.0f, 90.0f);
    arcR = 10 ;
    path.lineTo(rect.width(), arcR);
    path.arcTo(rect.width() - arcR * 2, 0, arcR * 2, arcR * 2, 0.0f, 90.0f);
    path.lineTo(arcR, 0);

    p.drawPath(path);
    p.fillPath(path, QBrush(Qt::red)); //arm和windows平台没有这行代码将显示一个透明的空空的框
    setMask(bmp);
}

void CLOADING::paintEvent(QPaintEvent *event)
{
   QPainter painter(this);
}

void CLOADING::mousePressEvent(QMouseEvent *e)
{
    pos = e->pos();
}

void CLOADING::mouseMoveEvent(QMouseEvent *e)
{
    move((e->globalPos().x()-pos.x()-10),(e->globalPos().y()-pos.y()-30));
}

CLOADING::~CLOADING()
{
    delete ui;
}

//刷新界面——下拉框
//void CLOADING::_Update()
//{
//    CMYSQL MySql;
//    ui->QLineUser->clear();
//    T.clear();
//    if(MySql._GetAllUser(&T))
//    {
//        for(int i = 0;i < T.size();i++)
//        {
////            ui->QLineUser->addItem(T.at(i));
//        }
//    }
//    ui->QlinePass->setFocus();
//}

/*登陆：m_nFlag :为0时代表的是第一次登陆,不为0时代表的是注销后登陆*/
void CLOADING::Login()
{
    if(Timer1->isActive())
        Timer1->stop();
    SUser sUser;    //用户结构体
    sUser.strPass = ui->QlinePass->text();
    sUser.strUser = ui->QLineUser->text();
    sUser.strCity = ui->QlineCity->text();
    sUser.nGrade = 0;   //权限，类型：unsigned short
    CMYSQL MySql;
    if(MySql._GetUser(sUser))   //连接数据库查询是否存在此用户
    {
        if(sUser.nGrade)    //是否有权限
        {
            if(sUser.strCityNum != NULL)
            {
                MySql._InsertSystem(1,sUser.strUser,sUser.strName); //添加系统日志
                if(m_nFlag == 0)    //0代表的是第一次登陆,非0代表的是注销后登陆
                {
                    COfficePark = new COFFICEPARK(sUser);
                    this->setAttribute(Qt::WA_DeleteOnClose);
                    COfficePark->move((QApplication::desktop()->width() - COfficePark->width())/2, (QApplication::desktop()->height() - COfficePark->height())/2);

                    connect(COfficePark,SIGNAL(SigLoading()), this, SLOT(SlotLoading()));
                    connect(this,SIGNAL(SigUser(QString)),COfficePark,SLOT(SlotSeleteUser(QString)));

                    this->hide();   //隐藏登录界面，不关闭
                    COfficePark->show();//显示主界面
                    m_nFlag = 1;    //0代表的是第一次登陆,非0代表的是注销后登陆
                }
                else    //注销之后登录
                {
                    COfficePark->_UpdateLoading(sUser);
                    this->hide();
                    COfficePark->show();
                    m_nFlag = 1;
                }
                emit SigUser(ui->QLineUser->text());    //执行到这里会自动触发SlotLoading()槽函数
            }
            else  //没写归属地
            {
                QMessageBox::warning(NULL,tr("提示"),tr("请输入正确的归属地"));
                ui->QlineCity->selectAll();
                ui->QlineCity->setFocus();
            }
        }
        else  //没有权限
        {
            QMessageBox::warning(NULL,tr("提示"),tr("账号或密码错误"));
            ui->QLineUser->selectAll();
            ui->QLineUser->setFocus();
        }
    }
    else  //用户不存在
    {
        QMessageBox::warning(NULL,tr("提示"),tr("请检查数据库连接"));
    }
}

void CLOADING::_Encode(char *buf)
{
    for(int i = 0;i < strlen(buf);i++)
    {
        int t = buf[i];
        buf[i] = t + 5;
    }
}

void CLOADING::_Decode(char *buf)
{
    for(int i = 0;i < strlen(buf);i++)
    {
        int t = buf[i];
        buf[i] = t - 5;
    }
}

/*********登录数据存入文本文件***********/
void CLOADING::_WriteString()
{
    if(ui->QchkPassWord->checkState() == Qt::Checked)
    {
        QString s2 = QString(ui->QlinePass->text());
        char buf[10] = {0};
        memcpy(buf,s2.toUtf8().data(),s2.toUtf8().size());
        _Encode(buf);//加密
        ::WritePrivateProfileStringA("Flag", "Pass", buf, strPath.toAscii().data());

        QString s3 = QString(ui->QLineUser->text());
        QByteArray byte3 = s3.toUtf8();
        ::WritePrivateProfileStringA("Flag", "User", byte3.data(), strPath.toAscii().data());

        QString s4 = QString(ui->QlineCity->text());
        QByteArray byte4 = s4.toUtf8();
        ::WritePrivateProfileStringA("Flag", "City", byte4.data(), strPath.toAscii().data());
    }
    else if(ui->QchkPassWord->checkState() == Qt::Unchecked)
    {
        QString s2 = QString("");
        QByteArray byte2 = s2.toUtf8();
        ::WritePrivateProfileStringA("Flag", "Pass", byte2.data(), strPath.toAscii().data());

        QString s3 = QString("");
        QByteArray byte3 = s3.toUtf8();
        ::WritePrivateProfileStringA("Flag", "User", byte3.data(), strPath.toAscii().data());

        QString s4 = QString("");
        QByteArray byte4 = s4.toUtf8();
        ::WritePrivateProfileStringA("Flag", "City", byte4.data(), strPath.toAscii().data());
    }
}

/*****信号函数：“登录”按钮*****/
void CLOADING::on_QpbtnLoad_clicked()
{
   Login();
   _WriteString();
}

//系统注销，槽函数响应
void CLOADING::SlotLoading( )
{
    int nIsCheckPassWord = ::GetPrivateProfileIntA("Flag","idex",0,strPath.toAscii().data());
    if(nIsCheckPassWord != 2)   //没有记住密码
    {
        ui->QlinePass->clear(); //清空
        ui->QlineCity->clear();
    }

    int nIsCheckAutoLogin = ::GetPrivateProfileIntA("Flag","Login",0,strPath.toAscii().data());
    if(nIsCheckAutoLogin == 2)
    {
        Timer1->start(3000);    //选择自动登录时，在注销3秒之后再次自动登录
    }
    this->show();   //重新显示登录窗口
    COfficePark->close();   //关闭当前应用程序界面
}

/*****信号函数：退出*****/
void CLOADING::on_QpbtnRevert_clicked()
{
    close();
}

/*****信号函数：记住密码复选框*****/
void CLOADING::on_QchkPassWord_clicked()
{
    //选中index为2，不选中为0
    QString s1 = QString::number(ui->QchkPassWord->checkState());
    QByteArray byte1 = s1.toUtf8();
    ::WritePrivateProfileStringA("Flag", "idex", byte1.data(), strPath.toAscii().data());
}

/*****信号函数：自动登录复选框*****/
void CLOADING::on_QchkAutoLogin_clicked()
{
    //选中login为2，不选中为0
    QString s1 = QString::number(ui->QchkAutoLogin->checkState());
    QByteArray byte1 = s1.toUtf8();
    ::WritePrivateProfileStringA("Flag", "Login", byte1.data(), strPath.toAscii().data());

    if(ui->QchkAutoLogin->isChecked())
    {
        ui->QchkPassWord->setCheckState(Qt::Checked);   //当选中自动登录时，记住密码也自动被勾选
        on_QchkPassWord_clicked();
    }
    else
    {
        Timer1->stop();
    }
}
