#include "officepark.h"
#include "ui_officepark.h"

COFFICEPARK::COFFICEPARK(SUser sUser,basicWindows *parent) :
    basicWindows(parent),
    ui(new Ui::COFFICEPARK)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/pic/OfficeParks.png"));
    this->setWindowTitle("中心管理系统");
    ui->stackedWidget->setCurrentIndex(7);
    m_sUser = sUser;
    setWindowFlags(Qt::WindowMinimizeButtonHint);   //还原按钮不可用

    CUdpServer = new CUDPSERVER("m_sUser.strCityNum");
    CUdpServer->start();        //启动线程

    CDelete = new CDELETE();
    CFind = new CFIND();
    _Init(this->size().width(),this->size().height());  //初始化窗口的位置和大小
    ptbnNum = 0;        //模块选择按键动态效果切换
    AlarmCurPage = 1;
    DeviceCurPage = 1;
    SystemCurPage = 1;

    setAutoFillBackground(true);

    SystemRecordMenu = new QMenu(ui->QSystem);
    AlarmRecordMenu = new QMenu(ui->QAlarmRecord);
    DeviceRecordMenu = new QMenu(ui->QDeviceRecord);
    CentralwidgetMenu = new QMenu(ui->centralwidget);

    FindRecordAction = new QAction(tr("查找"),this);
    DeleteRecordAction = new QAction(tr("删除"),this);
    DealAction = new QAction(tr("处理"),this);
    Logout = new QAction(tr("注销"),this);
    NextPage = new QAction(tr("下一页"), this);
    PrevPage = new QAction(tr("上一页"), this);

    connect(FindRecordAction, SIGNAL(triggered()), this, SLOT(SlotFindRecord()));
    connect(DeleteRecordAction, SIGNAL(triggered()), this, SLOT(SlotDeleteRecord()));
    connect(DealAction, SIGNAL(triggered()), this, SLOT(SlotDeal()));
    connect(NextPage, SIGNAL(triggered()), this, SLOT(SlotNextPage()));
    connect(PrevPage, SIGNAL(triggered()), this, SLOT(SlotPrevPage()));
    connect(CDelete,SIGNAL(SigDelete()),this,SLOT(SlotDeleteSuc()));
    connect(CFind,SIGNAL(SigFind(int,int,QString,QString,QString)),this,SLOT(SlotFind(int,int,QString,QString,QString)));

    ui->QSystem->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->QAlarmRecord->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->QDeviceRecord->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->centralwidget->setContextMenuPolicy(Qt::CustomContextMenu);

    _InitPrivilegeClass();  //初始化函数：权限项数据
    _InitSystem();          //系统日志窗口初始化
    _InitAlarmRecord();     //报警日志窗口初始化
    _InitDeviceRecord();    //设备日志窗口初始化
    _InitLogClass();        //日志树初始化
    _InitTray();            //初始化函数：托盘设置
    _Interface();
    setWindowFlags(Qt::FramelessWindowHint );

    Equipment = new equipment(CUdpServer->m_DeviceItem, CUdpServer->m_AlarmItem);//创建“设备”新窗口对象
    layout = new QVBoxLayout;//新窗口画图到控件上
    layout->addWidget(Equipment);
    layout->setMargin(0);
    ui->QstackeEquipment->setLayout(layout);

    connect(Equipment, SIGNAL(SigUdpSendPass(QString,QString,QString)), this, SLOT(SlotUdpSendPass(QString,QString,QString)));
    connect(Equipment, SIGNAL(SigUdpReset(QString,QString,QString)), this, SLOT(SlotUdpReset(QString,QString,QString)));
    connect(Equipment, SIGNAL(SigUdpRegister(QString,QString)), this, SLOT(SlotUdpRegister(QString,QString)));
    connect(Equipment, SIGNAL(SigUdpSendLamp(char*,QString)), this, SLOT(SlotUdpSendLamp(char*,QString)));
    connect(Equipment, SIGNAL(SigUdpserTocli(QString,unsigned char*,int)), CUdpServer, SLOT(SlotUdpserTocli(QString,unsigned char*,int)));
    //智能家居测试函数
    connect(Equipment, SIGNAL(SigDragSmart(QString,QString)), CUdpServer, SLOT(SlotDragSmart(QString,QString)));

    connect(Equipment, SIGNAL(SigMessage(QString, QString, QString, QString, QString, QString, QString)), CUdpServer, SLOT(SlotRevMessage(QString, QString, QString, QString, QString, QString, QString)));

//    connect(CUdpServer, SIGNAL(SigInitDevice()), this, SLOT(ToInitDevice()));       //更新设备
//    connect(this, SIGNAL(SigInitDevice2(NodeDevice*)), Equipment, SLOT(SlotInitDevice(NodeDevice*)));
    connect(CUdpServer, SIGNAL(SigInitDevice()), Equipment, SLOT(SlotInitDevice()));

    connect(CUdpServer, SIGNAL(SigUpdateDevice(QVariant)), Equipment, SLOT(SlotUpdateDevice(QVariant)));//实时更新设备状态
    connect(CUdpServer, SIGNAL(SigOnlineDevice(int,int)),  Equipment, SLOT(SlotOnlineDevice(int,int)));//设备掉线
    connect(CUdpServer, SIGNAL(SigRecvInitPass(QString,int)), Equipment, SLOT(SlotRecvInitPass(QString,int)));//初始化密码回复

    connect(CUdpServer, SIGNAL(SigInsertAlarm()), this, SLOT(ToInsertAlarm()));     //插入报警
    connect(this, SIGNAL(SigInsertAlarm2(NodeDevice*,NodeAlarm*)), Equipment, SLOT(SlotInsertAlarm(NodeDevice*,NodeAlarm*)));
    connect(CUdpServer, SIGNAL(SigInsertAlarm()), this, SLOT(SlotUpdateAlarm()));   //刷新“报警日志”

    connect(Equipment, SIGNAL(SigCancleAlarm()), this, SLOT(CancleAlarm()));    //更新取消报警链表
    connect(this, SIGNAL(SigCancleAlarm(NodeDevice*)), CUdpServer, SLOT(SlotCancleAlarm(NodeDevice*)));
    connect(Equipment, SIGNAL(SigCancleAlarm2()), this, SLOT(CancleAlarm2()));
    connect(this, SIGNAL(SigCancleAlarm2(NodeAlarm*)), CUdpServer, SLOT(SlotCancleAlarm2(NodeAlarm*)));

    connect(CUdpServer, SIGNAL(SigRecvLogOut()), this, SLOT(SlotRecvLogOut()));     //员工注销
    connect(CUdpServer, SIGNAL(SigUdpOpenFail()), this, SLOT(SlotUdpOpenFail()));   //Udp打开失败
    connect(CUdpServer, SIGNAL(SigReset()), this, SLOT(SlotReset()));   //设备重启反馈

    Personnel = new CPERSONNEL;     //创建“人事”新窗口对象
    layout = new QVBoxLayout;       //新窗口画图到控件上
    layout->addWidget(Personnel);
    layout->setMargin(0);
    ui->QstackePersonnel->setLayout(layout);

    connect(Personnel,SIGNAL(SigPersonnel_Staff_Leave(int)),this,SLOT(SlotPersonnel_Staff_Leave(int)));
    connect(Personnel,SIGNAL(SigDeleteAllStaff()),CUdpServer,SLOT(SlotDeleteAllStaff()));
    connect(CUdpServer, SIGNAL(SigRecvDeleteStaff()), Personnel, SLOT(SlotRecvDeleteAllStaff()));

    Attendance = new CATTENDANCE();     //创建“考勤”新窗口对象
    layout = new QVBoxLayout;           //新窗口画图到控件上
    layout->addWidget(Attendance);
    layout->setMargin(0);
    ui->QstackeAttendance->setLayout(layout);

    connect(CUdpServer, SIGNAL(SigProgress(int,int)), Attendance, SLOT(SlotProgress(int,int)));
    connect(Attendance, SIGNAL(SigAcquisition(QString,QString)), this, SLOT(SlotAcquisition(QString,QString)));

    System = new CSYSTEM;           //创建“权限”新窗口对象
    layout = new QVBoxLayout;       //新窗口画图到控件上
    layout->addWidget(System);
    layout->setMargin(0);
    ui->QDevice_2->setLayout(layout);

    About = new about;
    layout = new QVBoxLayout;
    layout->addWidget(About);
    layout->setMargin(0);
    ui->QstackeAbout->setLayout(layout);

    connect(this,SIGNAL(SigEquipmentClick()),Equipment,SLOT(SlotEquipmentClick()));
    connect(this,SIGNAL(SigPersonnelClick()),Personnel,SLOT(SlotPersonnelClick()));
    connect(this,SIGNAL(SigAttendanceClick()),Attendance,SLOT(SlotAttendanceClick()));
    connect(this,SIGNAL(SigEquipmentFocusOut()),Equipment,SLOT(SlotFocusOut()));
    connect(this,SIGNAL(SigPersonnelFocusOut()),Personnel,SLOT(SlotFocusOut()));
    connect(this,SIGNAL(SigAttendanceFocusOut()),Attendance,SLOT(SlotFocusOut()));

    connect(this, SIGNAL(Sigquit()), qApp, SLOT(quit()));

    ui->Photo_lineEdit->setEnabled(false);
}

