#include "officeparks.h"
#include "ui_officeparks.h"
#include <QDebug>
COFFICEPARKS::COFFICEPARKS(SUser sUser,QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::COFFICEPARKS)
{
    ui->setupUi(this);
    m_sUser = sUser;
    CUdpServer = new CUDPSERVER("m_sUser.strCityNum");
    CUdpServer->start();
    _InitTray();
    _InitDevice();
    _InitAlarm();
    _InitDeviceRecord();
    _InitSystem();
    _InitAlarmRecord();

    MinimizeAction = new QAction(tr("最小化"),this);
    MaximizeAction = new QAction(tr("最大化"),this);
    RestoreAction = new QAction(tr("还原"),this);
    TrayQuitAction = new QAction(tr("退出"),this);
    TrayIconMenu = new QMenu(QApplication::desktop());
    TrayIconMenu->addAction(MinimizeAction);
    TrayIconMenu->addAction(MaximizeAction);
    TrayIconMenu->addAction(RestoreAction);
    TrayIconMenu->addSeparator();
    TrayIconMenu->addAction(TrayQuitAction);
    TrayIcon->setContextMenu(TrayIconMenu);


    CDelete = new CDELETE();
    CFind = new CFIND();
    CSmartControl = new CSMARTCONTROL();
    Personnel=new CPERSONNEL;//创建新窗口对象
    layout=new QVBoxLayout;//新窗口画图到控件上
    layout->addWidget(Personnel);
    ui->QstackePersonnel->setLayout(layout);
    connect(Personnel,SIGNAL(SigPersonnel_Staff_Leave(int)),this,SLOT(SlotPersonnel_Staff_Leave(int)));

    CSystem = new CSYSTEM;//创建新窗口对象
    layout = new QVBoxLayout;//新窗口画图到控件上
    layout->addWidget(CSystem);
    ui->QstackeSystem->setLayout(layout);

    Attendance=new CATTENDANCE;//创建新窗口对象
    layout=new QVBoxLayout;//新窗口画图到控件上
    layout->addWidget(Attendance);
    ui->QstackeAttendance->setLayout(layout);

    _UpdateDevice(CUdpServer->m_DeviceItem);

    SystemRecordMenu = new QMenu(ui->QSystem);
    AlarmRecordMenu = new QMenu(ui->QAlarmRecord);
    DeviceRecordMenu = new QMenu(ui->QDeviceRecord);
    FindRecordAction = new QAction(tr("查找"),this);
    DeleteRecordAction = new QAction(tr("删除"),this);
    connect(FindRecordAction, SIGNAL(triggered()), this, SLOT(SlotFindRecord()));
    connect(DeleteRecordAction, SIGNAL(triggered()), this, SLOT(SlotDeleteRecord()));
    ui->QSystem->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->QAlarmRecord->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->QDeviceRecord->setContextMenuPolicy(Qt::CustomContextMenu);
    DeviceMenu = new QMenu(ui->QDevice);

    DeleteAction = new QAction(tr("删除"),this);
    ResetAction = new QAction(tr("重启"),this);
    PingAction = new QAction(tr("网络测试"),this);
    InitPassAction = new QAction(tr("初始化密码"),this);
    SmartControlAction = new QAction(tr("智能家居"),this);
    connect(SmartControlAction, SIGNAL(triggered()), this, SLOT(SlotSmartControl()));
    connect(ResetAction, SIGNAL(triggered()), this, SLOT(SlotReset()));
    connect(DeleteAction, SIGNAL(triggered()), this, SLOT(SlotDelete()));
    connect(PingAction, SIGNAL(triggered()), this, SLOT(SlotPing()));
    connect(InitPassAction, SIGNAL(triggered()), this, SLOT(SlotInitPass()));

    RegisterAction = new QAction(tr("注册模式"),this);
    connect(RegisterAction, SIGNAL(triggered()), this, SLOT(SlotRegister()));
    ui->QDevice->setContextMenuPolicy(Qt::CustomContextMenu);

    AlarmMenu = new QMenu(ui->QAlarm);
    DealAction = new QAction(tr("处理"),this);
    connect(DealAction, SIGNAL(triggered()), this, SLOT(SlotDeal()));
    ui->QAlarm->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(CUdpServer, SIGNAL(SigRecvLogOut()), this, SLOT(SlotRecvLogOut()));
    connect(CUdpServer, SIGNAL(SigInitDevice()), this, SLOT(SlotInitDevice()));
    connect(CUdpServer, SIGNAL(SigUpdateDevice(QVariant)), this, SLOT(SlotUpdateDevice(QVariant)));
    connect(CUdpServer, SIGNAL(SigOnlineDevice(int,int)), this, SLOT(SlotOnlineDevice(int,int)));
    connect(CUdpServer, SIGNAL(SigInsertAlarm()), this, SLOT(SlotInsertAlarm()));
    connect(CSmartControl,SIGNAL(SigSendLamp(char*,QString)),this,SLOT(SlotSendLamp(char*,QString)));
    connect(CUdpServer, SIGNAL(SigUdpOpenFail()), this, SLOT(SlotUdpOpenFail()));
    connect( this, SIGNAL(Sigquit()), qApp, SLOT(quit()) );
}

void COFFICEPARKS::closeEvent( QCloseEvent * event )
{
//    if(this->isVisible())
//    {
//    hide();
//    event->ignore();
//    emit Sigquit();
//    }else
//        event->accept();
    emit Sigquit();

}

void COFFICEPARKS::SlotFindRecord()
{
    CFind->_Update(m_nRecord);
    CFind->show();
}

void COFFICEPARKS::SlotDeleteRecord()
{
    CDelete->_Update(m_nRecord);
    CDelete->show();
}

