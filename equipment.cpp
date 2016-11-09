#include "equipment.h"
#include "ui_equipment.h"

equipment::equipment(NodeDevice *deviceItem, NodeAlarm *AlarmItem, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::equipment)
{
    ui->setupUi(this);
    m_Device = deviceItem;
    m_Alarm  = AlarmItem;
    deviceType = 0;

    _InitTree();        //初始化函数：设备树
    _InitDevice();      //初始化函数：设备数据
    _InitAlarm();       //初始化函数：报警数据
    _UpdateDevice(m_Device);

    DeviceMenu = new QMenu(ui->QDevice);
    DeleteAction = new QAction(tr("删除"),this);
    ResetAction = new QAction(tr("重启"),this);
    PingAction = new QAction(tr("网络测试"),this);
    RegisterAction = new QAction(tr("注册模式"),this);
    InitPassAction = new QAction(tr("初始化密码"),this);
    DragSmartControlAction = new QAction(tr("智能家居"),this);

    connect(DragSmartControlAction, SIGNAL(triggered()), this, SLOT(SlotDragSmartControl()));
    connect(ResetAction, SIGNAL(triggered()), this, SLOT(SlotReset()));
    connect(DeleteAction, SIGNAL(triggered()), this, SLOT(SlotDelete()));
    connect(PingAction, SIGNAL(triggered()), this, SLOT(SlotPing()));
    connect(InitPassAction, SIGNAL(triggered()), this, SLOT(SlotInitPass()));
    connect(RegisterAction, SIGNAL(triggered()), this, SLOT(SlotRegister()));
    ui->QDevice->setContextMenuPolicy(Qt::CustomContextMenu);

    AlarmMenu = new QMenu(ui->QAlarm);
    DealAction = new QAction(tr("处理"),this);
    connect(DealAction, SIGNAL(triggered()), this, SLOT(SlotDeal()));
    ui->QAlarm->setContextMenuPolicy(Qt::CustomContextMenu);
}

equipment::~equipment()
{
    delete ui;
}

/******初始化函数：设备树**********/
void equipment::_InitTree()
{
    ui->QType->setColumnCount(1);
    ui->QType->setShowGrid(false);//显示表格线
    QStringList headers;
    headers<<"序号";
    ui->QType->horizontalHeader()->hide();
    ui->QType->verticalHeader()->hide();//行头不显示
    ui->QType->horizontalHeader()->setClickable(false);//行头不可选
    ui->QType->setHorizontalHeaderLabels(headers);
    ui->QType->horizontalHeader()->setHighlightSections(false);//表头坍塌
    ui->QType->resizeColumnsToContents();
    ui->QType->setFont(QFont("Helvetica"));//字体
    ui->QType->setEditTriggers(QAbstractItemView::NoEditTriggers);//禁止编辑
    ui->QType->setSelectionBehavior(QAbstractItemView::SelectRows);  //整行选中的方式：一行
    ui->QType->setColumnWidth(0,160);

    for(int i=0;i<ui->QType->rowCount();i++)
        ui->QType->setRowHeight(i,44);
}

void equipment::on_QType_clicked(const QModelIndex &index)
{
    switch(index.row())
    {
        case 0:     //所有
            deviceType = 0;
            break;
        case 1:     //行政机
            deviceType = 1;
            break;
        case 2:     //访客机
            deviceType = 2;
            break;
        case 3:     //中心机
            deviceType = 3;
            break;
        default:
            break;
    }
    ui->stackedWidget->setCurrentIndex(1);
    _UpdateDevice(m_Device);
}

/******初始化函数：设备数据**********/
void equipment::_InitDevice()
{
    ui->QDevice->setColumnCount(7);
    ui->QDevice->setShowGrid(true);//显示表格线
    QStringList headers;
    headers<<"序号"<<"地址"<<"网卡地址"<<"IP地址"<<"连接状态"<<"布防状态"<<"设备类型";
    ui->QDevice->verticalHeader()->hide();//行头不显示
    ui->QDevice->horizontalHeader()->setClickable(false);//行头不可选
    ui->QDevice->setHorizontalHeaderLabels(headers);
    ui->QDevice->horizontalHeader()->setHighlightSections(false);//表头坍塌
    ui->QDevice->resizeColumnsToContents();
    ui->QDevice->setFont(QFont("Helvetica"));//字体
    ui->QDevice->setEditTriggers(QAbstractItemView::NoEditTriggers);//禁止编辑
    ui->QDevice->setSelectionBehavior(QAbstractItemView::SelectRows);  //整行选中的方式：一行
    ui->QDevice->setColumnWidth(0,69);
    ui->QDevice->setColumnWidth(1,250);
    ui->QDevice->setColumnWidth(2,130);
    ui->QDevice->setColumnWidth(3,120);
    ui->QDevice->setColumnWidth(4,80);
    ui->QDevice->setColumnWidth(5,80);
    ui->QDevice->setColumnWidth(6,80);
}