COFFICEPARK::~COFFICEPARK()
{
    delete ui;
}
//收到报警信号之后，立即刷新“报警日志”
void COFFICEPARK::SlotUpdateAlarm()
{
    _UpdateAlarmRecord("");
}

//更新取消报警后的设备链表
void COFFICEPARK::CancleAlarm()
{
    emit SigCancleAlarm(Equipment->m_Device);
}
//更新取消报警后的报警链表
void COFFICEPARK::CancleAlarm2()
{
    emit SigCancleAlarm2(Equipment->m_Alarm);
}

//更新"Equipment"设备列表
void COFFICEPARK::ToInitDevice()
{
    emit SigInitDevice2(CUdpServer->m_DeviceItem);
}

//插入报警
void COFFICEPARK::ToInsertAlarm()
{
    emit SigInsertAlarm2(CUdpServer->m_DeviceItem, CUdpServer->m_AlarmItem);
}

//考勤数据采集
void COFFICEPARK::SlotAcquisition(QString Addr, QString IpAddr)
{
    CUdpServer->_Acqkuisition(Addr, IpAddr);
}

//初始化密码
void COFFICEPARK::SlotUdpSendPass(QString strAddr, QString strMac, QString strIP)
{
    CUdpServer->_SendPass(strAddr, strMac, strIP);
}

//设备重启
void COFFICEPARK::SlotUdpReset(QString strIp, QString strAddr, QString strType)
{
    CUdpServer->_Reset(strIp, strAddr, strType);
}

//设备重启反馈
void COFFICEPARK::SlotReset()
{
    QMessageBox::information(this,tr("提示"),tr("重启成功"));
}

//注册模式
void COFFICEPARK::SlotUdpRegister(QString strAddr, QString strIp)
{
    CUdpServer->_Register(strAddr, strIp);
}

void COFFICEPARK::SlotUdpSendLamp(char *buf, QString strIp)
{
    CUdpServer->_SendLamp(buf, strIp);
}

void COFFICEPARK::paintEvent(QPaintEvent *event)
{
//   QPainter painter(this);
//   DropShadowWidget::paintEvent(event);
//   QPainter painter(this);
//   painter.setPen(Qt::NoPen);
//   painter.setBrush(Qt::white);
//   painter.drawPixmap(QRect(SHADOW_WIDTH, SHADOW_WIDTH, this->width()-2*SHADOW_WIDTH, this->height()-2*SHADOW_WIDTH), QPixmap(":/images/OfficeParks1.png"));
}

void COFFICEPARK::_UpdateLoading(SUser sUser)
{
    m_sUser = sUser;
//    ui->stackedWidget->setCurrentIndex(7);  //主界面
    System->_UpdateToUser(m_sUser);     //更新“账户管理”列表
}

//初始化函数：“账户管理”
void COFFICEPARK::_InitPrivilegeClass()
{
    ui->QType->setColumnCount(1);
    ui->QType->setShowGrid(false);//显示表格线
    QStringList headers;
    headers<<"序号";  //在列表尾添加值“序号”
    ui->QType->horizontalHeader()->hide();//隐藏行头
    ui->QType->verticalHeader()->hide();//列头不显示
    ui->QType->horizontalHeader()->setClickable(false);//行头不可选
    ui->QType->setHorizontalHeaderLabels(headers);//设置行头字段为“序号”
    ui->QType->horizontalHeader()->setHighlightSections(false);//表头坍塌,此属性保存是否突出显示选定项目的部分
    ui->QType->resizeColumnsToContents();
    ui->QType->setFont(QFont("Helvetica"));//字体
    ui->QType->setEditTriggers(QAbstractItemView::NoEditTriggers);//禁止编辑
    ui->QType->setSelectionBehavior(QAbstractItemView::SelectRows);  //整行选中的方式：一行
    ui->QType->setColumnWidth(0,160);   //设置列宽

    ui->QType->setRowCount(1);  //行数：1
    QTableWidgetItem *tabledmItem1 = new QTableWidgetItem(QString("账户管理"));
    tabledmItem1->setTextAlignment(Qt::AlignCenter);
    ui->QType->setItem(0, 0, tabledmItem1);
    ui->QType->setRowHeight(0,44);  //行高
}

/********信号函数：“关于”菜单*********/
void COFFICEPARK::on_QpbtnAbout_clicked()
{
    ui->stackedWidget->setCurrentIndex(5);
//    ui->QpbtnAbout->setWindowOpacity(0.99); //设置不透明度，范围：1（完全不透明）~0（完全透明）
    ptbnNum = 6;
    SetAllButtenSheet(ptbnNum);
}

//初始化函数：托盘设置(右下角最小化时的右键菜单设置)
void COFFICEPARK::_InitTray()
{
    QIcon icon = QIcon(":/pic/OfficeParks.png");
    TrayIcon = new QSystemTrayIcon(this);
    TrayIcon->setIcon(icon);
    TrayIcon->setToolTip("中心管理系统");

    Action_min = new QAction(tr("最小化"),this);
    Action_restore = new QAction(tr("还原"),this);    //最大化
    Action_cancel = new QAction(tr("注销"),this);
    Action_quit = new QAction(tr("退出"),this);

    Action_min->setIcon(QIcon(":/images/OfficeParks4-1.png"));
    Action_restore->setIcon(QIcon(":/images/OfficeParks3-1.png"));
    Action_cancel->setIcon(QIcon(":/images/return2.png"));
    Action_quit->setIcon(QIcon(":/images/OfficeParks5-1.png"));

    Menu_pallet = new QMenu();
    Menu_pallet->addAction(Action_min);
    Menu_pallet->addAction(Action_restore);
    Menu_pallet->addAction(Action_cancel);
    Menu_pallet->addAction(Action_quit);

    TrayIcon->setContextMenu(Menu_pallet);
    TrayIcon->show();
    connect(Action_min,SIGNAL(triggered()),this,SLOT(on_QpbtnMin_clicked()));   //最小化
    connect(Action_restore,SIGNAL(triggered()),this,SLOT(Double()));            //还原
    connect(Action_cancel,SIGNAL(triggered()),this,SLOT(SlotLogOut()));         //注销
    connect(Action_quit,SIGNAL(triggered()),this,SLOT(close()));                //退出
    connect(TrayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(IconIsActived(QSystemTrayIcon::ActivationReason)));
}