//响应UDP端口打开失败
void COFFICEPARKS::SlotUdpOpenFail()
{
//    m_nUdp = 1;
    QMessageBox::warning(NULL,tr("提示"),tr("udp端口打开失败，请检查后重启"));
    CUdpServer->m_Mysql._InsertSystem(3,m_sUser.strUser,m_sUser.strName);
    emit Sigquit();
}
//通过报警链表更新报警列表
void COFFICEPARKS::_UpdateAlarm(NodeAlarm *ItemAlarm)
{
    int ti = 0;
    ui->QAlarm->setRowCount(ti);
    while(ItemAlarm->next != NULL)
    {
        ui->QAlarm->setRowCount(ti + 1);
        ItemAlarm->data.nId = ti;

        QTableWidgetItem *tabledmItem0 = new QTableWidgetItem(QString::number(ti+1));
        tabledmItem0->setTextAlignment(Qt::AlignCenter);
        ui->QAlarm->setItem(ti, 0, tabledmItem0);

        QTableWidgetItem *tabledmItem1 = new QTableWidgetItem(QString(ItemAlarm->data.gcIpAddr));
        tabledmItem1->setTextAlignment(Qt::AlignCenter);
        ui->QAlarm->setItem(ti, 1, tabledmItem1);

        QTableWidgetItem *tabledmItem2 = new QTableWidgetItem(QString(ItemAlarm->data.gcAddrExplain));
        tabledmItem2->setTextAlignment(Qt::AlignCenter);
        ui->QAlarm->setItem(ti, 2, tabledmItem2);

                QString str2 = "防区" + QString::number(ItemAlarm->data.nFenceId);
        QTableWidgetItem *tabledmItem3 = new QTableWidgetItem(str2);
        tabledmItem3->setTextAlignment(Qt::AlignCenter);
        ui->QAlarm->setItem(ti, 3, tabledmItem3);

        QString str3;
        switch(ItemAlarm->data.nAlarmType)
        {
        str3.clear();
        case 0:
            str3 = "特殊报警";
            break;
        case 2:
            str3 = "红外";
            break;
        case 3:
            str3 = "门铃";
            break;
        case 4:
            str3 = "烟感";
            break;
        case 6:
            str3 = "门磁1";
            break;
        case 7:
            str3 = "遥控SOS";
            break;
        case 8:
            str3 = "门磁2";
            break;
        case 10:
            str3 = "水浸";
            break;
        case 12:
            str3 = "煤气";
            break;
        case 13:
            str3 = "门磁3";
            break;
        case 14:
            str3 = "胁迫开门";
            break;
        case 15:
            str3 = "设备强拆";
            break;
        case 16:
            str3 = "开门超时";
            break;
        default :
            str3 = "错误";
            break;
        }
        QTableWidgetItem *tabledmItem4 = new QTableWidgetItem(str3);
        tabledmItem4->setTextAlignment(Qt::AlignCenter);
        ui->QAlarm->setItem(ti, 4, tabledmItem4);

        QTableWidgetItem *tabledmItem5 = new QTableWidgetItem(QString(ItemAlarm->data.gcStime));
        tabledmItem5->setTextAlignment(Qt::AlignCenter);
        ui->QAlarm->setItem(ti, 5, tabledmItem5);

        QTableWidgetItem *tabledmItem6 = new QTableWidgetItem(QString(ItemAlarm->data.gcEtime));
        tabledmItem6->setTextAlignment(Qt::AlignCenter);
        ui->QAlarm->setItem(ti, 6, tabledmItem6);

        QTableWidgetItem *tabledmItem7 = new QTableWidgetItem(QString(ItemAlarm->data.gcType));
        tabledmItem7->setTextAlignment(Qt::AlignCenter);
        ui->QAlarm->setItem(ti, 7, tabledmItem7);

        ItemAlarm = ItemAlarm->next;
        ti ++;
    }
}
//更新住户设备列表通过住户链表
void COFFICEPARKS::_UpdateDevice(NodeDevice *ItemDevice)
{
    int ti = 0;
    ui->QDevice->setRowCount(ti);
    while(ItemDevice->next != NULL)
    {
        ui->QDevice->setRowCount(ti + 1);
        ItemDevice->data.nId = ti;

        QTableWidgetItem *tabledmItem0 = new QTableWidgetItem(QString::number(ti+1));
        tabledmItem0->setTextAlignment(Qt::AlignCenter);
        ui->QDevice->setItem(ti, 0, tabledmItem0);
//        qDebug()<<QString(ItemDevice->data.gcAddr);
        QTableWidgetItem *tabledmItem1 = new QTableWidgetItem(QString(ItemDevice->data.gcAddr));
        tabledmItem1->setTextAlignment(Qt::AlignCenter);
        ui->QDevice->setItem(ti, 1, tabledmItem1);

        QTableWidgetItem *tabledmItem2 = new QTableWidgetItem(QString(ItemDevice->data.gcMacAddr));
        tabledmItem2->setTextAlignment(Qt::AlignCenter);
        ui->QDevice->setItem(ti, 2, tabledmItem2);

        QTableWidgetItem *tabledmItem3 = new QTableWidgetItem(QString(ItemDevice->data.gcIpAddr));
        tabledmItem3->setTextAlignment(Qt::AlignCenter);
        ui->QDevice->setItem(ti, 3, tabledmItem3);

        if(ItemDevice->data.nState <= 0)
        {
            QTableWidgetItem *tabledmItem4 = new QTableWidgetItem("未连接");
            tabledmItem4->setTextAlignment(Qt::AlignCenter);
            ui->QDevice->setItem(ti, 4, tabledmItem4);
        }else{
            QTableWidgetItem *tabledmItem4 = new QTableWidgetItem("已连接");
            tabledmItem4->setTextAlignment(Qt::AlignCenter);
            ui->QDevice->setItem(ti, 4, tabledmItem4);
        }

        if(ItemDevice->data.nFenceState == 0)
        {
            QTableWidgetItem *tabledmItem5 = new QTableWidgetItem("撤防");
            tabledmItem5->setTextAlignment(Qt::AlignCenter);
            ui->QDevice->setItem(ti, 5, tabledmItem5);
        }else{
            switch(ItemDevice->data.nFenceState)
            {
            case 1:
            {
                QTableWidgetItem *tabledmItem5 = new QTableWidgetItem("外出布防");
                tabledmItem5->setTextAlignment(Qt::AlignCenter);
                ui->QDevice->setItem(ti, 5, tabledmItem5);
                break;
            }
            case 2:
            {
                QTableWidgetItem *tabledmItem5 = new QTableWidgetItem("在家布防");
                tabledmItem5->setTextAlignment(Qt::AlignCenter);
                ui->QDevice->setItem(ti, 5, tabledmItem5);
                break;
            }
            default :
            {
                QTableWidgetItem *tabledmItem5 = new QTableWidgetItem("未知布防");
                tabledmItem5->setTextAlignment(Qt::AlignCenter);
                ui->QDevice->setItem(ti, 5, tabledmItem5);
                break;
            }
            }
        }

        QTableWidgetItem *tabledmItem6 = new QTableWidgetItem(QString(ItemDevice->data.gcType));
        tabledmItem6->setTextAlignment(Qt::AlignCenter);
        ui->QDevice->setItem(ti, 6, tabledmItem6);

        if(ItemDevice->data.nAlarmState == 0)
        {
            for(int j = 0;j < 7;j++)
            {
                QTableWidgetItem *item = ui->QDevice->item(ti, j);
                item->setBackgroundColor(QColor(0,125,0,0));
            }
        }else{
            for(int j = 0;j < 7;j++)
            {
                QTableWidgetItem *item = ui->QDevice->item(ti, j);
                item->setBackgroundColor(QColor(255,0,0));
            }
        }

        if(ItemDevice->data.nState <= 0)
        {
            for(int j = 0;j < 7;j++)
            {
                QTableWidgetItem *item = ui->QDevice->item(ti, j);
                item->setTextColor(QColor(0,0,0));
            }
        }else{
            for(int j = 0;j < 7;j++)
            {
                QTableWidgetItem *item = ui->QDevice->item(ti, j);
                item->setTextColor(Qt::blue);
            }
        }
        ti++;
        ItemDevice = ItemDevice->next;
    }
//    ui->QlblOnlineTenement->setText(QString::number(CUdpServer->m_nAllTenementOnline));

}
void COFFICEPARKS::_InitDeviceRecord()
{
ui->QDeviceRecord->setColumnCount(5);
ui->QDeviceRecord->setShowGrid(true);//显示表格线
QStringList headers;
headers<<"序号"<<"地址"<<"IP地址"<<"状态"<<"时间";
ui->QDeviceRecord->verticalHeader()->hide();//行头不显示
ui->QDeviceRecord->horizontalHeader()->setClickable(false);//行头不可选
ui->QDeviceRecord->setHorizontalHeaderLabels(headers);
ui->QDeviceRecord->horizontalHeader()->setHighlightSections(false);
ui->QDeviceRecord->resizeColumnsToContents();
ui->QDeviceRecord->setFont(QFont("Helvetica"));
ui->QDeviceRecord->setEditTriggers(QAbstractItemView::NoEditTriggers);//禁止编辑
ui->QDeviceRecord->setSelectionBehavior(QAbstractItemView::SelectRows);  //整行选中的方式：一行
ui->QDeviceRecord->setColumnWidth(0,50);
ui->QDeviceRecord->setColumnWidth(1,200);
ui->QDeviceRecord->setColumnWidth(2,120);
ui->QDeviceRecord->setColumnWidth(3,120);
ui->QDeviceRecord->setColumnWidth(4,130);
//ui->QDeviceRecord->setColumnWidth(5,130);
}