/******更新函数：设备数据**********/
void equipment::_UpdateDevice(NodeDevice *ItemDevice)
{
    int ti = 0;
    if(!ItemDevice->next)       //链表为空则返回
    {
        ui->QDevice->setRowCount(ti);
        return;
    }
    QString gcType, gcType2, gcType3;
    switch(deviceType)
    {
        case 0:
            gcType  = "行政机";
            gcType2 = "访客机";
            gcType3 = "中心机";
            break;
        case 1:
            gcType  = "行政机";
            gcType2 = "";
            gcType3 = "";
            break;
        case 2:
            gcType  = "";
            gcType2 = "访客机";
            gcType3 = "";
            break;
        case 3:
            gcType  = "";
            gcType2 = "";
            gcType3 = "中心机";
            break;
        default:
            break;
    }
    ui->QDevice->setRowCount(ti);
    while(ItemDevice->next != NULL)
    {
        if(gcType == ItemDevice->data.gcType || gcType2 == ItemDevice->data.gcType || gcType3 == ItemDevice->data.gcType)
        {
            ui->QDevice->setRowCount(ti + 1);
            ItemDevice->data.nId = ti;          //改变设备在链表中的ID

            QTableWidgetItem *tabledmItem0 = new QTableWidgetItem(QString::number(ti+1));
            tabledmItem0->setTextAlignment(Qt::AlignCenter);
            ui->QDevice->setItem(ti, 0, tabledmItem0);

            QTableWidgetItem *tabledmItem1 = new QTableWidgetItem(QString(ItemDevice->data.gcAddr));
            tabledmItem1->setTextAlignment(Qt::AlignCenter);
            ui->QDevice->setItem(ti, 1, tabledmItem1);

            QTableWidgetItem *tabledmItem2 = new QTableWidgetItem(QString(ItemDevice->data.gcMacAddr));
            tabledmItem2->setTextAlignment(Qt::AlignCenter);
            ui->QDevice->setItem(ti, 2, tabledmItem2);

            QTableWidgetItem *tabledmItem3 = new QTableWidgetItem(QString(ItemDevice->data.gcIpAddr));
            tabledmItem3->setTextAlignment(Qt::AlignCenter);
            ui->QDevice->setItem(ti, 3, tabledmItem3);

            if(ItemDevice->data.nState <= 0)    //连接状态
            {
                QTableWidgetItem *tabledmItem4 = new QTableWidgetItem("未连接");
                tabledmItem4->setTextAlignment(Qt::AlignCenter);
                ui->QDevice->setItem(ti, 4, tabledmItem4);
            }
            else
            {
                QTableWidgetItem *tabledmItem4 = new QTableWidgetItem("已连接");
                tabledmItem4->setTextAlignment(Qt::AlignCenter);
                ui->QDevice->setItem(ti, 4, tabledmItem4);
            }

            if(ItemDevice->data.nFenceState == 0)
            {
                QTableWidgetItem *tabledmItem5 = new QTableWidgetItem("撤防");
                tabledmItem5->setTextAlignment(Qt::AlignCenter);
                ui->QDevice->setItem(ti, 5, tabledmItem5);
            }
            else
            {
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

            if(ItemDevice->data.nAlarmState == 0)   //正常，无报警状态
            {
                for(int j = 0;j < 7;j++)
                {
                    QTableWidgetItem *item = ui->QDevice->item(ti, j);
                    item->setBackgroundColor(QColor(0,125,0,0));    //设置背景颜色：淡绿色
                }
            }
            else            //报警状态
            {
                for(int j = 0;j < 7;j++)
                {
                    QTableWidgetItem *item = ui->QDevice->item(ti, j);
                    item->setBackgroundColor(QColor(255,0,0));      //设置背景颜色：红色
                }
            }

            if(ItemDevice->data.nState <= 0)        //未连接
            {
                for(int j = 0;j < 7;j++)
                {
                    QTableWidgetItem *item = ui->QDevice->item(ti, j);
                    item->setTextColor(QColor(0,0,0));      //设置文本颜色：黑色
                }
            }
            else        //已连接
            {
                for(int j = 0;j < 7;j++)
                {
                    QTableWidgetItem *item = ui->QDevice->item(ti, j);
                    item->setTextColor(Qt::blue);
                }
            }
            ti++;
        }
        ItemDevice = ItemDevice->next;
    }
}

//网络测试
void equipment::SlotPing()
{
    QString strIp;
    int nRow = ui->QDevice->currentRow();
    if(nRow >= 0)
    {
        strIp = ui->QDevice->item(nRow, 3)->text();
        QStringList args;
        QString str = "ping " + strIp;
        args << "/K" << str;    //？？？
        QProcess::startDetached("cmd.exe",args);
    }
    else
    {
        QMessageBox::information(this,tr("提示"),tr("请选中目标"));
    }
}

//初始化密码
void equipment::SlotInitPass()
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
                emit SigUdpSendPass(strAddr, strMac, strIp);
                break;
            default :
                break;
        }
    }
    else
    {
        QMessageBox::information(this,tr("提示"),tr("请选中目标"));
    }
}