//托盘图标点击事件
void COFFICEPARK::IconIsActived(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason)
    {
        //点击托盘显示窗口
        case QSystemTrayIcon::Trigger:
        {
            showNormal();
            break;
        }
        //双击托盘显示窗口
        case QSystemTrayIcon::DoubleClick:
        {
            showNormal();
            break;
        }
        default:
            break;
    }
}

//系统日志窗口初始化
void COFFICEPARK::_InitSystem()
{
    ui->QSystem->setColumnCount(5);
    ui->QSystem->setShowGrid(true);//显示表格线
    QStringList headers;
    headers<<"序号"<<"用户名"<<"姓名"<<"操作类型"<<"时间";   //按顺序向列表尾插入字段
    ui->QSystem->verticalHeader()->hide();//列头不显示
    ui->QSystem->horizontalHeader()->setClickable(false);//行头不可选
    ui->QSystem->setHorizontalHeaderLabels(headers);    //设置行头字段
    ui->QSystem->horizontalHeader()->setHighlightSections(false);//不突出显示选中部分
    ui->QSystem->resizeColumnsToContents();     //根据内容自动调整所有列的列宽
    ui->QSystem->setFont(QFont("Helvetica"));
    ui->QSystem->setEditTriggers(QAbstractItemView::NoEditTriggers);   //禁止编辑
    ui->QSystem->setSelectionBehavior(QAbstractItemView::SelectRows);  //整行选中的方式：一行
    ui->QSystem->setColumnWidth(0,100);     //设置每个字段的列宽
    ui->QSystem->setColumnWidth(1,200);
    ui->QSystem->setColumnWidth(2,150);
    ui->QSystem->setColumnWidth(3,150);
    ui->QSystem->setColumnWidth(4,199);
}

//报警日志窗口初始化
void COFFICEPARK::_InitAlarmRecord()
{
    ui->QAlarmRecord->setColumnCount(12);
    ui->QAlarmRecord->setShowGrid(true);//显示表格线
    QStringList headers;
    headers<<"序号"<<"地址"<<"IP地址"<<"报警防区"<<"报警类型"<<"报警时间"<<"确认时间"<<"取消时间"<<"是否误报"<<"处理人"<<"详情"<<"设备类型";
    ui->QAlarmRecord->verticalHeader()->hide();//行头不显示
    ui->QAlarmRecord->horizontalHeader()->setClickable(false);//行头不可选
    ui->QAlarmRecord->setHorizontalHeaderLabels(headers);
    ui->QAlarmRecord->horizontalHeader()->setHighlightSections(false);
    ui->QAlarmRecord->resizeColumnsToContents();
    ui->QAlarmRecord->setFont(QFont("Helvetica"));
    ui->QAlarmRecord->setEditTriggers(QAbstractItemView::NoEditTriggers);//禁止编辑
    ui->QAlarmRecord->setSelectionBehavior(QAbstractItemView::SelectRows);  //整行选中的方式：一行
    ui->QAlarmRecord->setColumnWidth(0,50);
    ui->QAlarmRecord->setColumnWidth(1,150);
    ui->QAlarmRecord->setColumnWidth(2,150);
    ui->QAlarmRecord->setColumnWidth(3,80);
    ui->QAlarmRecord->setColumnWidth(4,100);
    ui->QAlarmRecord->setColumnWidth(5,150);
    ui->QAlarmRecord->setColumnWidth(6,150);
    ui->QAlarmRecord->setColumnWidth(7,150);
    ui->QAlarmRecord->setColumnWidth(8,130);
    ui->QAlarmRecord->setColumnWidth(9,130);
    ui->QAlarmRecord->setColumnWidth(10,130);
    ui->QAlarmRecord->setColumnWidth(11,80);
}

//设备日志窗口初始化
void COFFICEPARK::_InitDeviceRecord()
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
    ui->QDeviceRecord->setColumnWidth(0,100);
    ui->QDeviceRecord->setColumnWidth(1,219);
    ui->QDeviceRecord->setColumnWidth(2,180);
    ui->QDeviceRecord->setColumnWidth(3,150);
    ui->QDeviceRecord->setColumnWidth(4,150);
}

//日志树初始化
void COFFICEPARK::_InitLogClass()
{
    ui->LogTable->setColumnCount(1);//设置为1列
    ui->LogTable->setShowGrid(false);//显示表格线，false为显示？？？
    QStringList headers;
    headers<<"序号";
    ui->LogTable->horizontalHeader()->hide();
    ui->LogTable->verticalHeader()->hide();//行头不显示
    ui->LogTable->horizontalHeader()->setClickable(false);//行头不可选
    ui->LogTable->setHorizontalHeaderLabels(headers);
    ui->LogTable->horizontalHeader()->setHighlightSections(false);//表头坍塌
    ui->LogTable->resizeColumnsToContents();
    ui->LogTable->setFont(QFont("Helvetica"));//字体
    ui->LogTable->setEditTriggers(QAbstractItemView::NoEditTriggers);//禁止编辑
    ui->LogTable->setSelectionBehavior(QAbstractItemView::SelectRows);  //整行选中的方式：一行
    ui->LogTable->setColumnWidth(0,160);

    ui->LogTable->setRowCount(3);//设置为3行
    QTableWidgetItem *tabledmItem0 = new QTableWidgetItem(QString("报警记录"));
    tabledmItem0->setTextAlignment(Qt::AlignCenter);
    ui->LogTable->setItem(0, 0, tabledmItem0);
    ui->LogTable->setRowHeight(0,44);

    QTableWidgetItem *tabledmItem1 = new QTableWidgetItem(QString("设备日志"));
    tabledmItem1->setTextAlignment(Qt::AlignCenter);
    ui->LogTable->setItem(1, 0, tabledmItem1);
    ui->LogTable->setRowHeight(1,44);

    QTableWidgetItem *tabledmItem2 = new QTableWidgetItem(QString("系统日志"));
    tabledmItem2->setTextAlignment(Qt::AlignCenter);
    ui->LogTable->setItem(2, 0, tabledmItem2);
    ui->LogTable->setRowHeight(2,44);
}

/********信号函数："权限"功能中功能列表被点击*********/
void COFFICEPARK::on_QType_clicked(const QModelIndex &index)
{  
//    ui->QType->setStyleSheet("::item:selected{background-color:rgb(138,184,208);}");
    System->_UpdateToUser(m_sUser);
}