void COFFICEPARKS::_InitAlarmRecord()
{
    ui->QAlarmRecord->setColumnCount(12);
    ui->QAlarmRecord->setShowGrid(true);//显示表格线
    QStringList headers;
    headers<<"序号"<<"IP地址"<<"地址"<<"报警防区"<<"报警类型"<<"报警时间"<<"确认时间"<<"取消时间"<<"是否误报"<<"处理人"<<"详情"<<"设备类型";
    ui->QAlarmRecord->verticalHeader()->hide();//行头不显示
    ui->QAlarmRecord->horizontalHeader()->setClickable(false);//行头不可选
    ui->QAlarmRecord->setHorizontalHeaderLabels(headers);
    ui->QAlarmRecord->horizontalHeader()->setHighlightSections(false);
    ui->QAlarmRecord->resizeColumnsToContents();
    ui->QAlarmRecord->setFont(QFont("Helvetica"));
    ui->QAlarmRecord->setEditTriggers(QAbstractItemView::NoEditTriggers);//禁止编辑
    ui->QAlarmRecord->setSelectionBehavior(QAbstractItemView::SelectRows);  //整行选中的方式：一行
    ui->QAlarmRecord->setColumnWidth(0,50);
    ui->QAlarmRecord->setColumnWidth(1,100);
    ui->QAlarmRecord->setColumnWidth(2,200);
    ui->QAlarmRecord->setColumnWidth(3,80);
    ui->QAlarmRecord->setColumnWidth(4,100);
    ui->QAlarmRecord->setColumnWidth(5,100);
    ui->QAlarmRecord->setColumnWidth(6,80);
    ui->QAlarmRecord->setColumnWidth(7,80);
    ui->QAlarmRecord->setColumnWidth(8,130);
    ui->QAlarmRecord->setColumnWidth(9,130);
    ui->QAlarmRecord->setColumnWidth(10,130);
    ui->QAlarmRecord->setColumnWidth(11,80);
}

