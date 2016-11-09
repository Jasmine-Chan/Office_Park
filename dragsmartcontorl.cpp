#include "dragsmartcontorl.h"
#include "ui_dragsmartcontorl.h"

dragsmartContorl::dragsmartContorl(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dragsmartContorl)
{
    ui->setupUi(this);

    IconicoSend = new dragwindows;//控件区
    Iconicorecv = new dragwindows;//画图区
    InputCode   = new QWidget;

    Hlayout = new QHBoxLayout;
    Hlayout->addWidget(IconicoSend);
    Hlayout->addWidget(Iconicorecv);
    Hlayout->addWidget(InputCode);
    Hlayout->setMargin(0);
    this->setLayout(Hlayout);
    this->setMaximumSize(900, 600); //固定大小
    this->setMinimumSize(900, 600);

    button = new QPushButton(Iconicorecv);  //测试控件
    button->setText(tr("发送"));
    button->setMaximumSize(120,36);

    LineType = new QLineEdit();
    LineRoomCode = new QLineEdit();
    LineDeviceCode = new QLineEdit();
    controlType = new QLineEdit();
    controlCode = new QLineEdit();
    Vlayout = new QVBoxLayout();

    LineType->setText(tr("输入设备类型"));
    LineType->setToolTip(tr("输入设备类型"));

    LineRoomCode->setText(tr("输入房间编号"));
    LineRoomCode->setToolTip(tr("输入房间编号"));

    LineDeviceCode->setText(tr("输入房内设备编号"));
    LineDeviceCode->setToolTip(tr("输入房内设备编号"));

    controlType->setText(tr("输入控制类型"));
    controlType->setToolTip(tr("输入控制类型"));

    controlCode->setText(tr("输入控制数据"));
    controlCode->setToolTip(tr("输入控制数据"));

    LineType->setMaximumSize(120,18);
    LineRoomCode->setMaximumSize(120,18);
    LineDeviceCode->setMaximumSize(120,18);
    controlType->setMaximumSize(120,18);
    controlCode->setMaximumSize(120,18);

    Vlayout->addWidget(LineType);
    Vlayout->addWidget(LineRoomCode);
    Vlayout->addWidget(LineDeviceCode);
    Vlayout->addWidget(controlType);
    Vlayout->addWidget(controlCode);
    Vlayout->addWidget(button);
    InputCode->setLayout(Vlayout);

    QDir dir;
    strPath = dir.currentPath() + "/SmartHome.ini";

    _Init();    //调用初始化函数

    connect(IconicoSend,SIGNAL(SigLocation(QString)),Iconicorecv,SLOT(SlotLocation(QString)));
    connect(button, SIGNAL(clicked()), this, SLOT(button_clicked()));   //测试函数
}

dragsmartContorl::~dragsmartContorl()
{
    delete ui;
}

void dragsmartContorl::_Init()
{
    _InitDray(); //画图功能界面初始化
    Iconicorecv->_Init(true);//初始化“画图区”
    IconicoSend->_Init(false);//初始化“控件区”
}

void dragsmartContorl::_InitDray()  //初始化控件区
{
    int temp = 0;   //记录Y轴坐标

    //为了在删除奇数编号的控件的时候，不出现布局异常，将ini文件中的控件数据按编号显示
    for(int i = 1; i <= 10; i++)
    {
        if(i%2 == 0)    //右边
        {
            char buf[80] = {0};
            temp = 63 + 23*((i/2)-1) + 64*((i/2)-1);
            QString tempStr;
            ::GetPrivateProfileStringA("Path", QString::number(i).toAscii().data(), "00", buf, 80, strPath.toAscii().data());
            tempStr = buf;
            if(tempStr != "00")
            {
                qDebug()<<"右tempStr:"<<tempStr<<i;
                QPoint Point1(24*2+64,temp);//40+23
                QLabel *boatIcon1 = new QLabel(IconicoSend);
                boatIcon1->setPixmap(QPixmap(tempStr));
                boatIcon1->move(Point1);
                boatIcon1->show();
                boatIcon1->setAttribute(Qt::WA_DeleteOnClose);
                boatIcon1->setScaledContents(true);
                boatIcon1->resize(QSize(64, 64));
                memset(buf, 0, 80);
            }
        }
        else    //左边
        {
            char buf[80] = {0};
            QString tempStr;
            ::GetPrivateProfileStringA("Path", QString::number(i).toAscii().data(), "00", buf, 80, strPath.toAscii().data());
            tempStr = buf;
            if(tempStr != "00")
            {
                qDebug()<<"左tempStr:"<<tempStr<<i;
                temp = 63 + (64 + 23)*((i + 1)/2 - 1);
                QPoint Point2(24,temp);
                QLabel *boatIcon2 = new QLabel(IconicoSend);
                boatIcon2->setPixmap(QPixmap(tempStr));
                boatIcon2->move(Point2);
                boatIcon2->show();
                boatIcon2->setAttribute(Qt::WA_DeleteOnClose);
                boatIcon2->setScaledContents(true);
                boatIcon2->resize(QSize(64, 64));
                memset(buf, 0, 80);
            }
        }
    }

    IconicoSend->setStyleSheet(" QTableWidget{border:1px solid rgb(105,150,177);}");
    IconicoSend->setMaximumSize(200,500);//设置固定大小
    IconicoSend->setMinimumSize(200,500);
    Iconicorecv->setMinimumSize(500,500);
    Iconicorecv->setMaximumSize(500,500);
    QLabel *Addr_widget = new QLabel(IconicoSend);//设置控件区标题
    Addr_widget->setAlignment(Qt::AlignCenter);
    Addr_widget->setText("控件区");
    Addr_widget->setGeometry(0,0,200,40);
    Addr_widget->setStyleSheet("background-color:rgb(43,68,87);font-size:14pt;color:white");
    Addr_widget->show();

    QLabel *Addr_map = new QLabel(Iconicorecv);//设置画图区标题
    Addr_map->setAlignment(Qt::AlignCenter);
    Addr_map->setText("画图区");

    Addr_map->setGeometry(0,0,500,40);
    Addr_map->setStyleSheet("background-color:rgb(43,68,87);font-size:14pt;color:white");
    Addr_map->show();
}

/******测试用槽函数*******/
void dragsmartContorl::button_clicked()
{
    QString DeviceType = LineType->text();
    QString RoomCode = LineRoomCode->text();
    QString DeviceCode = LineDeviceCode->text();
    QString strType    = controlType->text();
    QString strCode    = controlCode->text();
    emit sendMessageToUdp(DeviceType, RoomCode, DeviceCode, strType, strCode);
}