/********信号函数：*********/
void COFFICEPARK::on_LogTable_clicked(const QModelIndex &index)
{
    QString SQL;
    if(index.row()==0)  //报警记录被按下
    {
        ui->stackedWidget_2->setCurrentIndex(0);//切换页面
        SQL = "SELECT * FROM alarm ORDER BY Alarm_Id";
        QString CountSQL = "SELECT count(*) FROM alarm ORDER BY Alarm_Id DESC";
        QSqlQuery CountQuery;
        if(CountQuery.exec(CountSQL))
        {
            while(CountQuery.next())
            {
                m_nCount = CountQuery.value(0).toInt();
            }
        }
        _UpdateAlarmRecord(SQL);
        m_nRecord = 1;    //日志类型编号
    }
    else if(index.row()==1) //设备日志被按下
    {
        ui->stackedWidget_2->setCurrentIndex(1);
        SQL = "SELECT * FROM device_record ORDER BY Device_Record_Time DESC";
        QString CountSQL = "SELECT count(*) FROM device_record ORDER BY Device_Record_Time DESC";
        QSqlQuery CountQuery;
        if(CountQuery.exec(CountSQL))
        {
            while(CountQuery.next())
            {
                m_nCount = CountQuery.value(0).toInt();
            }
        }
        _UpdateDeviceRecord(SQL);
        m_nRecord = 2;
    }
    else if(index.row()==2) //系统日志被按下
    {
        ui->stackedWidget_2->setCurrentIndex(2);
        QDateTime dateTime = QDateTime::currentDateTime();
        dateTime = dateTime.addDays(-15);
        QString stime = dateTime.toString("yyyy-MM-dd ");   //只查找最近15天的登录记录
        SQL = "SELECT * FROM system_record WHERE System_Record_Time > '"+stime+"' ORDER BY System_Record_Time DESC";
        QString CountSQL = "SELECT count(*) FROM system_record WHERE System_Record_Time > '"+stime+"' ORDER BY System_Record_Time DESC";
        QSqlQuery CountQuery;
        if(CountQuery.exec(CountSQL))
        {
            while(CountQuery.next())
            {
                m_nCount = CountQuery.value(0).toInt();
            }
        }
        _UpdateSystem(SQL); //更新函数：系统日志数据
        m_nRecord = 3;
    }

    ui->QSystem->setCurrentItem(ui->QSystem->item(-1,-1));
}

/********更新函数：系统日志数据********/
void COFFICEPARK::_UpdateSystem(QString SQL)
{
    ui->QSystem->setRowCount(0);
    QSqlQuery query;
    SQL = SQL + " limit " + QString::number((SystemCurPage-1) * MAX_LENGTH) + "," + QString::number(MAX_LENGTH);

    if(query.exec(SQL))
    {
        int ti = 0; //记录数据行数
        ui->QSystem->setRowCount(ti);
        if(query.numRowsAffected() <= 0)  //判断查询成功的数据行数
        {
            QMessageBox::information(this,"提示","记录为空");
        }
        while(query.next()) //当数据行数为0时，直接跳过循环，所以上面if不需要加return
        {
            ui->QSystem->setRowCount(ti + 1);

            QTableWidgetItem *tabledmItem0 = new QTableWidgetItem(QString::number(ti+1));
            tabledmItem0->setTextAlignment(Qt::AlignCenter);
            ui->QSystem->setItem(ti, 0, tabledmItem0);//序号

            QTableWidgetItem *tabledmItem1 = new QTableWidgetItem(QString(query.value(1).toByteArray().data()));
            tabledmItem1->setTextAlignment(Qt::AlignCenter);
            ui->QSystem->setItem(ti, 1, tabledmItem1);//用户名

            QTableWidgetItem *tabledmItem2 = new QTableWidgetItem(QString(query.value(2).toByteArray().data()));
            tabledmItem2->setTextAlignment(Qt::AlignCenter);
            ui->QSystem->setItem(ti, 2, tabledmItem2);//姓名

            QTableWidgetItem *tabledmItem3 = new QTableWidgetItem(QString(query.value(3).toByteArray().data()));
            tabledmItem3->setTextAlignment(Qt::AlignCenter);
            ui->QSystem->setItem(ti, 3, tabledmItem3);//操作类型

            QTableWidgetItem *tabledmItem4 = new QTableWidgetItem(QString(query.value(4).toDateTime().toString("yyyy-MM-dd hh:mm:ss")));
            tabledmItem4->setTextAlignment(Qt::AlignCenter);
            ui->QSystem->setItem(ti, 4, tabledmItem4);//时间
            ti++;
        }
    }
}

/********更新函数：报警日志数据********/
void COFFICEPARK:: _UpdateAlarmRecord(QString SQL)
{
    ui->QAlarmRecord->setRowCount(0);

    if(SQL.length() <= 0)
    {
        SQL = "SELECT a.* FROM alarm a ORDER BY a.Alarm_Stime ASC";
    }

    SQL = SQL + " limit " + QString::number((AlarmCurPage-1) * MAX_LENGTH) + "," + QString::number(MAX_LENGTH);
    QSqlQuery query;
    if(query.exec(SQL))
    {
        int ti = 0;
        ui->QAlarmRecord->setRowCount(ti);
        if(query.numRowsAffected() <= 0)
        {
            QMessageBox::information(this,"提示","记录为空");
        }
        while(query.next())
        {
            ui->QAlarmRecord->setRowCount(ti+1);

            QTableWidgetItem *tablemItem0 = new QTableWidgetItem(QString::number(ti+1));
            tablemItem0->setTextAlignment(Qt::AlignCenter);
            ui->QAlarmRecord->setItem(ti,0,tablemItem0);//序号

            QTableWidgetItem *tablemItem1 = new QTableWidgetItem(query.value(12).toString());
            tablemItem1->setTextAlignment(Qt::AlignCenter);
            ui->QAlarmRecord->setItem(ti,1,tablemItem1);//设备内部地址

            QTableWidgetItem *tablemItem2 = new QTableWidgetItem(query.value(2).toString());
            tablemItem2->setTextAlignment(Qt::AlignCenter);
            ui->QAlarmRecord->setItem(ti,2,tablemItem2);//IP地址

            QTableWidgetItem *tablemItem3 = new QTableWidgetItem(query.value(3).toString());
            tablemItem3->setTextAlignment(Qt::AlignCenter);
            ui->QAlarmRecord->setItem(ti,3,tablemItem3);//报警防区

            QTableWidgetItem *tablemItem4 = new QTableWidgetItem(query.value(4).toString());
            tablemItem4->setTextAlignment(Qt::AlignCenter);
            ui->QAlarmRecord->setItem(ti,4,tablemItem4);//报警类型

            QTableWidgetItem *tablemItem5 = new QTableWidgetItem(query.value(5).toDateTime().toString("yyyy-MM-dd hh:mm:ss"));
            tablemItem5->setTextAlignment(Qt::AlignCenter);
            ui->QAlarmRecord->setItem(ti,5,tablemItem5);//报警开始时间

            QTableWidgetItem *tablemItem6 = new QTableWidgetItem(query.value(7).toDateTime().toString("yyyy-MM-dd hh:mm:ss"));
            tablemItem6->setTextAlignment(Qt::AlignCenter);
            ui->QAlarmRecord->setItem(ti,6,tablemItem6);//报警处理时间

            QTableWidgetItem *tablemItem7 = new QTableWidgetItem(query.value(6).toDateTime().toString("yyyy-MM-dd hh:mm:ss"));
            tablemItem7->setTextAlignment(Qt::AlignCenter);
            ui->QAlarmRecord->setItem(ti,7,tablemItem7);//报警结束时间

            QTableWidgetItem *tablemItem8 = new QTableWidgetItem(query.value(8).toString());
            tablemItem8->setTextAlignment(Qt::AlignCenter);
            ui->QAlarmRecord->setItem(ti,8,tablemItem8);//是否误报

            QTableWidgetItem *tablemItem9 = new QTableWidgetItem(query.value(9).toString());
            tablemItem9->setTextAlignment(Qt::AlignCenter);
            ui->QAlarmRecord->setItem(ti,9,tablemItem9);//处理人

            QTableWidgetItem *tablemItem10 = new QTableWidgetItem(query.value(10).toString());
            tablemItem10->setTextAlignment(Qt::AlignCenter);
            ui->QAlarmRecord->setItem(ti,10,tablemItem10);//详情

            QTableWidgetItem *tablemItem11 = new QTableWidgetItem(query.value(11).toString());
            tablemItem11->setTextAlignment(Qt::AlignCenter);
            ui->QAlarmRecord->setItem(ti,11,tablemItem11);//设备类型

            ti++;
        }
    }
    else
    {
        QMessageBox::information(this, "提示", "查询失败，请检查数据库连接！");
    }
}