void COFFICEPARKS::_InitSystem()
{
    ui->QSystem->setColumnCount(5);
    ui->QSystem->setShowGrid(true);//显示表格线
    QStringList headers;
    headers<<"序号"<<"用户名"<<"姓名"<<"操作类型"<<"时间";
    ui->QSystem->verticalHeader()->hide();//行头不显示
    ui->QSystem->horizontalHeader()->setClickable(false);//行头不可选
    ui->QSystem->setHorizontalHeaderLabels(headers);
    ui->QSystem->horizontalHeader()->setHighlightSections(false);
    ui->QSystem->resizeColumnsToContents();
    ui->QSystem->setFont(QFont("Helvetica"));
    ui->QSystem->setEditTriggers(QAbstractItemView::NoEditTriggers);//禁止编辑
    ui->QSystem->setSelectionBehavior(QAbstractItemView::SelectRows);  //整行选中的方式：一行
    ui->QSystem->setColumnWidth(0,50);
    ui->QSystem->setColumnWidth(1,120);
    ui->QSystem->setColumnWidth(2,120);
    ui->QSystem->setColumnWidth(3,120);
    ui->QSystem->setColumnWidth(4,130);
}
void COFFICEPARKS::_InitDevice()
{
    ui->QDevice->setColumnCount(7);
    ui->QDevice->setShowGrid(true);//显示表格线
    QStringList headers;
    headers<<"序号"<<"地址说明"<<"网卡地址"<<"IP地址"<<"连接状态"<<"布防状态"<<"设备类型";
    ui->QDevice->verticalHeader()->hide();//行头不显示
    ui->QDevice->horizontalHeader()->setClickable(false);//行头不可选
    ui->QDevice->setHorizontalHeaderLabels(headers);
    ui->QDevice->horizontalHeader()->setHighlightSections(false);//表头坍塌
    ui->QDevice->resizeColumnsToContents();
    ui->QDevice->setFont(QFont("Helvetica"));//字体
    ui->QDevice->setEditTriggers(QAbstractItemView::NoEditTriggers);//禁止编辑
    ui->QDevice->setSelectionBehavior(QAbstractItemView::SelectRows);  //整行选中的方式：一行
    ui->QDevice->setColumnWidth(0,40);
    ui->QDevice->setColumnWidth(1,200);
    ui->QDevice->setColumnWidth(2,120);
    ui->QDevice->setColumnWidth(3,100);
    ui->QDevice->setColumnWidth(4,80);
    ui->QDevice->setColumnWidth(5,80);
    ui->QDevice->setColumnWidth(6,80);
}

void COFFICEPARKS::_InitAlarm()
{
    ui->QAlarm->setColumnCount(8);
    ui->QAlarm->setShowGrid(true);//显示表格线
    QStringList headers;
    headers<<"序号"<<"IP地址"<<"地址"<<"报警防区"<<"报警类型"<<"报警时间"<<"取消时间"<<"设备类型";
    ui->QAlarm->verticalHeader()->hide();//行头不显示
    ui->QAlarm->horizontalHeader()->setClickable(false);//行头不可选
    ui->QAlarm->setHorizontalHeaderLabels(headers);
    ui->QAlarm->horizontalHeader()->setHighlightSections(false);//表头坍塌
    ui->QAlarm->resizeColumnsToContents();
    ui->QAlarm->setFont(QFont("Helvetica"));//字体
    ui->QAlarm->setEditTriggers(QAbstractItemView::NoEditTriggers);//禁止编辑
    ui->QAlarm->setSelectionBehavior(QAbstractItemView::SelectRows);  //整行选中的方式：一行
    ui->QAlarm->setColumnWidth(0,40);
    ui->QAlarm->setColumnWidth(1,200);
    ui->QAlarm->setColumnWidth(2,120);
    ui->QAlarm->setColumnWidth(3,100);
    ui->QAlarm->setColumnWidth(4,80);
    ui->QAlarm->setColumnWidth(5,80);
    ui->QAlarm->setColumnWidth(6,80);
    ui->QAlarm->setColumnWidth(7,80);
    ui->QAlarm->setColumnWidth(8,80);
}

COFFICEPARKS::~COFFICEPARKS()
{
    delete ui;
}

//登录数据更新
void COFFICEPARKS::_UpdateDeviceRecord()
{
    QDateTime dateTime=QDateTime::currentDateTime();
    dateTime = dateTime.addDays(-15);
    QString stime = dateTime.toString("yyyy-MM-dd");
    QString SQL = "SELECT *FROM device_record WHERE  Device_Record_Time > '"+stime+"' ORDER BY Device_Record_Time DESC";
    qDebug()<<SQL;
    QSqlQuery query;
    if(query.exec(SQL))
    {
        int ti = 0;
        ui->QDeviceRecord->setRowCount(ti);
        while(query.next())
        {
            ui->QDeviceRecord->setRowCount(ti + 1);
            QTableWidgetItem *tabledmItem0 = new QTableWidgetItem(QString::number(ti+1));
            tabledmItem0->setTextAlignment(Qt::AlignCenter);
            ui->QDeviceRecord->setItem(ti, 0, tabledmItem0);
            QTableWidgetItem *tabledmItem1 = new QTableWidgetItem(QString(query.value(1).toByteArray().data()));
            tabledmItem1->setTextAlignment(Qt::AlignCenter);
            ui->QDeviceRecord->setItem(ti, 1, tabledmItem1);
            QTableWidgetItem *tabledmItem2 = new QTableWidgetItem(QString(query.value(3).toByteArray().data()));
            tabledmItem2->setTextAlignment(Qt::AlignCenter);
            ui->QDeviceRecord->setItem(ti, 2, tabledmItem2);
            QTableWidgetItem *tabledmItem3 = new QTableWidgetItem(QString(query.value(2).toByteArray().data()));
            tabledmItem3->setTextAlignment(Qt::AlignCenter);
            ui->QDeviceRecord->setItem(ti, 3, tabledmItem3);
            QTableWidgetItem *tabledmItem4 = new QTableWidgetItem(QString(query.value(4).toDateTime().toString("yyyy-MM-dd hh:mm:ss")));
            tabledmItem4->setTextAlignment(Qt::AlignCenter);
            ui->QDeviceRecord->setItem(ti, 4, tabledmItem4);
//            QTableWidgetItem *tabledmItem5 = new QTableWidgetItem(QString(query.value(5).toByteArray().data()));
//            tabledmItem5->setTextAlignment(Qt::AlignCenter);
//            ui->QDevice->setItem(ti, 5, tabledmItem5);
            ti++;
        }
    }
}