//接收到初始化密码的回复
void equipment::SlotRecvInitPass(QString strAddr,int nFlag)
{
    if(nFlag == 1)
    {
        strAddr = strAddr + "初始化工程密码成功";
    }
    else
    {
        strAddr = strAddr + "初始化开门密码成功";
    }
    QMessageBox::information(this,tr("提示"),strAddr);
}

//删除
void equipment::SlotDelete()
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
                if(mysql._DeleteDevice(strMac, strIp, strAddr, strType))
                {
                    QByteArray byte;
                    byte = strMac.toAscii();
                    memcpy(sDevice.gcMacAddr,byte.data(),byte.size());
                    m_Device = DeleteItemDevice(sDevice, &m_Device);
                    _UpdateDevice(m_Device);
                    QMessageBox::information(this,tr("提示"),tr("删除成功"));
                }
            }
                break;
            case 1:
                break;
            default :
                break;
        }
    }
    else
    {
        QMessageBox::information(this,tr("提示"),tr("请选中目标"));
    }
}

void equipment::on_QDevice_customContextMenuRequested(const QPoint &pos)
{
    DeviceMenu->addAction(DeleteAction);
    DeviceMenu->addAction(ResetAction);
    DeviceMenu->addAction(PingAction);
    DeviceMenu->addAction(RegisterAction);
//    DeviceMenu->addAction(SmartControlAction);
    DeviceMenu->addAction(InitPassAction);
    DeviceMenu->addAction(DragSmartControlAction);  //智能家居
    DeviceMenu->exec(QCursor::pos());
}

//重启
void equipment::SlotReset()
{
    int i = ui->QDevice->currentRow();
    if(i >= 0)
    {
        QString strIp = ui->QDevice->item(i, 3)->text();
        QString strAddr = ui->QDevice->item(i, 1)->text();
        QString strType = ui->QDevice->item(i, 6)->text();
        emit SigUdpReset(strIp, strAddr, strType);
    }
    else
    {
        QMessageBox::information(this,tr("提示"),tr("请选中目标"));
    }
}

//注册模式
void equipment::SlotRegister()
{
    int nRow = ui->QDevice->currentRow();
    if(nRow >= 0)
    {
        QString strIP = ui->QDevice->item(nRow,3)->text();
        QString strType = ui->QDevice->item(nRow,6)->text();
        qDebug()<<strIP<<strType;
        if(strType == "访客机" || strType == "访客机")//单元门口机 or 二次门口机
        {
            QString strExplainAddr = ui->QDevice->item(nRow,1)->text();
            QString strAdd = _AddrExplainToAddr(strExplainAddr,strType);
            emit SigUdpRegister(strAdd, strIP);
        }
        else    //只有门口机才有注册模式
        {
            QMessageBox::information(this,tr("提示"),tr("请选择访客机"));
        }
    }
    else
    {
        QMessageBox::information(this, tr("提示"), tr("请选择一行数据"));
    }
}

/******信号槽函数：实时更新设备**********/
//void equipment::SlotInitDevice(NodeDevice *Device)
void equipment::SlotInitDevice()
{
//    m_Device = Device;
    _UpdateDevice(m_Device);
}