/********更新函数：设备日志数据********/
void COFFICEPARK::_UpdateDeviceRecord(QString SQL)
{
    QSqlQuery query;
    SQL = SQL + " limit " + QString::number((DeviceCurPage-1) * MAX_LENGTH) + "," + QString::number(MAX_LENGTH);
    qDebug()<<SQL;
    if(query.exec(SQL))
    {
        int ti=0;
        ui->QDeviceRecord->setRowCount(ti);
        if(query.numRowsAffected()<=0)
            QMessageBox::information(this,"提示","记录为空");
        while(query.next())
        {
            ui->QDeviceRecord->setRowCount(ti+1);

            QTableWidgetItem *tablemItem0 = new QTableWidgetItem(QString::number(ti+1));
            tablemItem0->setTextAlignment(Qt::AlignCenter);
            ui->QDeviceRecord->setItem(ti,0,tablemItem0);

            QTableWidgetItem *tablemItem1 = new QTableWidgetItem(QString(query.value(1).toString()));
            tablemItem1->setTextAlignment(Qt::AlignCenter);
            ui->QDeviceRecord->setItem(ti,1,tablemItem1);

            QTableWidgetItem *tablemItem2 = new QTableWidgetItem(QString(query.value(3).toString()));
            tablemItem2->setTextAlignment(Qt::AlignCenter);
            ui->QDeviceRecord->setItem(ti,2,tablemItem2);

            QTableWidgetItem *tablemItem3 = new QTableWidgetItem(QString(query.value(2).toString()));
            tablemItem3->setTextAlignment(Qt::AlignCenter);
            ui->QDeviceRecord->setItem(ti,3,tablemItem3);

            QTableWidgetItem *tablemItem4 = new QTableWidgetItem(QString(query.value(4).toDateTime().toString("yyyy-MM-dd hh:mm:ss")));
            tablemItem4->setTextAlignment(Qt::AlignCenter);
            ui->QDeviceRecord->setItem(ti,4,tablemItem4);

            ti++;
        }
    }
}

//重绘界面
void COFFICEPARK::resizeWidget()
{
    int Width  = ui->QpbtnShut->width();    //右上角关闭按钮
    int Heigth = ui->QpbtnShut->height();
    ui->QpbtnShut->move((this->width())- Width,0);
//    ui->QpbtnMax->move(this->width()-2*Width,0);
    ui->QpbtnMin->move(this->width()-3*Width,0);
    //ui->label->setSizeIncrement(this->width()-3*Width,Heigth);
    ui->Title_label->resize(this->width()-3*Width,Heigth);
}

/********信号函数：“设备”菜单*********/
void COFFICEPARK::on_QpbtnEquipment_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    emit SigEquipmentClick();
    ptbnNum = 1;
    SetAllButtenSheet(ptbnNum);

    m_nRecord = 4;
}

/********信号函数：“人事”菜单*********/
void COFFICEPARK::on_QpbtnPersonnel_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    emit SigPersonnelClick();
    ptbnNum = 2;
    SetAllButtenSheet(ptbnNum);
}

/********信号函数：“考勤”菜单*********/
void COFFICEPARK::on_QpbtnAttendance_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    emit SigAttendanceClick();
    ptbnNum = 3;
    SetAllButtenSheet(ptbnNum);
}

/********信号函数：“日志”菜单*********/
void COFFICEPARK::on_QpbtnSystem_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
//    ui->stackedWidget_2->setCurrentIndex(3);
    ptbnNum=5;
    SetAllButtenSheet(ptbnNum);
}

/********信号函数：“权限”菜单*********/
void COFFICEPARK::on_QpbtnRecord_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
    System->_UpdateToUser(m_sUser);
    ptbnNum = 4;
    SetAllButtenSheet(ptbnNum);
}

/********信号函数：“最小化”按钮*********/
void COFFICEPARK::on_QpbtnMin_clicked()
{
    setWindowState(Qt::WindowMinimized);    //设置窗口最小化
    //resizeWidget();
}

/********信号函数：“关闭”按钮*********/
void COFFICEPARK::on_QpbtnShut_clicked()
{
    switch( QMessageBox::question( this, tr("提示"),"\n  系统选项",tr("最小化到托盘"), tr("退出系统"),0, 1 ) )
    {
        case 0:
            this->hide();
            break;
        case 1:
            this->close();
            break;
        default:
            break;
    }
}

//右键“查找”功能界面
void COFFICEPARK::SlotFindRecord()
{
    CFind->_Update(m_nRecord);  //更新查找操作
    CFind->show();  //调用显示查找对话框界面
}

//右键“删除”功能界面
void COFFICEPARK::SlotDeleteRecord()
{
    CDelete->_Update(m_nRecord, m_sUser);    //日志类型：报警 or 设备 or 系统
    CDelete->show();    //显示删除界面
}

//udp端口打开失败
void COFFICEPARK::SlotUdpOpenFail()
{
    QMessageBox::warning(NULL,tr("提示"),tr("udp端口打开失败，请检查后重启"));
    CUdpServer->m_Mysql._InsertSystem(3,m_sUser.strUser,m_sUser.strName);
    emit Sigquit();
}

void COFFICEPARK::SlotPersonnel_Staff_Leave(int Id)
{
    CUdpServer->_StaffLeave(Id);
}

void COFFICEPARK::on_centralwidget_customContextMenuRequested(const QPoint &pos)
{
    CentralwidgetMenu->addAction(Logout);
    CentralwidgetMenu->exec(QCursor::pos());
}

//报警记录->右键
void COFFICEPARK::on_QAlarmRecord_customContextMenuRequested(const QPoint &pos)
{
    AlarmRecordMenu->addAction(DeleteRecordAction);
    AlarmRecordMenu->addAction(FindRecordAction);
    AlarmRecordMenu->addAction(DealAction);
    AlarmRecordMenu->addAction(NextPage);   //下一页
    AlarmRecordMenu->addAction(PrevPage);   //上一页
    AlarmRecordMenu->exec(QCursor::pos());
}

//设备日志->右键
void COFFICEPARK::on_QDeviceRecord_customContextMenuRequested(const QPoint &pos)
{
    SystemRecordMenu->addAction(DeleteRecordAction);
    SystemRecordMenu->addAction(FindRecordAction);
    SystemRecordMenu->addAction(NextPage);  //下一页
    SystemRecordMenu->addAction(PrevPage);  //上一页
    SystemRecordMenu->exec(QCursor::pos());
}

//系统日志->右键
void COFFICEPARK::on_QSystem_customContextMenuRequested(const QPoint &pos)
{
    DeviceRecordMenu->addAction(DeleteRecordAction);
    DeviceRecordMenu->addAction(FindRecordAction);
    DeviceRecordMenu->addAction(NextPage);  //下一页
    DeviceRecordMenu->addAction(PrevPage);  //上一页
    DeviceRecordMenu->exec(QCursor::pos());
}