void COFFICEPARKS::_UpdateSystemRecord()
{
    QDateTime dateTime=QDateTime::currentDateTime();
    dateTime = dateTime.addDays(-15);
    QString stime = dateTime.toString("yyyy-MM-dd ");
    QString SQL = "SELECT *FROM system_record WHERE  System_Record_Time > '"+stime+"' ORDER BY System_Record_Time DESC";
    QSqlQuery query;
    if(query.exec(SQL))
    {
        int ti = 0;
        ui->QSystem->setRowCount(ti);
        while(query.next())
        {
            ui->QSystem->setRowCount(ti + 1);
            QTableWidgetItem *tabledmItem0 = new QTableWidgetItem(QString::number(ti+1));
            tabledmItem0->setTextAlignment(Qt::AlignCenter);
            ui->QSystem->setItem(ti, 0, tabledmItem0);
            QTableWidgetItem *tabledmItem1 = new QTableWidgetItem(QString(query.value(1).toByteArray().data()));
            tabledmItem1->setTextAlignment(Qt::AlignCenter);
            ui->QSystem->setItem(ti, 1, tabledmItem1);
            QTableWidgetItem *tabledmItem2 = new QTableWidgetItem(QString(query.value(2).toByteArray().data()));
            tabledmItem2->setTextAlignment(Qt::AlignCenter);
            ui->QSystem->setItem(ti, 2, tabledmItem2);
            QTableWidgetItem *tabledmItem3 = new QTableWidgetItem(QString(query.value(3).toByteArray().data()));
            tabledmItem3->setTextAlignment(Qt::AlignCenter);
            ui->QSystem->setItem(ti, 3, tabledmItem3);
            QTableWidgetItem *tabledmItem4 = new QTableWidgetItem(QString(query.value(4).toDateTime().toString("yyyy-MM-dd hh:mm:ss")));
            tabledmItem4->setTextAlignment(Qt::AlignCenter);
            ui->QSystem->setItem(ti, 4, tabledmItem4);
            ti++;
        }
    }
}
//补卡表更新
void COFFICEPARKS::_UpdateAlarmRecord()
{
    QDateTime dateTime=QDateTime::currentDateTime();
    dateTime = dateTime.addDays(-15);
    QString stime = dateTime.toString("yyyy-MM-dd");
    QString SQL = "SELECT a.Alarm_Ip_Addr,a.Alarm_Intra_Addr,a.Alarm_Fence,a.Alarm_Type,a.Alarm_Stime,a.Alarm_Dtime,\
            a.Alarm_Etime,a.Alarm_Status,a.Alarm_Deal_Human,a.Alarm_deal_message,a.Alarm_Device_Type \
            FROM alarm a WHERE  a.Alarm_stime > '"+stime+"' ORDER BY  a.Alarm_dtime,a.Alarm_stime DESC";
    QSqlQuery query;
    if(query.exec(SQL))
    {
        int ti = 0;
        ui->QAlarmRecord->setRowCount(ti);
        while(query.next())
        {
            ui->QAlarmRecord->setRowCount(ti + 1);
            QTableWidgetItem *tabledmItem0 = new QTableWidgetItem(QString::number(ti+1));
            tabledmItem0->setTextAlignment(Qt::AlignCenter);
            ui->QAlarmRecord->setItem(ti, 0, tabledmItem0);
            QTableWidgetItem *tabledmItem1 = new QTableWidgetItem(QString(query.value(0).toByteArray().data()));
            tabledmItem1->setTextAlignment(Qt::AlignCenter);
            ui->QAlarmRecord->setItem(ti, 1, tabledmItem1);
            QTableWidgetItem *tabledmItem2 = new QTableWidgetItem(QString(query.value(1).toByteArray().data()));
            tabledmItem2->setTextAlignment(Qt::AlignCenter);
            ui->QAlarmRecord->setItem(ti, 2, tabledmItem2);
            QTableWidgetItem *tabledmItem3 = new QTableWidgetItem(QString(query.value(2).toByteArray().data()));
            tabledmItem3->setTextAlignment(Qt::AlignCenter);
            ui->QAlarmRecord->setItem(ti, 3, tabledmItem3);
            QTableWidgetItem *tabledmItem4 = new QTableWidgetItem(QString(query.value(3).toByteArray().data()));
            tabledmItem4->setTextAlignment(Qt::AlignCenter);
            ui->QAlarmRecord->setItem(ti, 4, tabledmItem4);
            QTableWidgetItem *tabledmItem5 = new QTableWidgetItem(query.value(4).toDateTime().toString("yyyy-MM-dd hh:mm:ss"));
            tabledmItem5->setTextAlignment(Qt::AlignCenter);
            ui->QAlarmRecord->setItem(ti, 5, tabledmItem5);
            QTableWidgetItem *tabledmItem6 = new QTableWidgetItem(query.value(5).toDateTime().toString("yyyy-MM-dd hh:mm:ss"));
            tabledmItem6->setTextAlignment(Qt::AlignCenter);
            ui->QAlarmRecord->setItem(ti, 6, tabledmItem6);
            QTableWidgetItem *tabledmItem7 = new QTableWidgetItem(query.value(6).toDateTime().toString("yyyy-MM-dd hh:mm:ss"));
            tabledmItem7->setTextAlignment(Qt::AlignCenter);
            ui->QAlarmRecord->setItem(ti, 7, tabledmItem7);
            QTableWidgetItem *tabledmItem8 = new QTableWidgetItem(query.value(7).toString());
            tabledmItem8->setTextAlignment(Qt::AlignCenter);
            ui->QAlarmRecord->setItem(ti, 8, tabledmItem8);
            QTableWidgetItem *tabledmItem9 = new QTableWidgetItem(query.value(8).toString());
            tabledmItem9->setTextAlignment(Qt::AlignCenter);
            ui->QAlarmRecord->setItem(ti, 9, tabledmItem9);
            QTableWidgetItem *tabledmItem10 = new QTableWidgetItem(query.value(9).toString());
            tabledmItem10->setTextAlignment(Qt::AlignCenter);
            ui->QAlarmRecord->setItem(ti, 10, tabledmItem10);
            ti++;
        }
    }
}

void COFFICEPARKS::SlotInitDevice()
{
    _UpdateDevice(CUdpServer->m_DeviceItem);
}