/******信号槽函数：实时更新设备状态**********/
void equipment::SlotUpdateDevice(QVariant dataVar)
{
    int count = ui->QDevice->rowCount();
    if(count <= 0)
    {
        qDebug()<<"返回1";
        return;
    }

    SDevice data;
    data = dataVar.value<SDevice>();

    int ti = 0;
    for(int i = 0; i < count; i++)
    {
        if(ui->QDevice->item(i, 3)->text() == data.gcIpAddr)        //IP地址唯一
        {
            ti = i;
//            qDebug()<<"********"<<data.nId<<i;
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
            }
            else
            {
                QTableWidgetItem *tabledmItem4 = new QTableWidgetItem("已连接");
                tabledmItem4->setTextAlignment(Qt::AlignCenter);
                ui->QDevice->setItem(ti, 4, tabledmItem4);
            }

            if(data.nFenceState == 0)
            {
                QTableWidgetItem *tabledmItem5 = new QTableWidgetItem("撤防");
                tabledmItem5->setTextAlignment(Qt::AlignCenter);
                ui->QDevice->setItem(ti, 5, tabledmItem5);
            }
            else
            {
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

            if(data.nState <= 0)    //未连接
            {
                for(int j = 0;j < 7;j++)
                {
                    QTableWidgetItem *item = ui->QDevice->item(ti, j);
                    item->setTextColor(QColor(0,0,0));          //设置文本颜色为：黑色
                }
            }
            else        //已连接
            {
                for(int j = 0;j < 7;j++)
                {
                    QTableWidgetItem *item = ui->QDevice->item(ti, j);
                    item->setTextColor(Qt::blue);           //设置文本颜色为：蓝色
                }
            }
            return;
        }
    }
}

//设备掉线
void equipment::SlotOnlineDevice(int nId, int Alarm)
{
    int count = ui->QDevice->rowCount();
    if(count <= 0)    //不能给没有数据的列表，设置颜色，所以判断当前页面有没有数据
    {
        qDebug()<<"返回2";
        return;
    }

    qDebug()<<"掉线："<<nId;
    QTableWidgetItem *tabledmItem8 = new QTableWidgetItem("未连接");
    tabledmItem8->setTextAlignment(Qt::AlignCenter);
    ui->QDevice->setItem(nId, 4, tabledmItem8);

    if(Alarm)
    {
        QTableWidgetItem *item = ui->QDevice->item(nId, 4);
        item->setBackgroundColor(QColor(255,0,0));      //红色
    }

    for(int j = 0;j < 7;j++)
    {
        QTableWidgetItem *item = ui->QDevice->item(nId, j);
        item->setTextColor(QColor(0,0,0));              //黑色
    }
}

/**********初始化函数：报警数据************/
void equipment::_InitAlarm()
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
    ui->QAlarm->setColumnWidth(0,59);
    ui->QAlarm->setColumnWidth(1,150);
    ui->QAlarm->setColumnWidth(2,200);
    ui->QAlarm->setColumnWidth(3,80);
    ui->QAlarm->setColumnWidth(4,80);
    ui->QAlarm->setColumnWidth(5,80);
    ui->QAlarm->setColumnWidth(6,80);
    ui->QAlarm->setColumnWidth(7,80);
}

/**********信号槽函数：实时更新报警数据************/
void equipment::SlotDeal()
{
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
        if(FindAlarmItem(sAlarmDevice, m_Alarm))//是否实时的报警
        {
            m_Alarm = DeleteItemAlarm(&sAlarmDevice, &m_Alarm);
            _UpdateAlarm(m_Alarm);
            emit SigCancleAlarm2();     //更新m_Alarm链表

            int isNULL = AddrFindAlarmItem(sAlarmDevice, m_Alarm);  //查找链表
            if(isNULL == 1)     //是否更新实时表
            {
                m_Device = AlarmUpdateDeviceItem(sAlarmDevice, m_Device);
                emit SigCancleAlarm();      //更新m_Device链表
                _UpdateDevice(m_Device);
            }
        }

        //数据库更新
        mysql._UpdateDealAlarm(sAlarmDevice,"误报","1","11");
    }
    else
    {
        QMessageBox::information(this,tr("提示"),tr("请选中目标"));
    }
}

void equipment::SlotInsertAlarm(NodeDevice *Device, NodeAlarm *Alarm)
{
    m_Device = Device;
    m_Alarm = Alarm;
    _UpdateDevice(m_Device);
    _UpdateAlarm(m_Alarm);
}

/************更新函数：报警数据**************/
void equipment::_UpdateAlarm(NodeAlarm *ItemAlarm)
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