void COFFICEPARK::SlotRecvLogOut()
{
    Personnel->_UpdateToStaff();
}

//报警记录右键“处理”功能界面
void COFFICEPARK::SlotDeal()
{
    int ti = ui->QAlarmRecord->currentRow();    //获取当前行
    if(ti >= 0 )
    {
        //获取ti行，第6列（从0开始，即“确认时间”列）单元格中的内容
        QString strDtime = ui->QAlarmRecord->item(ti, 6)->text();
        if(strDtime == NULL)
        {
            memset(sAlarmDevice.gcIntraAddr,0,20);  //初始化“警报设备”结构体中的属性
            memset(sAlarmDevice.gcPhone1,0,20);
            memset(sAlarmDevice.gcIpAddr,0,20);
            memset(sAlarmDevice.gcMacAddr,0,20);
            memset(sAlarmDevice.gcStime,0,20);
            memset(sAlarmDevice.gcEtime,0,20);
            memset(sAlarmDevice.gcType,0,20);
            //取ti行，第5列（从0开始，即“报警时间”列）单元格中的内容
            QString strStime = ui->QAlarmRecord->item(ti, 5)->text();
            QByteArray byte;
            byte = strStime.toUtf8();
            memcpy(sAlarmDevice.gcStime,byte.data(),byte.size());
            if(FindItemAlarm(sAlarmDevice,CUdpServer->m_AlarmItem))//是否实时的报警
            {
                //删除报警设备
                CUdpServer->m_AlarmItem = DeleteItemAlarm(&sAlarmDevice,&(CUdpServer->m_AlarmItem));
                Equipment->_UpdateAlarm(CUdpServer->m_AlarmItem);   //更新报警数据
                //查找链表
                int isNULL = AddrFindAlarmItem(sAlarmDevice,CUdpServer->m_AlarmItem);
                //是否更新实时表
                if(isNULL == 1)
                {
                    CUdpServer->m_DeviceItem = AlarmUpdateDeviceItem(sAlarmDevice,CUdpServer->m_DeviceItem);
                    Equipment->_UpdateDevice(CUdpServer->m_DeviceItem);
                }
            }

            //获取当前行的数据到链表
            SetDealInformation();
        }
        else
        {
            QMessageBox::information(this,tr("提示"),tr("已处理过的记录"));
            return;
        }
    }
    else
    {
        QMessageBox::information(this,tr("提示"),tr("请选中目标"));
    }
}

/********信号函数：“帮助”菜单*********/
void COFFICEPARK::on_QpbtnHelp_clicked()
{
    ui->stackedWidget->setCurrentIndex(6);
    ptbnNum=7;
    SetAllButtenSheet(ptbnNum);
}

/*****槽函数：“帮助”菜单->“上传照片”按钮*****/
void COFFICEPARK::on_QIcon_clicked()
{
    QString Path = QFileDialog::getOpenFileName();
    ui->Photo_lineEdit->setStyleSheet("border-image:url("+Path+")");
}

/*********模块选择按键动态效果切换*********/
void COFFICEPARK::SetAllButtenSheet(int m_ptbnNum)
{
    switch(m_ptbnNum)
    {
        case 1:
        {
            ui->QpbtnEquipment->setStyleSheet("border-image:url(:/images/OfficeParks49.png)");
            break;
        }
        case 2:
        {
            ui->QpbtnPersonnel->setStyleSheet("border-image:url(:/images/OfficeParks50.png)");
            break;
        }
        case 3:
        {
            ui->QpbtnAttendance->setStyleSheet("border-image:url(:/images/OfficeParks52.png)");
            break;
        }
        case 4:
        {
            ui->QpbtnRecord->setStyleSheet("border-image:url(:/images/OfficeParks53.png)");
            break;
        }
        case 5:
        {
            ui->QpbtnSystem->setStyleSheet("border-image:url(:/images/OfficeParks54.png)");
            break;
        }
        case 6:
        {
            ui->QpbtnAbout->setStyleSheet("border-image:url(:/images/OfficeParks55.png)");
            break;
        }
        case 7:
        {
            ui->QpbtnHelp->setStyleSheet("border-image:url(:/images/OfficeParks56.png)");
            break;
        }
        default:
            break;

    }
    if(m_ptbnNum!=1)
    {
        ui->QpbtnEquipment->setStyleSheet("border-image:url(:/images/OfficeParks6.png)");
        ui->QpbtnEquipment->setStyleSheet(":hover{border-image:url(:/images/OfficeParks42.png)}");
    }

    if(m_ptbnNum!=2)
    {
        ui->QpbtnPersonnel->setStyleSheet("border-image:url(:/images/OfficeParks7.png)");
        ui->QpbtnPersonnel->setStyleSheet(":hover{border-image:url(:/images/OfficeParks43.png)}");
    }

    if(m_ptbnNum!=3)
    {
        ui->QpbtnAttendance->setStyleSheet("border-image:url(:/images/OfficeParks8.png)");
        ui->QpbtnAttendance->setStyleSheet(":hover{border-image:url(:/images/OfficeParks44.png)}");
    }
    if(m_ptbnNum!=4)
    {
        ui->QpbtnRecord->setStyleSheet("border-image:url(:/images/OfficeParks9.png)");
        ui->QpbtnRecord->setStyleSheet(":hover{border-image:url(:/images/OfficeParks45.png)}");
        ui->LogTable->setCurrentItem(ui->LogTable->item(10,0));
    }
    if(m_ptbnNum!=5)
    {
        ui->QpbtnSystem->setStyleSheet("border-image:url(:/images/OfficeParks10.png)");
        ui->QpbtnSystem->setStyleSheet(":hover{border-image:url(:/images/OfficeParks46.png)}");
        ui->QType->setCurrentItem(ui->QType->item(10,0));
    }
    if(m_ptbnNum!=6)
    {
        ui->QpbtnAbout->setStyleSheet("border-image:url(:/images/OfficeParks12.png)");
        ui->QpbtnAbout->setStyleSheet(":hover{border-image:url(:/images/OfficeParks47.png)}");
    }
    if(m_ptbnNum!=7)
    {
        ui->QpbtnHelp->setStyleSheet("border-image:url(:/images/OfficeParks11.png)");
        ui->QpbtnHelp->setStyleSheet(":hover{border-image:url(:/images/OfficeParks48.png)}");

    }
    ui->LogTable->setCurrentItem(ui->LogTable->item(10,0));
    ui->QType->setCurrentItem(ui->QType->item(10,0));
    emit SigEquipmentFocusOut();
    emit SigPersonnelFocusOut();
    emit SigAttendanceFocusOut();
}