void COFFICEPARKS::SlotSendLamp(char *buf,QString strIp)
{
    CUdpServer->_SendLamp(buf,strIp);
}

void COFFICEPARKS::SlotUpdateDevice(QVariant dataVar)
{
    SDevice data;
    data = dataVar.value<SDevice>();

    int ti = data.nId ;

    QTableWidgetItem *tabledmItem0 = new QTableWidgetItem(QString::number(ti+1));
    tabledmItem0->setTextAlignment(Qt::AlignCenter);
    ui->QDevice->setItem(ti, 0, tabledmItem0);

    QTableWidgetItem *tabledmItem1 = new QTableWidgetItem(QString(data.gcAddr));
    tabledmItem1->setTextAlignment(Qt::AlignCenter);
    ui->QDevice->setItem(ti, 1, tabledmItem1);

    QTableWidgetItem *tabledmItem2 = new QTableWidgetItem(QString(data.gcMacAddr));
    tabledmItem2->setTextAlignment(Qt::AlignCenter);
    ui->QDevice->setItem(ti, 2, tabledmItem2);

    QTableWidgetItem *tabledmItem3 = new QTableWidgetItem(QString(data.gcIpAddr));
    tabledmItem3->setTextAlignment(Qt::AlignCenter);
    ui->QDevice->setItem(ti, 3, tabledmItem3);

    if(data.nState <= 0)
    {
        QTableWidgetItem *tabledmItem4 = new QTableWidgetItem("未连接");
        tabledmItem4->setTextAlignment(Qt::AlignCenter);
        ui->QDevice->setItem(ti, 4, tabledmItem4);
    }else{
        QTableWidgetItem *tabledmItem4 = new QTableWidgetItem("已连接");
        tabledmItem4->setTextAlignment(Qt::AlignCenter);
        ui->QDevice->setItem(ti, 4, tabledmItem4);
    }

    if(data.nFenceState == 0)
    {
        QTableWidgetItem *tabledmItem5 = new QTableWidgetItem("撤防");
        tabledmItem5->setTextAlignment(Qt::AlignCenter);
        ui->QDevice->setItem(ti, 5, tabledmItem5);
    }else{
        switch(data.nFenceState)
        {
        case 1:
        {
            QTableWidgetItem *tabledmItem5 = new QTableWidgetItem("外出布防");
            tabledmItem5->setTextAlignment(Qt::AlignCenter);
            ui->QDevice->setItem(ti, 5, tabledmItem5);
            break;
        }
        case 2:
        {
            QTableWidgetItem *tabledmItem5 = new QTableWidgetItem("在家布防");
            tabledmItem5->setTextAlignment(Qt::AlignCenter);
            ui->QDevice->setItem(ti, 5, tabledmItem5);
            break;
        }
        default :
        {
            QTableWidgetItem *tabledmItem5 = new QTableWidgetItem("未知布防");
            tabledmItem5->setTextAlignment(Qt::AlignCenter);
            ui->QDevice->setItem(ti, 5, tabledmItem5);
            break;
        }
        }
    }

    QTableWidgetItem *tabledmItem6 = new QTableWidgetItem(QString(data.gcType));
    tabledmItem6->setTextAlignment(Qt::AlignCenter);
    ui->QDevice->setItem(ti, 6, tabledmItem6);

    if(data.nState <= 0)
    {
        for(int j = 0;j < 7;j++)
        {
            QTableWidgetItem *item = ui->QDevice->item(ti, j);
            item->setTextColor(QColor(0,0,0));
        }
    }else{
        for(int j = 0;j < 7;j++)
        {
            QTableWidgetItem *item = ui->QDevice->item(ti, j);
            item->setTextColor(Qt::blue);
        }
    }
}

void COFFICEPARKS::SlotOnlineDevice(int nId,int Alarm)
{
    QTableWidgetItem *tabledmItem8 = new QTableWidgetItem("未连接");
    tabledmItem8->setTextAlignment(Qt::AlignCenter);
    ui->QDevice->setItem(nId, 4, tabledmItem8);
    qDebug()<<nId<<Alarm;
    if(Alarm)
    {
        qDebug()<<"hong";
        QTableWidgetItem *item = ui->QDevice->item(nId, 4);
        item->setBackgroundColor(QColor(255,0,0));
    }
    for(int j = 0;j < 7;j++)
    {
        QTableWidgetItem *item = ui->QDevice->item(nId, j);
        item->setTextColor(QColor(0,0,0));
    }
}

void COFFICEPARKS::SlotPersonnel_Staff_Leave(int Id)
{
    CUdpServer->_StaffLeave(Id);
}