//智能家居
void equipment::SlotDragSmartControl()
{
    if(ui->QDevice->rowCount() <= 0)
    {
        QMessageBox::information(this, tr("提示"), tr("操作失败，当前没有设备记录"));
        return;
    }

    QString strS = ui->QDevice->item(ui->QDevice->currentRow(), 6)->text(); //设备类型
    if(deviceType != 1 && strS != "行政机")
    {
        QMessageBox::information(this, tr("提示"), tr("请选择一台行政机！"));
        return;
    }

    QString strConnect = ui->QDevice->item(ui->QDevice->currentRow(), 4)->text();   //连接状态
    if(strConnect == "未连接")
    {
        QMessageBox::information(this, tr("提示"), tr("设备未连接，请连接设备之后再试！"));
        return;
    }
    QString strIP = ui->QDevice->item(ui->QDevice->currentRow(), 3)->text();    //ip地址

    QString SQL = "SELECT * FROM device WHERE Device_Ip_Addr = '"+ strIP +"'";
    QSqlQuery query;
    qDebug()<<query.exec(SQL)<<SQL;
    query.next();
    QString strAddr = query.value(1).toString();    //设备地址
    QString Addr;
    Addr = _AddrExplainToAddr(strAddr, "行政机");    //设备地址转换

    qDebug()<<"智能家居一键查询";
    QString Ip = query.value(3).toString();
    emit SigDragSmart(Addr, Ip);

    dragsmart = new dragsmartContorl();     //为了每次打开时都是初始状态，必须在使用的时候再new
    dragsmart->exec();  //显示模态对话框
    dragsmart->setAttribute(Qt::WA_DeleteOnClose);
    connect(dragsmart, SIGNAL(sendMessageToUdp(QString, QString, QString, QString, QString)), this, SLOT(SlotMessageFromDrag(QString, QString, QString, QString, QString)));//测试函数
}

void equipment::SlotSendLamp(char *buf, QString strIp)
{
    emit SigUdpSendLamp(buf, strIp);
}

//设备地址转化
QString equipment::_AddrExplainToAddr(QString AddrExplain, QString strType)
{
    QString str;
    if(strType == "访客机")
    {
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
    }
    else if(strType == "围墙机")
    {
        QString str1 = AddrExplain;
        str = str1.left(4);
        str1 = str1.right(str1.length() - 5);

        str = str + str1.left(3);
        str1 = str1.right(str1.length() - 5);

        str = str +"000000"+ str1.left(3);
    }
    else if(strType == "访客机")
    {
        QString str1 = AddrExplain;
        str = str1.left(4);
        str1 = str1.right(str1.length() - 5);

        str = str + str1.left(3);
        str1 = str1.right(str1.length() - 5);

        str = str +"000000"+ str1.left(3);
    }
    else if(strType == "行政机")
    {
        QString strAddr = AddrExplain;
        str.clear();//解析设备地址
        str = str + strAddr.left(4);//S001

        strAddr = strAddr.right(strAddr.length() - 5);
        str = str + strAddr.left(3);//001

        strAddr = strAddr.right(strAddr.length() - 5);
        str = str + strAddr.left(3);//009

        strAddr = strAddr.right(strAddr.length() - 4);
        str = str + strAddr.left(3);//005

        strAddr = strAddr.right(strAddr.length() - 4);
        str = str + strAddr.left(3) + "0000";
    }
    return str;
}

//报警处理
void equipment::on_QAlarm_customContextMenuRequested(const QPoint &pos)
{
    AlarmMenu->addAction(DealAction);
    AlarmMenu->exec(QCursor::pos());
}

void equipment::SlotFocusOut()
{
    ui->QType->setCurrentItem(ui->QType->item(-1,0));
}

void equipment::SlotEquipmentClick()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void equipment::on_QpbtnS_clicked()
{
    deviceType = 1;
    ui->stackedWidget->setCurrentIndex(1);
    _UpdateDevice(m_Device);
}

void equipment::on_QpbtnM_clicked()
{
    deviceType = 2;
    ui->stackedWidget->setCurrentIndex(1);
    _UpdateDevice(m_Device);
}

void equipment::on_QpbtnZ_clicked()
{
    deviceType = 3;
    ui->stackedWidget->setCurrentIndex(1);
    _UpdateDevice(m_Device);
}

void equipment::on_QpbtnAll_clicked()
{
    deviceType = 0;
    ui->stackedWidget->setCurrentIndex(1);
    _UpdateDevice(m_Device);
}

/******* 信号槽：测试 ******/
void equipment::SlotMessageFromDrag(QString DeviceType, QString RoomCode, QString DeviceCode, QString strType, QString strCode)
{
    QString _Addr = ui->QDevice->item(ui->QDevice->currentRow(), 1)->text(); //设备地址
    QString Addr = _AddrExplainToAddr(_Addr, "行政机");
    QString Ip   = ui->QDevice->item(ui->QDevice->currentRow(), 3)->text(); //IP地址
    emit SigMessage(Addr, Ip, DeviceType, RoomCode, DeviceCode, strType, strCode);
}