/*********信号槽函数：日志查找（nIndex:查找条件的类型，nTime:是否进行时间限制）*************/
void COFFICEPARK::SlotFind(int nIndex, int nTime, QString strFind, QString strStime, QString strEtime)
{
    if(strFind.length() > 0)
    {
        if(nTime == 1)
        {
            switch(nIndex)
            {
                case 0:
                {
                    if(m_nRecord == 1)
                        FindSQL = "SELECT * FROM alarm a WHERE a.Alarm_Intra_Addr = '"+strFind+"' and a.Alarm_Stime >= '"+strStime+"'"
                        " and a.Alarm_Stime <= '"+strEtime+"' ORDER BY a.Alarm_Stime ASC";
                    if(m_nRecord == 2)
                        FindSQL = "SELECT * FROM device_record a WHERE a.Device_Record_Addr = '"+strFind+"' and a.Device_Record_Time >= '"+strStime+"'"
                        " and a.Device_Record_Time <= '"+strEtime+"' ORDER BY a.Device_Record_Time ASC";
                    if(m_nRecord == 3)
                        FindSQL = "SELECT * FROM system_record a WHERE a.System_Record_Name = '"+strFind+"' and a.System_Record_Time >= '"+strStime+"'"
                        " and a.System_Record_Time <= '"+strEtime+"' ORDER BY a.System_Record_Time ASC";
                    break;
                }
                case 1:
                {
                    if(m_nRecord == 1)
                        FindSQL = "SELECT * FROM alarm a WHERE a.Alarm_Ip_Addr = '"+strFind+"' and a.Alarm_Stime >= '"+strStime+"'"
                        " and a.Alarm_Stime <= '"+strEtime+"' ORDER BY a.Alarm_Stime ASC";
                    if(m_nRecord == 2)
                        FindSQL = "SELECT * FROM device_record a WHERE a.Device_Record_Ip = '"+strFind+"' and a.Device_Record_Time >= '"+strStime+"'"
                        " and a.Device_Record_Time <= '"+strEtime+"' ORDER BY a.Device_Record_Time ASC";
                    if(m_nRecord == 3)
                        FindSQL = "SELECT * FROM system_record a WHERE a.System_Record_Human = '"+strFind+"' and a.System_Record_Time >= '"+strStime+"'"
                        " and a.System_Record_Time <= '"+strEtime+"' ORDER BY a.System_Record_Time ASC";
                    break;
                }
                case 2:
                {
                    if(m_nRecord == 1)
                        FindSQL = "SELECT * FROM alarm a WHERE isnull(a.Alarm_Dtime) ORDER BY a.Alarm_Stime ASC";
                    if(m_nRecord == 2)
                        FindSQL = "SELECT * FROM device_record a WHERE a.Device_Record_Type = '"+strFind+"' and a.Device_Record_Time >= '"+strStime+"'"
                        " and a.Device_Record_Time <= '"+strEtime+"' ORDER BY a.Device_Record_Time ASC";
                    if(m_nRecord == 3)
                        FindSQL = "SELECT * FROM system_record a WHERE a.System_Record_Type = '"+strFind+"' and a.System_Record_Time >= '"+strStime+"'"
                        " and a.System_Record_Time <= '"+strEtime+"' ORDER BY a.System_Record_Time ASC";
                    break;
                }
                default:
                    break;
            }
        }
        else if(nTime == 0)
        {
            switch(nIndex)
            {
                case 0:
                {
                    if(m_nRecord == 1)
                        FindSQL = "SELECT * FROM alarm a WHERE a.Alarm_Intra_Addr = '"+strFind+"' ORDER BY a.Alarm_Stime ASC";
                    if(m_nRecord == 2)
                        FindSQL = "SELECT * FROM device_record a WHERE a.Device_Record_Addr = '"+strFind+"' ORDER BY a.Device_Record_Time ASC";

                    if(m_nRecord == 3)
                        FindSQL = "SELECT * FROM system_record a WHERE a.System_Record_Name = '"+strFind+"' ORDER BY a.System_Record_Time ASC";
                    break;
                }
                case 1:
                {
                    if(m_nRecord == 1)
                        FindSQL = "SELECT * FROM alarm a WHERE a.Alarm_Ip_Addr = '"+strFind+"' ORDER BY a.Alarm_Stime ASC";
                    if(m_nRecord == 2)
                        FindSQL = "SELECT * FROM device_record a WHERE a.Device_Record_Ip = '"+strFind+"' ORDER BY a.Device_Record_Time ASC";
                    if(m_nRecord == 3)
                        FindSQL = "SELECT * FROM system_record a WHERE a.System_Record_Human = '"+strFind+"' ORDER BY a.System_Record_Time ASC";
                    break;
                }
                case 2:
                {
                    if(m_nRecord == 1)
                        FindSQL = "SELECT * FROM alarm a WHERE isnull(a.Alarm_Dtime) ORDER BY a.Alarm_Stime ASC";
                    if(m_nRecord == 2)
                        FindSQL = "SELECT * FROM device_record a WHERE a.Device_Record_Type = '"+strFind+"' ORDER BY a.Device_Record_Time ASC";
                    if(m_nRecord == 3)
                        FindSQL = "SELECT * FROM system_record a WHERE a.System_Record_Type = '"+strFind+"' ORDER BY a.System_Record_Time ASC";
                    break;
                }
                default:
                    break;
            }
        }
    }
    else
    {
        if(nTime == 1)
        {
            if(m_nRecord == 1)          //报警日志
            {
                switch(nIndex)
                {
                    case 0:
                    case 1:
                    {
                        FindSQL = "SELECT * FROM alarm a WHERE a.Alarm_Stime >= '"+strStime+"'"
                        " and a.Alarm_Stime <= '"+strEtime+"' ORDER BY a.Alarm_Stime ASC";
                        break;
                    }
                    case 2:
                    {
                        FindSQL = "SELECT * FROM alarm a WHERE isnull(a.Alarm_Dtime) ORDER BY a.Alarm_Stime ASC";
                        break;
                    }
                    default:
                        break;
                }
            }

            if(m_nRecord == 2)          //设备日志
            {
                FindSQL = "SELECT * FROM device_record a WHERE a.Device_Record_Time >= '"+strStime+"'"
                        " and a.Device_Record_Time <= '"+strEtime+"' ORDER BY a.Device_Record_Time ASC";
            }

            if(m_nRecord == 3)          //系统日志
            {
                FindSQL = "SELECT * FROM system_record a WHERE a.System_Record_Time >= '"+strStime+"'"
                        " and a.System_Record_Time <= '"+strEtime+"' ORDER BY a.System_Record_Time ASC";
            }
        }
        else if(nTime == 0)
        {
            if(m_nRecord == 1)
            {
                switch(nIndex)
                {
                    case 0:
                    case 1:
                    {
                        FindSQL = "SELECT a.* FROM alarm a ORDER BY a.Alarm_Stime ASC";
                        break;
                    }
                    case 2:
                    {
                        FindSQL = "SELECT * FROM alarm a WHERE isnull(a.Alarm_Dtime) ORDER BY a.Alarm_Stime ASC";
                        break;
                    }
                    default:
                        break;
                }
            }

            if(m_nRecord == 2)
            {
                FindSQL = "SELECT * FROM device_record a ORDER BY a.Device_Record_Time ASC";
            }

            if(m_nRecord == 3)
            {
                FindSQL = "SELECT * FROM system_record a ORDER BY a.System_Record_Time ASC";
            }
        }
    }
    switch(m_nRecord)
    {
        case 1: _UpdateAlarmRecord(FindSQL);
            break;
        case 2: _UpdateDeviceRecord(FindSQL);
            break;
        case 3: _UpdateSystem(FindSQL);
            break;
        default:
            break;
    }
}

void COFFICEPARK::SetDealInformation()
{
    SAlarmDeal AlarmDeal;
    int nRow = ui->QAlarmRecord->currentRow();
    if(nRow >= 0)
    {
        AlarmDeal.QAlarmIPAdd       = ui->QAlarmRecord->item(nRow,2)->text();
        AlarmDeal.QAlarmAdd         = ui->QAlarmRecord->item(nRow,1)->text();
        AlarmDeal.QAlarmNo          = ui->QAlarmRecord->item(nRow,0)->text();
        AlarmDeal.QAlarmDeviceType  = ui->QAlarmRecord->item(nRow,11)->text();
        AlarmDeal.QAlarmStime       = ui->QAlarmRecord->item(nRow,5)->text();
        AlarmDeal.QAlarmEtime       = ui->QAlarmRecord->item(nRow,7)->text();

        Deal = new deal();
//        Deal->setAttribute(Qt::WA_DeleteOnClose);
        Deal->setAttribute(Qt::WA_ShowModal);
        connect(Deal,SIGNAL(SigDealDate(QString,QString,QString)),this,SLOT(SlotDealInformation(QString,QString,QString)));

        Deal->_Update(AlarmDeal);
        Deal->show();
    }
    else
    {
        QMessageBox::information(this,"提示","请选择要处理的对象");
        return;
    }
}