void COFFICEPARKS::SlotDeal()
{
    qDebug()<<m_nRecord;
    if(m_nRecord != 1){
    int ti = ui->QAlarm->currentRow();
    if(ti >= 0 )
    {
            QString strStime = ui->QAlarm->item(ti, 5)->text();
            SAlarmDevice sAlarmDevice;
            memset(sAlarmDevice.gcIntraAddr,0,20);
            memset(sAlarmDevice.gcPhone1,0,20);
            memset(sAlarmDevice.gcIpAddr,0,20);
            memset(sAlarmDevice.gcMacAddr,0,20);
            memset(sAlarmDevice.gcStime,0,20);
            memset(sAlarmDevice.gcEtime,0,20);
            memset(sAlarmDevice.gcType,0,20);
            QByteArray byte;
            byte = strStime.toUtf8();
            memcpy(sAlarmDevice.gcStime,byte.data(),byte.size());
            if(FindItemAlarm(sAlarmDevice,CUdpServer->m_AlarmItem))//是否实时的报警
            {
                CUdpServer->m_AlarmItem = DeleteItemAlarm(&sAlarmDevice,&(CUdpServer->m_AlarmItem));
                _UpdateAlarm(CUdpServer->m_AlarmItem);
               //查找链表
                int isNULL = AddrFindAlarmItem(sAlarmDevice,CUdpServer->m_AlarmItem);
                //是否更新实时表
                if(isNULL == 1)
                {
                    CUdpServer->m_DeviceItem = AlarmUpdateDeviceItem(sAlarmDevice,CUdpServer->m_DeviceItem);
                    _UpdateDevice(CUdpServer->m_DeviceItem);
                }
            }

            //数据库更新
            CUdpServer->m_Mysql._UpdateDealAlarm(sAlarmDevice,"误报","1","11");
            _UpdateAlarmRecord();
    }else{
        QMessageBox::information(this,tr("提示"),tr("请选中目标"));
    }
    }else{
        int ti = ui->QAlarmRecord->currentRow();
        if(ti >= 0 )
        {
            QString strDtime = ui->QAlarmRecord->item(ti, 6)->text();
            if(strDtime == NULL)
            {
                SAlarmDevice sAlarmDevice;
                memset(sAlarmDevice.gcIntraAddr,0,20);
                memset(sAlarmDevice.gcPhone1,0,20);
                memset(sAlarmDevice.gcIpAddr,0,20);
                memset(sAlarmDevice.gcMacAddr,0,20);
                memset(sAlarmDevice.gcStime,0,20);
                memset(sAlarmDevice.gcEtime,0,20);
                memset(sAlarmDevice.gcType,0,20);
                QString strStime = ui->QAlarmRecord->item(ti, 5)->text();
                QByteArray byte;
                byte = strStime.toUtf8();
                memcpy(sAlarmDevice.gcStime,byte.data(),byte.size());
                if(FindItemAlarm(sAlarmDevice,CUdpServer->m_AlarmItem))//是否实时的报警
                {
                    CUdpServer->m_AlarmItem = DeleteItemAlarm(&sAlarmDevice,&(CUdpServer->m_AlarmItem));
                    _UpdateAlarm(CUdpServer->m_AlarmItem);
                   //查找链表
                    int isNULL = AddrFindAlarmItem(sAlarmDevice,CUdpServer->m_AlarmItem);
                    //是否更新实时表
                    if(isNULL == 1)
                    {
                        CUdpServer->m_DeviceItem = AlarmUpdateDeviceItem(sAlarmDevice,CUdpServer->m_DeviceItem);
                        _UpdateDevice(CUdpServer->m_DeviceItem);
                    }
                }

                //数据库更新
                CUdpServer->m_Mysql._UpdateDealAlarm(sAlarmDevice,"误报","1","11");
                _UpdateAlarmRecord();

            }else{
                QMessageBox::information(this,tr("提示"),tr("已处理过的记录"));
                return;
            }
        }else{
    QMessageBox::information(this,tr("提示"),tr("请选中目标"));
    }
    }
}

void COFFICEPARKS::SlotRecvLogOut()
{
    Personnel->_UpdateToStaff();
}

void COFFICEPARKS::SlotInsertAlarm()
{
    _UpdateDevice(CUdpServer->m_DeviceItem);
    _UpdateAlarm(CUdpServer->m_AlarmItem);
}

//智能家居控制
void COFFICEPARKS::SlotSmartControl()
{
    int i = ui->QDevice->currentRow();
    //QTableWidgetItem * item = ui->QTenement->currentItem();
    if(i >= 0)
    {
    QString strType = ui->QDevice->item(i, 11)->text();
    if(strType != "室内机"){
    QString strIp = ui->QDevice->item(i, 6)->text();
    QString strAddr = ui->QDevice->item(i, 1)->text();
    CSmartControl->_Updata(strIp,strAddr,strType);
    CSmartControl->show();
    }else{
        QMessageBox::information(this,tr("提示"),tr("请选择室内机"));
    }
    }else{
        QMessageBox::information(this,tr("提示"),tr("请选中目标"));
    }
}

void COFFICEPARKS::SlotReset()
{
    int i = ui->QDevice->currentRow();
    if(i >= 0)
    {
    QString strIp = ui->QDevice->item(i, 3)->text();
    QString strAddr = ui->QDevice->item(i, 1)->text();
    QString strType = ui->QDevice->item(i, 6)->text();
    CUdpServer->_Reset(strIp,strAddr,strType);
    }else{
        QMessageBox::information(this,tr("提示"),tr("请选中目标"));
    }
}

void COFFICEPARKS::SlotDelete()
{
    int nRow = ui->QDevice->currentRow();
    if(nRow >= 0)
    {
        SDevice sDevice;
        memset(sDevice.gcMacAddr,0,20);

        QString strMac = ui->QDevice->item(nRow, 2)->text();
        QString strIp = ui->QDevice->item(nRow, 3)->text();
        QString strAddr = ui->QDevice->item(nRow, 1)->text();
        QString strType = ui->QDevice->item(nRow, 6)->text();

        switch( QMessageBox::question( this, tr("提示"),"确定要删除"+strType+strAddr,tr("Yes"), tr("No"),0, 1 ) )
        {
        case 0:
        {
            if(CUdpServer->m_Mysql._DeleteDevice(strMac,strIp,strAddr,strType))
            {
                QByteArray byte;
                byte = strMac.toAscii();
                memcpy(sDevice.gcMacAddr,byte.data(),byte.size());
                CUdpServer->m_DeviceItem = DeleteItemDevice(sDevice,&(CUdpServer->m_DeviceItem));
                _UpdateDevice(CUdpServer->m_DeviceItem);
                QMessageBox::information(this,tr("提示"),tr("删除成功"));
            }
        }
        break;

        case 1:
            break;
        default :
            break;
        }

    }else{
        QMessageBox::information(this,tr("提示"),tr("请选中目标"));
    }
}

void COFFICEPARKS::SlotPing()
{
    QString strIp;
    int nRow = ui->QDevice->currentRow();
    if(nRow >= 0)
    {
    strIp = ui->QDevice->item(nRow, 3)->text();
    QStringList args;
    QString str = "ping " + strIp;
    args << "/K" << str;
    QProcess::startDetached("cmd.exe",args);
    }else{
        QMessageBox::information(this,tr("提示"),tr("请选中目标"));
    }

}

//人员信息修改
void COFFICEPARKS::SlotInitPass()
{
    int nRow = ui->QDevice->currentRow();
    if(nRow >= 0)
    {
        QString strMac = ui->QDevice->item(nRow, 2)->text();
        QString strIp = ui->QDevice->item(nRow, 3)->text();
        QString strAddr = ui->QDevice->item(nRow, 1)->text();
        QString strType = ui->QDevice->item(nRow, 6)->text();
        switch( QMessageBox::question( this, tr("提示"),"确定要重置"+strType+strAddr+" 的工程密码",tr("Yes"), tr("No"),0, 1 ) )
      {
      case 0:
        CUdpServer->_SendPass(strAddr,strMac,strIp);
        break;
      default :
          break;
      }
    }else{
        QMessageBox::information(this,tr("提示"),tr("请选中目标"));
    }
}

void COFFICEPARKS::SlotRegister()
{
    int nRow = ui->QDevice->currentRow();
    QString strIP = ui->QDevice->item(nRow,3)->text();
    QString strType = ui->QDevice->item(nRow,6)->text();
    if(strType == "二次门口机")
    {
    QString strExplainAddr = ui->QDevice->item(nRow,1)->text();
    QString strAdd = _AddrExplainToAddr(strExplainAddr,strType);
//    qDebug()<<strAdd;
    CUdpServer->_Register(strAdd,strIP);

    }else{
        QMessageBox::information(this,tr("提示"),tr("请选择门口机"));
    }
}


QString COFFICEPARKS::_AddrExplainToAddr(QString AddrExplain,QString strType)
{
    QString str;
    if(strType == "二次门口机"){
        QString str1 = AddrExplain;
        str = str1.left(4);
        str1 = str1.right(str1.length() - 5);

        str = str + str1.left(3);
        str1 = str1.right(str1.length() - 5);

        str = str + str1.left(3);
        str1 = str1.right(str1.length() - 4);

        str = str + str1.left(3);
        str1 = str1.right(str1.length() - 4);

        str = str + str1.left(3);
    }else if(strType == "围墙机"){
        QString str1 = AddrExplain;
        str = str1.left(4);
        str1 = str1.right(str1.length() - 5);

        str = str + str1.left(3);
        str1 = str1.right(str1.length() - 5);

        str = str +"000000"+ str1.left(3);
    }else if(strType == "单元门口机"){
        QString str1 = AddrExplain;
        str = str1.left(4);
        str1 = str1.right(str1.length() - 5);

        str = str + str1.left(3);
        str1 = str1.right(str1.length() - 5);

        str = str +"000000"+ str1.left(3);
    }
     return str;
    /*
    QString str1 = AddrExplain;
    QString str;
    str = str1.left(4);
    str1 = str1.right(str1.length() - 5);

    str = str + str1.left(3);
    str1 = str1.right(str1.length() - 5);

    str = str + str1.left(3);
    str1 = str1.right(str1.length() - 4);

    str = str + str1.left(3);
    str1 = str1.right(str1.length() - 4);

    str = str + str1.left(3);
    return str;*/
}

//按键功能
void COFFICEPARKS::on_QpbtnEquipment_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    m_nRecord = 4;
}

void COFFICEPARKS::on_QpbtnPersonnel_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    Personnel->_SetInit();
}

void COFFICEPARKS::on_QpbtnAttendance_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}

void COFFICEPARKS::on_QpbtnSystem_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
    CSystem->_UpdateToUser(m_sUser);
}





void COFFICEPARKS::on_QDevice_customContextMenuRequested(const QPoint &pos)
{
    DeviceMenu->addAction(DeleteAction);
    DeviceMenu->addAction(ResetAction);
    DeviceMenu->addAction(PingAction);
    DeviceMenu->addAction(RegisterAction);
    DeviceMenu->addAction(SmartControlAction);
    DeviceMenu->addAction(InitPassAction);
    DeviceMenu->exec(QCursor::pos());
}



void COFFICEPARKS::on_QpbtnAlarm_clicked()
{
    ui->QstackedDevice->setCurrentIndex(0);
    _UpdateAlarmRecord();
    m_nRecord = 1;
}

void COFFICEPARKS::on_QpbtnDeviceRecord_clicked()
{
    ui->QstackedDevice->setCurrentIndex(1);
    _UpdateDeviceRecord();
    m_nRecord = 2;
}


void COFFICEPARKS::on_QpbtnSystemRecord_clicked()
{
    ui->QstackedDevice->setCurrentIndex(2);
    _UpdateSystemRecord();
    m_nRecord = 3;
}

void COFFICEPARKS::on_QpbtnRecord_clicked()
{
    ui->stackedWidget->setCurrentIndex(5);
}

void COFFICEPARKS::on_QAlarm_customContextMenuRequested(const QPoint &pos)
{
    AlarmMenu->addAction(DealAction);
    AlarmMenu->exec(QCursor::pos());
}

void COFFICEPARKS::on_QAlarmRecord_customContextMenuRequested(const QPoint &pos)
{
    AlarmRecordMenu->addAction(DeleteRecordAction);
    AlarmRecordMenu->addAction(FindRecordAction);
    AlarmRecordMenu->addAction(DealAction);
    AlarmRecordMenu->exec(QCursor::pos());
}

void COFFICEPARKS::on_QSystem_customContextMenuRequested(const QPoint &pos)
{
    SystemRecordMenu->addAction(DeleteRecordAction);
    SystemRecordMenu->addAction(FindRecordAction);
    SystemRecordMenu->exec(QCursor::pos());
}

void COFFICEPARKS::on_QDeviceRecord_customContextMenuRequested(const QPoint &pos)
{
    DeviceRecordMenu->addAction(DeleteRecordAction);
    DeviceRecordMenu->addAction(FindRecordAction);
    DeviceRecordMenu->exec(QCursor::pos());
}

void COFFICEPARKS::_InitTray()
{
    QIcon icon = QIcon(":/pic/tu.png");
    TrayIcon = new QSystemTrayIcon(this);
    TrayIcon->setIcon(icon);
    TrayIcon->setToolTip("中心管理系统");
    TrayIcon->show();
}