void COFFICEPARK::SlotDealInformation(QString whetherMesIfo, QString dealHuman, QString Messages)
{
    //数据库更新
    if(CUdpServer->m_Mysql._UpdateDealAlarm(sAlarmDevice,whetherMesIfo,dealHuman,Messages))
    {
        QMessageBox::information(this,"提示","处理成功",QMessageBox::Yes);
        Deal->close();
    }
    _UpdateAlarmRecord(FindSQL);
}

/*******信号槽函数：删除日志记录********/
void COFFICEPARK::SlotDeleteSuc()
{
    switch(m_nRecord)
    {
        case 1:     //报警记录
        {
            FindSQL = "SELECT * FROM alarm ORDER BY Alarm_Stime DESC";
            _UpdateAlarmRecord(FindSQL);
            break;
        }
        case 2:     //设备记录
        {
            FindSQL = "SELECT * FROM device_record ORDER BY Device_Record_Time DESC";
            _UpdateDeviceRecord(FindSQL);
            break;
        }
        case 3:     //系统记录
        {
            FindSQL = "SELECT * FROM system_record ORDER BY System_Record_Time DESC";
            _UpdateSystem(FindSQL);
            break;
        }
        default:
            break;
    }
}

/*******信号槽函数：注销********/
void COFFICEPARK::SlotLogOut()
{
    emit SigLoading();  //调用loading.cpp中的SlotLoading()方法
}

void COFFICEPARK::SlotSeleteUser(QString user)
{
    QString SQL="SELECT * from `user` a WHERE a.User_User = '"+user+"'";
    QSqlQuery query;
    query.exec(SQL);
    query.next();
    ui->User_label->clear();
    ui->User_label->setText(user);
}

void COFFICEPARK::on_QpbtnAlarm_clicked()
{
    ui->stackedWidget_2->setCurrentIndex(0);
    QString SQL = "SELECT * FROM alarm ORDER BY Alarm_Id DESC";
    QString CountSQL = "SELECT count(*) FROM alarm ORDER BY Alarm_Id DESC";
    QSqlQuery CountQuery;
    if(CountQuery.exec(CountSQL))
    {
        while(CountQuery.next())
        {
            m_nCount = CountQuery.value(0).toInt();
        }
    }
    _UpdateAlarmRecord(SQL);
    m_nRecord=1;
    ui->QAlarmRecord->setCurrentItem(ui->QAlarmRecord->item(-1,-1));
}

void COFFICEPARK::on_QpbtnDevice_clicked()
{
    ui->stackedWidget_2->setCurrentIndex(1);
    QString SQL = "SELECT * FROM device_record ORDER BY Device_Record_Time DESC";
    QString CountSQL = "SELECT count(*) FROM device_record ORDER BY Device_Record_Time DESC";
    QSqlQuery CountQuery;
    if(CountQuery.exec(CountSQL))
    {
        while(CountQuery.next())
        {
            m_nCount = CountQuery.value(0).toInt();
        }
    }
    _UpdateDeviceRecord(SQL);
    m_nRecord=2;
    ui->QDeviceRecord->setCurrentItem(ui->QDeviceRecord->item(-1,-1));
}

void COFFICEPARK::on_QpbtnSystem_2_clicked()
{
    ui->stackedWidget_2->setCurrentIndex(2);
    QDateTime dateTime=QDateTime::currentDateTime();
    dateTime = dateTime.addDays(-15);
    QString stime = dateTime.toString("yyyy-MM-dd ");
    QString SQL = "SELECT *FROM system_record WHERE  System_Record_Time > '"+stime+"' ORDER BY System_Record_Time DESC";
    QString CountSQL = "SELECT count(*) FROM system_record WHERE System_Record_Time > '"+stime+"' ORDER BY System_Record_Time DESC";
    QSqlQuery CountQuery;
    if(CountQuery.exec(CountSQL))
    {
        while(CountQuery.next())
        {
            m_nCount = CountQuery.value(0).toInt();
        }
    }
    _UpdateSystem(SQL);
    m_nRecord=3;
    ui->QSystem->setCurrentItem(ui->QSystem->item(-1,-1));
}

/******槽函数：下一页*******/
void COFFICEPARK::SlotNextPage()
{
    switch(m_nRecord)
    {
        case 1:
        {
            int alarmAllPage = (m_nCount % MAX_LENGTH)?(m_nCount/MAX_LENGTH + 1):(m_nCount/MAX_LENGTH);
            if(AlarmCurPage == alarmAllPage)
            {
                QMessageBox::information(this, tr(""), tr("已经是最后一页了"));
            }
            else
            {
                AlarmCurPage++;
                FindSQL = "SELECT * FROM alarm ORDER BY Alarm_Time DESC";
                _UpdateAlarmRecord(FindSQL);
            }
            break;
        }
        case 2:
        {
            int DeviceAllPage = (m_nCount % MAX_LENGTH)?(m_nCount/MAX_LENGTH + 1):(m_nCount/MAX_LENGTH);
            if(DeviceCurPage == DeviceAllPage)
            {
                QMessageBox::information(this, tr(""), tr("已经是最后一页了"));
            }
            else
            {
                DeviceCurPage++;
                FindSQL = "SELECT * FROM device_record ORDER BY Device_Record_Time DESC";
                _UpdateDeviceRecord(FindSQL);
            }
            break;
        }
        case 3:
        {
            int SystemAllPage = (m_nCount % MAX_LENGTH)?(m_nCount/MAX_LENGTH + 1):(m_nCount/MAX_LENGTH);
            if(SystemCurPage == SystemAllPage)
            {
                QMessageBox::information(this, tr(""), tr("已经是最后一页了"));
            }
            else
            {
                SystemCurPage++;
                FindSQL = "SELECT * FROM system_record ORDER BY System_Record_Time DESC";
                _UpdateSystem(FindSQL);
            }
            break;
        }
    }
}

/******槽函数：上一页*******/
void COFFICEPARK::SlotPrevPage()
{
    switch(m_nRecord)
    {
        case 1:
            if(AlarmCurPage == 1)
            {
                QMessageBox::information(this, tr(""), tr("已经是第一页了"));
            }
            else
            {
                AlarmCurPage--;
                FindSQL = "SELECT * FROM alarm ORDER BY Alarm_Time DESC";
                _UpdateAlarmRecord(FindSQL);
            }
            break;
        case 2:
            if(DeviceCurPage == 1)
            {
                QMessageBox::information(this, tr(""), tr("已经是第一页了"));
            }
            else
            {
                DeviceCurPage--;
                FindSQL = "SELECT * FROM device_record ORDER BY Device_Record_Time DESC";
                _UpdateDeviceRecord(FindSQL);
            }
            break;
        case 3:
            if(SystemCurPage == 1)
            {
                QMessageBox::information(this, tr(""), tr("已经是第一页了"));
            }
            else
            {
                SystemCurPage--;
                FindSQL = "SELECT * FROM system_record ORDER BY System_Record_Time DESC";
                _UpdateSystem(FindSQL);
            }
            break;
    }
}
