#include "udpserver.h"
#include <QMessageBox>
#include <QDebug>
CUDPSERVER::CUDPSERVER(QString strCityNum,QObject *parent) :
    QThread(parent)
{
    qRegisterMetaType<QVariant>("QVariant");    //注册QVariant，为了使信号槽能使用QVariant做参数
    m_DeviceItem = (NodeDevice *)malloc(sizeof(NodeDevice));
    m_DeviceItem ->next = NULL;
    m_AlarmItem = (NodeAlarm *)malloc(sizeof(NodeAlarm));
    m_AlarmItem ->next = NULL;

    m_strCity = strCityNum;//获取天气时，查询网址需要用到
    dataNUM = 0;
    _Init();    //初始化成员变量
    _QueryNowWeather(); //获取实时天气
    _QueryDayWeather(); //获取当天天气
    _InitDevice();  //初始化链表

    QTimer *timer = new QTimer();
    connect(timer, SIGNAL(timeout()), SLOT(_UpdateWeather()));  //发送对象和接受对象是同一个的话，接受对象可以省略
    timer->start(1000);
}
//管理机处理报警信息之后，更新报警链表
void CUDPSERVER::SlotCancleAlarm(NodeDevice *devic)
{
    m_DeviceItem = devic;
}
//管理机处理报警信息之后，更新设备链表
void CUDPSERVER::SlotCancleAlarm2(NodeAlarm *alarm)
{
    m_AlarmItem = alarm;
}

int CUDPSERVER::_CRCvelocity(BYTE *pucMsg, BYTE ucMsgLen)
{
    int wCRC = 0xffff;
    int nPos=0;
    while( ucMsgLen-- )
    {
        wCRC ^= *pucMsg++;  //按位异或
        for(nPos=0;nPos<8;nPos++)
        {
            if((wCRC & 0x01) == 1)
            {
                wCRC = wCRC >>1;
                wCRC ^= 0xA001;
            }
            else
                wCRC = wCRC >>1;
        }
    }
    // 该算法计算出的效验和说明书上的正好高低字节相反
    wCRC = (wCRC%0x100)*0x100+(wCRC-wCRC%0x100)/0x100;
    return (wCRC);
}

//发送控制灯
void CUDPSERVER::_SendLamp(char *pBuf,QString strIp)
{
    unsigned char gSend[37] = {0};
    memcpy(gSend,m_gHead,6);
    gSend[6] = 6;
    gSend[7] = 1;
    memcpy(gSend+8,(unsigned char*)pBuf,29);
    int nSendlen = 37;
    _Sendto(strIp,gSend,nSendlen);
}

//udp发送
int CUDPSERVER::_Sendto(QString strIp,unsigned char *pSend,int nSendlen)
{
    qDebug()<<"发送的数据为："<<QString((char *)pSend)<<endl;
    unsigned long nAddr = inet_addr(strIp.toStdString().data());
    To.sin_family = AF_INET;
    To.sin_port = htons(8300);
    To.sin_addr.S_un.S_addr = (int)nAddr;
    if(::sendto(this->Fsocket,(char*)pSend,nSendlen,0,(struct sockaddr*)&To,sizeof(struct sockaddr))==SOCKET_ERROR)
    {
        qDebug() <<WSAGetLastError();
        return 0;
    }
    return 1;
}

//收到报警
void CUDPSERVER::_DeviceAlarm(unsigned char *buf,char *pFromIP)
{
    SAlarmDevice sAlarmDevice;
    memset(sAlarmDevice.gcIntraAddr,0,20);
    memset(sAlarmDevice.gcPhone1,0,20);
    memset(sAlarmDevice.gcIpAddr,0,20);
    memset(sAlarmDevice.gcMacAddr,0,20);
    memset(sAlarmDevice.gcStime,0,20);
    memset(sAlarmDevice.gcEtime,0,20);
    memset(sAlarmDevice.gcType,0,20);
    memset(sAlarmDevice.gcAddrExplain,0,64);
    memcpy(sAlarmDevice.gcIntraAddr ,(char*)buf + 8,20);

    QString strIntra = QString(sAlarmDevice.gcIntraAddr);
    _IntraToAddr(strIntra,sAlarmDevice.gcAddrExplain,sAlarmDevice.gcType);
    QString str;
    for(int i = 0;i<6;i++)
    {
        int kk = buf[28+i];
        if(kk < 16)
            str = str + "0";
        str = str + QString::number(kk, 16).toUpper();
         if(i != 5)
            str = str + ":";
    }

    QByteArray byte;
    byte = str.toAscii();

    memcpy(sAlarmDevice.gcMacAddr,byte.data(),byte.size());
    memcpy(sAlarmDevice.gcIpAddr,pFromIP,strlen(pFromIP));

    sAlarmDevice.nFenceId = buf[37];
    sAlarmDevice.nTenementId = 0;

    str.clear();
    str = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    byte.clear();
    byte = str.toAscii();

    memcpy(sAlarmDevice.gcStime,byte.data(),byte.size());
    if(buf[35])//1：特殊报警
    {
        sAlarmDevice.nAlarmType = 0;
        if(FindAlarmItem(sAlarmDevice,m_AlarmItem))
        {
            //特殊报警插入
            if(!FindItemDevice(&sAlarmDevice,m_DeviceItem))
            {
                if(m_Mysql._InsertAlarm(sAlarmDevice))
                {
                    m_AlarmItem = InsertAlarmItem(sAlarmDevice,m_AlarmItem);
                    emit SigInsertAlarm();
                }
            }
        }
    }
    else
    {
        if(buf[34])     //布防状态
        {
            sAlarmDevice.nAlarmType = buf[36];  //类型
            sAlarmDevice.nFenceId = buf[37];    //防区
            if(FindAlarmItem(sAlarmDevice,m_AlarmItem))
            {
                if(!FindItemDevice(&sAlarmDevice,m_DeviceItem))
                {
                    if(m_Mysql._InsertAlarm(sAlarmDevice))
                    {
                        m_AlarmItem = InsertAlarmItem(sAlarmDevice,m_AlarmItem);
                        emit SigInsertAlarm();
                    }
                }
            }
        }
    }
    _AskAlarm(buf,pFromIP);
}

//回复住户设备报警
void CUDPSERVER::_AskAlarm(unsigned char *buf,char *pFromIP)
{
    QString strIp = QString(pFromIP);
    unsigned char pSend[42] = {0};
    memcpy(pSend,buf,42);
    pSend[7] = 2;
    int tSendLen = 42;
    pSend[38] = (tSendLen>>8)&0xFF;
    pSend[39] = tSendLen&0xFF;
    int CRC = _CRCvelocity(pSend,tSendLen-2);
    pSend[40] = (CRC>>8)&0xFF;
    pSend[41] = CRC&0xFF;
    _Sendto(strIp,pSend,tSendLen);
}

//收到取消报警
void CUDPSERVER::_CancelAlarm(unsigned char *buf,char *pFromIP)
{
    SAlarmDevice sAlarmDevice;
    memset(sAlarmDevice.gcIntraAddr,0,20);
    memset(sAlarmDevice.gcPhone1,0,20);
    memset(sAlarmDevice.gcIpAddr,0,20);
    memset(sAlarmDevice.gcMacAddr,0,20);
    memset(sAlarmDevice.gcStime,0,20);
    memset(sAlarmDevice.gcEtime,0,20);
    memset(sAlarmDevice.gcType,0,20);

    memcpy(sAlarmDevice.gcIntraAddr ,(char*)buf + 8,20);
    memcpy(sAlarmDevice.gcIpAddr,pFromIP,strlen(pFromIP));

    QString str;
    QByteArray byte;
    str = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    byte.clear();
    byte = str.toAscii();
    memcpy(sAlarmDevice.gcEtime,byte.data(),byte.size());

    m_Mysql._UpdateAlarm(sAlarmDevice);
    if(UpdataItemAlarm(sAlarmDevice,m_AlarmItem))
    {
        emit SigInsertAlarm();
    }
    _AskCancelAlarm(buf,pFromIP);
}

//回复取消报警
void CUDPSERVER::_AskCancelAlarm(unsigned char *buf,char *pFromIP)
{
    QString strIp = QString(pFromIP);
    unsigned char pSend[34] = {0};
    memcpy(pSend,buf,34);
    pSend[7] = 2;
    int tSendLen = 34;
    pSend[30] = (tSendLen>>8)&0xFF;
    pSend[31] = tSendLen&0xFF;
    int CRC = _CRCvelocity(pSend,tSendLen-2);
    pSend[32] = (CRC>>8)&0xFF;
    pSend[33] = CRC&0xFF;
    _Sendto(strIp,pSend,tSendLen);
}

void CUDPSERVER::_Recv(unsigned char *buf,char *pFromIP)
{
    qDebug()<<"接收到的数据为:"<<QString((char *)buf);
    if(memcmp(buf,m_gHead,6) == 0)
    {
        switch(buf[7])
        {
            case 1:     //下行
                switch(buf[6])
                {
                    case 1://室内机报警
                        qDebug()<<"S室内机报警";
                        _DeviceAlarm(buf,pFromIP);
                        break;
                    case 2://室内机取消报警
                        qDebug()<<"S室内机取消报警";
                        _CancelAlarm(buf,pFromIP);
                        break;
                    case 4://设备在线状态报告
                        qDebug()<<"S设备在线状态报告";
                        _DeviceOnline(buf,pFromIP);
                        break;
                    case 81://注册
                        qDebug()<<"S注册";
                        _RegisterStaff(buf,pFromIP);
                        break;
                    case 83://刷卡
                        qDebug()<<"S刷卡";
                        _SwipeCard(buf,pFromIP);
                        break;
                }
            case 2:     //上行
                switch(buf[6])
                {
                    case 3:     //普通信息发送到室内机
                        break;
                    case 13:    //设备重启
                        emit SigReset();
                        break;
                    case 30:    //初始化密码
                        _RecvInitPass(buf,pFromIP);
                        break;
                    case 61:    //家居控制
                    {
//                        QString strIp = "192.168.0.34";//测试用
//                        unsigned char pSend[42] = {0};
//                        memcpy(pSend,buf,42);
//                        int tSendLen = 42;
//                        SigUdpCliToSer(strIp,pSend,tSendLen);//客户端->服务器
                        SmaetTransfer(buf, pFromIP);
                        break;
                    }
                    case 60:    //智能家居一键查询
                    {
//                        QString strIp = "192.168.0.34";
//                        int lampNum = ((buf[33]&0xF0)*4096)+((buf[33]&0x0F)*256)+((buf[34]&0xF0)*16)+buf[34]&0x0F;//设备数量
//                        int curtainNum = ((buf[36+lampNum*8]&0xF0)*4096)+((buf[36+lampNum*8]&0x0F)*256)+((buf[37+lampNum*8]&0xF0)*16)+buf[37+lampNum*8]&0x0F;
//                        int airNum = ((buf[39+(lampNum+curtainNum)*8]&0xF0)*4096)+((buf[39+(lampNum+curtainNum)*8]&0x0F)*256)+((buf[40+(lampNum+curtainNum)*8]&0xF0)*16)+buf[40+(lampNum+curtainNum)*8]&0x0F;
//                        int leng = 47+(lampNum+curtainNum)*8+17*airNum;
//                        unsigned char pSend[leng];
//                        memcpy(pSend,buf,leng);
                        Smartfurniture(buf, pFromIP);
//                        SigUdpCliToSer(strIp,pSend,leng);//客户端->服务器
                    }
                        break;
                    case 82:
                        qDebug()<<"R注销";
                        _RecvLeaveStaff(buf,pFromIP);
                        break;
                    case 84:
                        switch(buf[8])
                        {
                            case 2:
                            {
                                qDebug()<<"R刷卡记录";
                                QString strIP = QString(pFromIP);
                                _UpInformationCard(buf,strIP);
                            }
                                break;
                            default:
                                break;
                        }
                    case 85:
                        qDebug()<<"R注销所有";
                        _DeleteAllStaff();
                        break;
                    default :
                        break;
                }
                break;
            default :
                break;
        }
    }
}

QString CUDPSERVER::_AddrExplainToIntraAddr(QString strExplain)
{
    QString str;
    if(strExplain.left(1) == "S" || strExplain.left(1) == "H")
    {
        QString str1 = strExplain;
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
    else if(strExplain.left(1) == "W")
    {
        QString str1 = strExplain;
        str = str1.left(4);
        str1 = str1.right(str1.length() - 5);

        str = str + str1.left(3);
        str1 = str1.right(str1.length() - 5);

        str = str +"000000"+ str1.left(3);
    }
    else if(strExplain.left(1) == "M")
    {
        QString str1 = strExplain;
        str = str1.left(4);
        str1 = str1.right(str1.length() - 5);

        str = str + str1.left(3);
        str1 = str1.right(str1.length() - 5);

        str = str +"000000"+ str1.left(3);
    }
    return str;
}

//设备地址转化
void CUDPSERVER::_IntraToAddr(QString strIntra,char *pAddr,char *pType)
{
    QString str,strAddr;
    if(strIntra.left(1) == "S")//“S”：室内机
    {
        str.clear();
        strAddr.clear();
        str = strIntra.left(4);
        strAddr = str + "栋";
        strIntra = strIntra.right(strIntra.length() - 4);

        str.clear();
        str = strIntra.left(3);
        strAddr = strAddr + str + "单元";
        strIntra = strIntra.right(strIntra.length() - 3);

        str.clear();
        str = strIntra.left(3);
        strAddr = strAddr + str + "楼";
        strIntra = strIntra.right(strIntra.length() - 3);

        str.clear();
        str = strIntra.left(3);
        strAddr = strAddr + str + "房";
        strIntra = strIntra.right(strIntra.length() - 3);

        str.clear();
        str = strIntra.left(3);
        strAddr = strAddr + str + "号设备";

        memcpy(pAddr,strAddr.toUtf8().data(),strAddr.toUtf8().size());
//        QString strType = "室内机";
        QString strType = "行政机";
        memcpy(pType,strType.toUtf8().data(),strType.toUtf8().size());
    }
    else if(strIntra.left(1) == "M")//“M”:单元门口机
    {
        str.clear();
        strAddr.clear();
        str = strIntra.left(4);
        strAddr = str + "栋";
        strIntra = strIntra.right(strIntra.length() - 4);

        str.clear();
        str = strIntra.left(3);
        strAddr = strAddr + str + "单元";
        strIntra = strIntra.right(strIntra.length() - 3);

        strIntra = strIntra.right(strIntra.length() - 6);

        str.clear();
        str = strIntra.left(3);
        strAddr = strAddr + str + "号设备";

        memcpy(pAddr,strAddr.toUtf8().data(),strAddr.toUtf8().size());
//        QString strType = "单元门口机";
        QString strType = "访客机";
        memcpy(pType,strType.toUtf8().data(),strType.toUtf8().size());
    }
    else if(strIntra.left(1) == "W")//“W”：围墙机
    {
        str.clear();
        strAddr.clear();
        str = strIntra.left(4);
        strAddr = str + "栋";
        strIntra = strIntra.right(strIntra.length() - 4);


        strIntra = strIntra.right(strIntra.length() - 9);

        str.clear();
        str = strIntra.left(3);
        strAddr = strAddr + str + "号设备";

        memcpy(pAddr,strAddr.toUtf8().data(),strAddr.toUtf8().size());
        QString strType = "围墙机";
        memcpy(pType,strType.toUtf8().data(),strType.toUtf8().size());
    }
    else if(strIntra.left(1) == "Z")//“Z”：中心机
    {
        str.clear();
        strAddr.clear();
        str = strIntra.left(4);
        strAddr = str + "栋";
        strIntra = strIntra.right(strIntra.length() - 4);

        strIntra = strIntra.right(strIntra.length() - 9);

        str.clear();
        str = strIntra.left(3);
        strAddr = strAddr + str + "号设备";

        memcpy(pAddr,strAddr.toUtf8().data(),strAddr.toUtf8().size());
        QString strType = "中心机";
        memcpy(pType,strType.toUtf8().data(),strType.toUtf8().size());
    }
    else if(strIntra.left(1) == "H")//“H”：二次门口机
    {
        str.clear();
        strAddr.clear();
        str = strIntra.left(4);
        strAddr = str + "栋";
        strIntra = strIntra.right(strIntra.length() - 4);

        str.clear();
        str = strIntra.left(3);
        strAddr = strAddr + str + "单元";
        strIntra = strIntra.right(strIntra.length() - 3);

        str.clear();
        str = strIntra.left(3);
        strAddr = strAddr + str + "楼";
        strIntra = strIntra.right(strIntra.length() - 3);

        str.clear();
        str = strIntra.left(3);
        strAddr = strAddr + str + "房";
        strIntra = strIntra.right(strIntra.length() - 3);

        str.clear();
        str = strIntra.left(3);
        strAddr = strAddr + str + "号设备";

        memcpy(pAddr,strAddr.toUtf8().data(),strAddr.toUtf8().size());
//        QString strType = "二次门口机";
        QString strType = "访客机";
        memcpy(pType,strType.toUtf8().data(),strType.toUtf8().size());
    }
    return ;
}

//刷卡
void CUDPSERVER::_SwipeCard(unsigned char *buf,char *pFromIP)
{
    char pIntraAddr[20] = {0};
    char gcType[20] = {0};          //接收用
    char gcAddrExplain[45] = {0};   //接收用
    char gccardTime[7]={0};
    memcpy(pIntraAddr,(char*)buf+8,20);

    QDateTime curTime = QDateTime::currentDateTime();
    int year = curTime.date().year();
//    gccardTime[0] = QString::number(year).left(2).toInt();  //截取字符串
//    gccardTime[1] = QString::number(year).right(2).toInt();
    gccardTime[0] = curTime.date().year()/256;
    gccardTime[1] = curTime.date().year()%256;
    gccardTime[2] = curTime.date().month();
    gccardTime[3] = curTime.date().day();
    gccardTime[4] = curTime.time().hour();
    gccardTime[5] = curTime.time().minute();
    gccardTime[6] = curTime.time().second();

    int nID = buf[28]*256 + buf[29];

    QString strIntra,strAddrExplain;

    strIntra = QString(pIntraAddr);
    _IntraToAddr(strIntra,gcAddrExplain,gcType);
    strAddrExplain = QString(gcAddrExplain);

    m_Mysql._InsertCardRecord(nID,strAddrExplain,gccardTime);

    QString strIp = QString(pFromIP);
    unsigned char pSend[41] = {0};
    memcpy(pSend,buf,37);
    pSend[7] = 2;
    int tSendLen = 41;
    pSend[37] = (tSendLen>>8)&0xFF; //长度位
    pSend[38] = tSendLen&0xFF;
    int CRC = _CRCvelocity(pSend,tSendLen-2);
    pSend[39] = (CRC>>8)&0xFF;  //校验位
    pSend[40] = CRC&0xFF;
    qDebug()<<"刷卡"<<gccardTime[0]<<gccardTime[1]<<"ID"<<nID;
    _Sendto(strIp,pSend,tSendLen);
}

//接收到注销命令
void CUDPSERVER::_RecvLeaveStaff(unsigned char *buf,char *pFromIp)
{
    int nId = buf[28]*256 + buf[29];
    qDebug()<<"注销ID为："<<nId;
    if(m_Mysql._LogOutIC(nId))
    {
//        子线程里不能对GUI界面进行操作
//        QMessageBox::information(NULL,"提示","注销成功");
        emit SigRecvLogOut();
        qDebug()<<"接收成功";
    }
    else
    {
        QMessageBox::information(NULL,"提示","数据库连接失败");
    }
//    unsigned char pSend[34] = {0};
//    memcpy(pSend,buf,30);
//    QString strIp = QString(pFromIp);
//    pSend[7]= 1;
//    int tSendLen = 34;
//    pSend[30] = (tSendLen>>8)&0xFF;
//    pSend[31] = tSendLen&0xFF;
//    int CRC = _CRCvelocity(pSend,tSendLen-2);
//    pSend[32] = (CRC>>8)&0xFF;
//    pSend[33] = CRC&0xFF;
//    _Sendto(strIp,pSend,tSendLen);
}

//注册
void CUDPSERVER::_RegisterStaff(unsigned char *buf,char *pFromIP)
{
    char pIntraAddr[20] = {0};
    char gcType[20] = {0};
    char gcAddrExplain[45] = {0};
    memcpy(pIntraAddr,(char*)buf+8,20);
    int nID = buf[28]*256 + buf[29];

    QString strIntra,strAddrExplain;

    strIntra = QString(pIntraAddr);
    _IntraToAddr(strIntra,gcAddrExplain,gcType);
    strAddrExplain = QString(gcAddrExplain);
    m_Mysql._InsertRegisterStaff(nID,strAddrExplain);//插入数据库

    QString strIp = QString(pFromIP);
    unsigned char pSend[34] = {0};
    memcpy(pSend,buf,30);
    pSend[7] = 2;
    int tSendLen = 34;
    pSend[30] = (tSendLen>>8)&0xFF;
    pSend[31] = tSendLen&0xFF;
    int CRC = _CRCvelocity(pSend,tSendLen-2);
    pSend[32] = (CRC>>8)&0xFF;
    pSend[32] = CRC&0xFF;
    qDebug()<<"设备注册";
    _Sendto(strIp,pSend,tSendLen);
}

//收到设备在线数据
void CUDPSERVER::_DeviceOnline(unsigned char *buf,char *pFromIP)
{
    SDevice sDevice;    //初始化结构体变量
    memset(sDevice.gcAddr,0,45);
    memset(sDevice.gcIntraAddr,0,20);
    memset(sDevice.gcIpAddr,0,20);
    memset(sDevice.gcMacAddr,0,20);
    memset(sDevice.gcType,0,20);

    sDevice.nId = 0;
    sDevice.nState = 0;     //连接状态
    sDevice.nAlarmState = 0;
    sDevice.nFenceState = buf[34];
//    qDebug()<<"sDevice.nFenceState:"<<sDevice.nFenceState;
    memcpy(sDevice.gcIntraAddr,(char *)buf + 8,20);
    memcpy(sDevice.gcIpAddr,pFromIP,strlen(pFromIP));
    QString strIntra = QString(sDevice.gcIntraAddr);
    _IntraToAddr(strIntra,sDevice.gcAddr,sDevice.gcType);//设备地址转化
    QString str;

    for(int i = 0; i<6; i++)//网卡地址
    {
        int k = buf[28+i];
        if(k < 16)
            str = str + "0";
        str = str + QString::number(k, 16).toUpper();
        if(i != 5)
            str = str + ":";
    }

    QByteArray byte;
    byte = str.toAscii();

    memcpy(sDevice.gcMacAddr,byte.data(),byte.size());

    int nMark = ItemDeviceFind(&sDevice,m_DeviceItem);      //查找设备
    qDebug()<<"nMark:"<<nMark;
    switch(nMark)
    {
        case -3:    //更新设备
            if(m_Mysql._UpdateDevice(nMark,sDevice))
            {
                m_DeviceItem = DeleteItemDevice(sDevice,&m_DeviceItem);
                m_DeviceItem = InsertItemDevice(sDevice,m_DeviceItem);
                qDebug()<<"更新："<<m_DeviceItem->data.gcIpAddr<<m_DeviceItem->data.nFenceState<<m_DeviceItem->data.nState;
                emit SigInitDevice();           //更新设备界面的设备连接状态
            }
            break;
        case -2:
            if(m_Mysql._UpdateDevice(nMark,sDevice))
            {
                m_DeviceItem = UpdateItemDevice(sDevice,m_DeviceItem);
                QVariant var1;
                var1.setValue(sDevice);
                qDebug()<<"-2:"<<sDevice.gcIpAddr<<sDevice.nFenceState<<sDevice.nState;
                emit SigUpdateDevice(var1);
            }
            break;
        case -1:    //插入设备
            if(m_Mysql._InsertDevice(sDevice))
            {
                m_DeviceItem = InsertItemDevice(sDevice,m_DeviceItem);
                qDebug()<<"插入："<<m_DeviceItem->data.gcIpAddr<<m_DeviceItem->data.nFenceState<<m_DeviceItem->data.nState;
                emit SigInitDevice();           //更新设备界面的设备连接状态
            }
            break;
        default:
            if(m_Mysql._UpdateDevice(nMark,sDevice))
            {
                m_DeviceItem = UpdateItemDevice(sDevice,m_DeviceItem);
                QVariant var1;
                var1.setValue(sDevice);
                qDebug()<<"no gengxin no charu "<<m_DeviceItem->data.gcIpAddr<<m_DeviceItem->data.nFenceState;
                emit SigUpdateDevice(var1);
            }
            break;
    }
    _AskDeviceOnline(buf,pFromIP);
}

//回复设备在线指令
void CUDPSERVER::_AskDeviceOnline(unsigned char *buf,char *pFromIP)
{
    unsigned char pSend[512] = {0};
    memcpy(pSend,buf,28);
    pSend[7] = 2;
    pSend[28] = 10;
    pSend[29] = 0;
    QDateTime dateTime = QDateTime::currentDateTime();
    pSend[30] = dateTime.date().year()/256;
    pSend[31] = dateTime.date().year()%256;
    pSend[32] = dateTime.date().month();
    pSend[33] = dateTime.date().day();
    pSend[34] = dateTime.time().hour();
    pSend[35] = dateTime.time().minute();
    pSend[36] = dateTime.time().second();
    pSend[37] = sWeather.nWeather;
    pSend[38] = sWeather.nMinTemp;
    pSend[39] = sWeather.nMaxTemp;
    int tSendLen = 44;
    pSend[40] = (tSendLen>>8)&0xFF;
    pSend[41] = tSendLen&0xFF;
    int CRC = _CRCvelocity(pSend,tSendLen-2);
    pSend[42] = (CRC>>8)&0xFF;
    pSend[43] = CRC&0xFF;
    int nSendlen = 512;
    QString strIp = QString(pFromIP);
    _Sendto(strIp,pSend,nSendlen);
}

//发送复位密码
void CUDPSERVER::_SendPass(QString strAddr,QString strMac,QString strIp)
{
    unsigned char pSend[35] = {0};
    QString Addr;
    Addr.clear();
    memcpy(pSend,m_gHead,6);
    pSend[6] = 30;
    pSend[7] = 1;

    Addr = _AddrExplainToIntraAddr(strAddr);
    QByteArray byte;
    byte = Addr.toUtf8();
    memcpy(pSend+8,byte.data(),byte.size());
    QString str;
    str.clear();
    for(int i = 0;i < 5;i++)
    {
        int j = strMac.indexOf(":");
        str = strMac.left(j);
        bool ok;
        pSend[28+i] = str.toInt(&ok,16);
        strMac = strMac.right(strMac.length() - j - 1);
        if(i == 4)
        {
            pSend[29+i] = strMac.toInt(&ok,16);
        }
    }

    pSend[34] = 1;
    _Sendto(strIp,pSend,35);
}

//接收复位密码回复
void CUDPSERVER::_RecvInitPass(unsigned char *buf,char *pFromIP)
{
    int nFlag = buf[34];
    char gcAddr[20] = {0};
    memcpy(gcAddr,(char*)buf + 8,20);   //设备地址
    emit SigRecvInitPass(QString(gcAddr),nFlag);
}

//注册模式
void CUDPSERVER::_Register(QString strAddr,QString strIp)
{
    unsigned char pSend[32] = {0};
    memcpy(pSend,m_gHead,6);
    pSend[6] = 80;
    pSend[7] = 1;
    memcpy(pSend+8,strAddr.toAscii().data(),strAddr.toAscii().size());
    int tSendLen = 32;
    pSend[28] = (tSendLen>>8)&0xFF;
    pSend[29] = tSendLen&0xFF;
    int CRC = _CRCvelocity(pSend,tSendLen-2);
    pSend[30] = (CRC>>8)&0xFF;
    pSend[31] = CRC&0xFF;
    qDebug()<<"进入注册模式：";
    _Sendto(strIp,pSend,tSendLen);
}

//重启设备命令生成
void CUDPSERVER::_Reset(QString strIp,QString strAddr,QString strType)
{
    unsigned char pSend[28] = {0};
    QString Addr;
    Addr.clear();
    memcpy(pSend,m_gHead,6);
    pSend[6] = 13;
    pSend[7] = 1;

    Addr = _AddrExplainToIntraAddr(strAddr);

    QByteArray byte;
    byte = Addr.toUtf8();
    memcpy(pSend+8,byte.data(),byte.size());
    _Sendto(strIp,pSend,28);
}

//注销
void CUDPSERVER::_StaffLeave(int Id)
{
    qDebug()<<"发送的注销ID"<<Id;
    NodeDevice *p = m_DeviceItem;
    while(p->next)  //注销该ID在所有设备上的记录，包括室内机和门口机
    {
        unsigned char gSend[34] = {0};
        memcpy(gSend,m_gHead,6);
        gSend[6] = 82;
        gSend[7] = 1;
        memcpy(gSend+8,p->data.gcIntraAddr,20);
        int tSendLen = 34;
//        gSend[28] = (Id>>8)&0XFF;
//        gSend[29] = Id&0Xff;
        gSend[28] = 0;
        gSend[29] = Id;
        gSend[30] = (tSendLen>>8)&0xFF;
        gSend[31] = tSendLen&0xFF;
        int CRC = _CRCvelocity(gSend, tSendLen-2);
        gSend[32] = (CRC>>8)&0xFF;
        gSend[33] = CRC&0xFF;
        _Sendto(QString(p->data.gcIpAddr),gSend,tSendLen);  //在这动态的获取目的IP
        p = p->next;
    }
}

/************************************************************************************
int setsockopt(int sockfd, int level, int optname,const void *optval, socklen_t optlen);
sockfd： 标识一个套接口的描述字。
level：  选项定义的层次；支持SOL_SOCKET、IPPROTO_TCP、IPPROTO_IP和IPPROTO_IPV6。
optname：需设置的选项。
optval： 指针，指向存放选项待设置的新值的缓冲区。
optlen： optval缓冲区长度。
返回：若无错误发生，setsockopt()返回0。否则的话，返回SOCKET_ERROR错误，
     应用程序可通过WSAGetLastError()获取相应错误代码

inet_addr()的功能是将一个点分十进制的IP转换成一个长整数型数（u_long类型）
返回：若字符串有效则将字符串转换为32位二进制网络字节序的IPV4地址，否则为INADDR_NONE

htonl()将主机数转换成无符号长整型的网络字节顺序。本函数将一个32位数从主机字节顺序转换成网络字节顺序

创建一个套接口
SOCKET PASCAL FAR socket( int af, int type, int protocol)
af：一个地址描述。目前仅支持AF_INET格式，也就是说ARPA Internet地址格式
type：新套接口的类型描述
protocol：套接口所用的协议。如调用者不想指定，可用0指定，表示缺省
***********************************************************************************/
//线程启动，绑定udp
void CUDPSERVER::run()
{
    //进行版本协商，加载特定版本的socket动态链接库
    int retVal;
    int port = 8300;
    if((retVal = WSAStartup(MAKEWORD(2,2), &wsd)) != 0)//Windows Sockets Asynchronous API
    {
        perror("socket1");//用来将上一个函数发生错误的原因输出到标准设备(stderr)
        return ;
    }
    Fsocket = socket(PF_INET,SOCK_DGRAM,IPPROTO_IP);
    if(Fsocket == INVALID_SOCKET)
    {
        emit SigUdpOpenFail();  //equipment.cpp没有响应
        perror("socket2");
        return;
    }
    bool bOptVal=FALSE;
    retVal = setsockopt(Fsocket,SOL_SOCKET,SO_BROADCAST,(char*)&bOptVal,sizeof(bool));//可以发生广播
    struct sockaddr_in FAddr;
    memset(&FAddr,0,sizeof(FAddr));
    FAddr.sin_family = AF_INET;
    FAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    FAddr.sin_port = htons(port);
    if(bind(Fsocket,(sockaddr*)&FAddr,sizeof(FAddr))==SOCKET_ERROR)
    {
        emit SigUdpOpenFail();  //equipment.cpp没有响应
        perror("socket3");
        return;
    }
    struct ip_mreq imreq;
    imreq.imr_multiaddr.s_addr = inet_addr(m_strMulticastIP.toLatin1().data());//inet_addr("238.9.9.1");
    imreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if(setsockopt(Fsocket,IPPROTO_IP,IP_ADD_MEMBERSHIP,(char*)&imreq,sizeof(ip_mreq))==SOCKET_ERROR)
    {
        emit SigUdpOpenFail();  //equipment.cpp没有响应
        perror("socket4");
//        qDebug() <<WSAGetLastError();
        return;
    }
    while(1)
    {
        _Read();
    }
}

//接收数据
int CUDPSERVER::_Read()
{
    struct sockaddr_in from;
    int nLength = sizeof(struct sockaddr_in);
    unsigned char buf[8100] = {0};  //？？？
    char gFromIp[20] = {0};
    int t_nRecvlen = ::recvfrom(this->Fsocket,(char*)buf,8100,0,(struct sockaddr * )&from,&nLength);
    if(!t_nRecvlen)
    {
//        qDebug() <<WSAGetLastError();
        return 0;
    }
    strcpy(gFromIp,inet_ntoa(from.sin_addr));
    _Recv(buf,gFromIp);
    return t_nRecvlen;
}

//定时器刷新
void CUDPSERVER::_UpdateWeather()
{
    QDateTime dateTime = QDateTime::currentDateTime();
//    int hour = dateTime.time().hour();
    int minute = dateTime.time().minute();
    int second = dateTime.time().second();

    if((minute == 5) && (second == 1))//每小时重新获取天气和有效期
    {
        _QueryNowWeather();
        _QueryDayWeather();
        _ReadConfig();
    }

    NodeDevice *q;
    q = m_DeviceItem;
    while(q->next)
    {
//        qDebug()<<"shin"<<q->data.gcIpAddr<<q->data.nState;
        if(q->data.nState > 0)//在线监测
        {
            q->data.nState --;      //初始值是ONLINE_TIME 15
            if(q->data.nState <= 0)
            {
//                QString IP = q->data.gcIpAddr;
//                qDebug()<<"baojzt"<<q->data.nAlarmState<<q->data.nId<<q->data.gcIpAddr;
                m_Mysql._OnlineDevice(q->data);
                emit SigOnlineDevice(q->data.nId,q->data.nAlarmState);
//                emit SigOnlineDevice(IP, q->data.nAlarmState);
            }
        }
        q = q->next;
    }
}

//初始化成员变量
void CUDPSERVER::_Init()
{
    memset(m_gHead,0,6);
    memcpy(m_gHead,"XXXCID",6);     //包头
    //返回IP地址列表中的第一个IP，并转换为字符串
    m_strLocalIP = QNetworkInterface::allAddresses().first().toString();
    m_strMulticastIP = "238.9.9.1";     //以后广播时用到

    //Weather
    sWeather.nWeather = 0;
    sWeather.nMaxTemp = 0;
    sWeather.nMinTemp = 0;
    m_NetworkManager = new QNetworkAccessManager(this);     //查询天气需要用到
    m_NetworkManager1 = new QNetworkAccessManager(this);
}

//更新城市编码，并获取天气实时数据
void CUDPSERVER::_UpdateLoading(QString strCityNum)
{
    m_strCity = strCityNum;
    _QueryNowWeather();
    _QueryDayWeather();
}

//获取实时天气
void CUDPSERVER::_QueryNowWeather()
{
    QString url = "http://www.weather.com.cn/weather1d/"+m_strCity+".shtml";//"http://www.weather.com.cn/data/sk/"+m_strCity+".html";
    m_NetworkRequest1.setUrl(QUrl(url));    //设置天气的地址
    m_NetworkReply1 = m_NetworkManager1->get(m_NetworkRequest1);    //将URL发送到网络管理器中
    connect(m_NetworkReply1, SIGNAL(readyRead()), this, SLOT(SlotReadyRead1()));
}

//获取当天天气
void CUDPSERVER::_QueryDayWeather()
{
    QString url = "http://www.weather.com.cn/data/cityinfo/"+m_strCity+".html";
    m_NetworkRequest.setUrl(QUrl(url));
    m_NetworkReply = m_NetworkManager->get(m_NetworkRequest);
    connect(m_NetworkReply, SIGNAL(readyRead()), this, SLOT(SlotReadyRead()));
}

//获取天气到的数据的解析
void CUDPSERVER::SlotReadyRead()
{
    QString strXmlFile = m_NetworkReply->readAll();//以QByteArray类型返回所有可以获得的数据
    int t_nLen = strXmlFile.indexOf(":",0) + 1;
    if(t_nLen)
    {
        strXmlFile = strXmlFile.right(strXmlFile.length()-t_nLen);
        strXmlFile = strXmlFile.left(strXmlFile.length()-1);
        QScriptEngine engine;
        QScriptValue sc = engine.evaluate("value="+strXmlFile);
        QString temp = sc.property("temp2").toString();
        sWeather.nMinTemp = temp.left(temp.length()-1).toInt();
        temp.clear();
        temp = sc.property("temp1").toString(); //返回指定名字的属性值
        sWeather.nMaxTemp = temp.left(temp.length()-1).toInt();

        QString str = sc.property("weather").toString();
        if(str.indexOf("雪",0) == -1)
        {
            if(str.indexOf("雨",0) == -1)
            {
                if(str.indexOf("多云",0) == -1)
                {
                    if(str.indexOf("晴",0) == -1)
                    {
                        sWeather.nWeather = 0;
                    }
                    else
                        sWeather.nWeather = 1;
                }
                else
                    sWeather.nWeather = 2;
            }
            else
                sWeather.nWeather = 3;
        }
        else
            sWeather.nWeather = 4;
    }
}

//获取天气到的数据的解析
void CUDPSERVER::SlotReadyRead1()
{
    QString strXmlFile = m_NetworkReply1->readAll();//以QByteArray类型返回所有可以获得的数据
//    int t_nLen = strXmlFile.indexOf(":",0) + 1;value
    int t_nLen = strXmlFile.indexOf("input type=\"hidden\" id=\"hidden_title\" value=",0) + 44;//？？？
    QString str = strXmlFile.right(strXmlFile.length() - t_nLen).left(30);  //先从右边开始截取字符串，再从左边截

    if(str.indexOf("雪",0) == -1)
    {
        if(str.indexOf("雨",0) == -1)
        {
            if(str.indexOf("多云",0) == -1)
            {
                if(str.indexOf("晴",0) == -1)
                {
                    sWeather.nWeather = 0;
                }
                else
                    sWeather.nWeather = 1;//晴
            }
            else
                sWeather.nWeather = 2;    //多云
        }
        else
            sWeather.nWeather = 3;        //雨
    }
    else
        sWeather.nWeather = 4;            //雪

    t_nLen = str.indexOf("°C",0);
    int t_nLen1 = str.indexOf("/",0);
    if(t_nLen && t_nLen1)
    {
        sWeather.nMinTemp = str.left(t_nLen).right(t_nLen - t_nLen1 - 1).toInt();
        sWeather.nMaxTemp = str.left(t_nLen1).right(2).toInt();
//        qDebug()<<t_nLen1<<t_nLen<<str.length()<<str.left(t_nLen).right(t_nLen - t_nLen1 - 1)<<str.left(t_nLen1).right(2);
    }

    emit SigUpdateWeatherNew();
//    qDebug()<<str<<sWeather.nMinTemp<<sWeather.nMaxTemp<<sWeather.nWeather;

//    if(t_nLen)
//    {
//        strXmlFile = strXmlFile.right(strXmlFile.length()-t_nLen);
//        strXmlFile = strXmlFile.left(strXmlFile.length()-1);
//        QScriptEngine engine;
//        QScriptValue sc = engine.evaluate("value="+strXmlFile);
//        sWeather.strName = sc.property("city").toString(); //解析字段
//        sWeather.nNowTemp = sc.property("temp").toInteger();
//        QString str =  sc.property("SD").toString();
//        str = str.left(str.length()-1);
//        sWeather.nHumidity = str.toInt();
//        sWeather.strWindDirection = sc.property("WD").toString();
//        str.clear();
//        str = sc.property("WS").toString();
//        sWeather.nWind = str.left(str.length()-1).toInt();
//    }else{
//        sWeather.nNowTemp = 0;
//        sWeather.nWind = 0;
//        sWeather.nHumidity = 0;
//    }

}

//读取软件有效期配置
void CUDPSERVER::_ReadConfig()
{
    char buf[12] = {0};
    ::GetPrivateProfileStringA("Flag","STime","sa",buf,12,"C:\\WINDOWS\\UTSet.ini");
    QDate Date = QDate::fromString(QString(buf),"yyyy-MM-dd");
    if(Date.isNull())
    {
        emit SigDateTime(1,0);
        return;
    }
    QDate NowDate = QDateTime::currentDateTime().date();
    int AllDay = Date.daysTo(NowDate);
    int  nFlag = ::GetPrivateProfileIntA("Flag","idex",0,"C:\\WINDOWS\\UTSet.ini");
    if(nFlag==0)
    {
        emit SigDateTime(1,0);
        return;
    }
    else if(nFlag==1)
    {
        if(AllDay < (::GetPrivateProfileIntA("1","AllDay",0,"C:\\WINDOWS\\UTSet.ini")))
        {
            emit SigDateTime(1,0);
            return;
        }
        else
        {
            if(AllDay >=60 &&AllDay <70)
            {
                if(::GetPrivateProfileIntA("1","Count",0,"C:\\WINDOWS\\UTSet.ini") == 0)
                {
                    emit SigDateTime(2,AllDay);
                    ::WritePrivateProfileStringA("1", "Count", QString::number(1).toUtf8().data(), "C:\\WINDOWS\\UTSet.ini");
                }
            }
            else if(AllDay >=70 &&AllDay <80)
            {
                if(::GetPrivateProfileIntA("1","Count",0,"C:\\WINDOWS\\UTSet.ini") <= 1)
                {
                    emit SigDateTime(3,AllDay);
                    ::WritePrivateProfileStringA("1", "Count", QString::number(2).toUtf8().data(), "C:\\WINDOWS\\UTSet.ini");
                }
            }
            else if(AllDay >=80)
            {
                    emit SigDateTime(4,AllDay);
            }
            ::WritePrivateProfileStringA("1", "AllDay", QString::number(AllDay).toUtf8().data(), "C:\\WINDOWS\\UTSet.ini");
        }
    }
    else if(nFlag==2)
    {
        if(AllDay < (::GetPrivateProfileIntA("2","AllDay",0,"C:\\WINDOWS\\UTSet.ini")))
        {
            emit SigDateTime(1,0);
            return;
        }
        else
        {
            if(AllDay >=750 &&AllDay <780){
                if(::GetPrivateProfileIntA("2","Count",0,"C:\\WINDOWS\\UTSet.ini") == 0)
                {
                    emit SigDateTime(2,AllDay);
                    ::WritePrivateProfileStringA("2", "Count", QString::number(1).toUtf8().data(), "C:\\WINDOWS\\UTSet.ini");
                }
            }
            else if(AllDay >=780 &&AllDay <810)
            {
                if(::GetPrivateProfileIntA("2","Count",0,"C:\\WINDOWS\\UTSet.ini") <= 1)
                {
                    emit SigDateTime(3,AllDay);
                    ::WritePrivateProfileStringA("2", "Count", QString::number(2).toUtf8().data(), "C:\\WINDOWS\\UTSet.ini");
                }
            }
            else if(AllDay >=810)
            {
                    emit SigDateTime(4,AllDay);
            }
            ::WritePrivateProfileStringA("2", "AllDay", QString::number(AllDay).toUtf8().data(), "C:\\WINDOWS\\UTSet.ini");
        }
    }
}

//初始化链表
void CUDPSERVER::_InitDevice()
{
    QString SQL = "SELECT * FROM device ORDER BY Device_Intra_Addr DESC";
    QSqlQuery query;
    int i = query.exec(SQL);
    if(i)
    {
        int nAllTenement = 0;
        while(query.next())
        {
            SDevice sDevice;
            memset(sDevice.gcAddr,0,45);
            memset(sDevice.gcIntraAddr,0,20);
            memset(sDevice.gcIpAddr,0,20);
            memset(sDevice.gcMacAddr,0,20);
            memset(sDevice.gcType,0,20);
            sDevice.nAlarmState = 0;    //报警状态
            sDevice.nFenceState = 0;    //布防状态
            sDevice.nId = 0;
            sDevice.nState = 0;         //连接状态

            sDevice.nId = nAllTenement; //每次都是0，后面会赋值修改
            strcpy(sDevice.gcAddr,query.value(1).toByteArray().data());         //Device_Addr
            strcpy(sDevice.gcIntraAddr,query.value(5).toByteArray().data());    //Device_Intra_Addr
            strcpy(sDevice.gcIpAddr,query.value(3).toByteArray().data());       //Device_Ip_Addr
            strcpy(sDevice.gcMacAddr,query.value(2).toByteArray().data());      //Device_Mac_Addr
            strcpy(sDevice.gcType,query.value(6).toByteArray().data());         //Device_Type
            sDevice.nFenceState = query.value(4).toInt();                       //Device_Fence_Statu
            m_DeviceItem = ItemDeviceCreatH(sDevice,m_DeviceItem);              //创建链表，头插法插入节点
        }
    }
    else
    {
        QMessageBox::warning(NULL,tr("提示"),tr("数据库查询失败，请检查数据库连接"));
    }
}

//考勤数据采集
void CUDPSERVER::_Acqkuisition(QString strAddr, QString strIp)
{
    qDebug()<<"考勤数据采集"<<endl;
    unsigned char gSend[21] = {0};
    memcpy(gSend,m_gHead,6);
    QString Addr;
    Addr.clear();
    gSend[6] = 84;
    gSend[7] = 1;
    gSend[8] = 1;
    Addr = _AddrExplainToIntraAddr(strAddr);
    qDebug()<<"Addr"<<Addr<<endl;
    QByteArray byte;
    byte = Addr.toUtf8();
    memcpy(gSend+9,byte.data(),byte.size());
    int nSendlen = 29;
    _Sendto(strIp,gSend,nSendlen);
}

//刷卡记录
void CUDPSERVER::_UpInformationCard(unsigned char *buf, QString strIp)
{
    char pIntraAddr[20] = {0};
    char gcType[20] = {0};
    char gcAddrExplain[45] = {0};
    memcpy(pIntraAddr,(char*)buf+9,20);
    QString strIntra,strAddrExplain;

    strIntra = QString(pIntraAddr);
    _IntraToAddr(strIntra,gcAddrExplain,gcType);

    strAddrExplain = QString(gcAddrExplain);
    recordTotal = (buf[29]&0xF0)*4096+(buf[29]&0x0F)*256+(buf[30]&0xF0)*16+buf[30];
    qDebug()<<"(buf[29]&0xF0)*4096"<<(buf[29]&0xF0)*4096<<endl; //记录总数
    qDebug()<<"recordTotal"<<recordTotal<<endl;

    for(int i=0; i<recordTotal; i++)
    {
        number=i+1;
        QDate RacdDate;
        QTime RacdTime;
        int year = buf[9*i+37]*100+buf[9*i+38];
        int month = buf[9*i+39];
        int day = buf[9*i+40];
        int hour = buf[9*i+41];
        int minute = buf[9*i+42];
        int second = buf[9*i+43];
        RacdDate.setYMD(year,month,day);
        RacdTime.setHMS(hour,minute,second);

        QString Racd_Id = QString((buf[9*i+35]&0xF0)*4096+(buf[9*i+35]&0x0F)*256+(buf[9*i+36]&0xF0)*16+(buf[9*i+36]));
        QString SQL;
        SQL = "INSERT into card_record (card_record.Card_Record_Staff_No,card_record.Card_Record_Date,card_record.Card_Record_Time,card_record.Card_Record_Device_No) VALUES ('"+Racd_Id+"',"
                "'"+RacdDate.toString("yyyy-MM-dd")+"','"+RacdTime.toString("hh:mm:ss")+"','"+strAddrExplain+"')";
        QSqlQuery query;
        qDebug()<<"query insert"<<query.exec(SQL)<<SQL<<endl;
        if(query.exec(SQL))
            emit SigProgress(recordTotal,number);
    }
    m_Mysql._UpdateAllRacd(strAddrExplain,buf);
    unsigned char pSend[39] = {0};
    memcpy(pSend,buf,35);
    int tSendLen = 39;
    pSend[35] = (tSendLen>>8)&0xFF;
    pSend[36] = tSendLen&0xFF;
    int CRC = _CRCvelocity(pSend,tSendLen-2);
    pSend[37] = (CRC>>8)&0xFF;
    pSend[38] = CRC&0xFF;
    qDebug()<<"刷卡记录";
    _Sendto(strIp,pSend,tSendLen);
}

void CUDPSERVER::_UpDateServer(unsigned char *buf1)
{
    QString Ip;
    Ip = QString(buf1[28])+":"+QString(buf1[29])+":"+QString(buf1[30])+":"+QString(buf1[31]);
    unsigned char pSend[35] = {0};
    memcpy(pSend,buf1,42);
    int tSendLen = 42;
    _Sendto(Ip,pSend,tSendLen);
}

//服务器->客户端
void CUDPSERVER::SlotUdpserTocli(QString strIp,unsigned char *pSend,int nSendlen)
{
    int tSendLen = nSendlen+4;
    unsigned char RSend[tSendLen];
    memcpy(RSend,0,tSendLen);
    memcpy(RSend,pSend,nSendlen);
    pSend[nSendlen] = (tSendLen>>8)&0xFF;
    pSend[nSendlen+1] = tSendLen&0xFF;
    int CRC = _CRCvelocity(pSend,tSendLen-2);
    pSend[nSendlen+2] = (CRC>>8)&0xFF;
    pSend[nSendlen+3] = CRC&0xFF;
    _Sendto(strIp,RSend,tSendLen);
    _Sendto(strIp, pSend, nSendlen);
}

//智能家居中转 & 智能家居一键查询
void CUDPSERVER::SigUdpCliToSer(QString strIp,unsigned char *pSend,int nSendlen)
{
    unsigned char RSend[nSendlen];
    memcpy(RSend,0,nSendlen);
    memcpy(RSend,pSend,nSendlen);
    pSend[nSendlen-4] = (nSendlen>>8)&0xFF;//设置长度位
    pSend[nSendlen-3] = nSendlen&0xFF;
    int CRC = _CRCvelocity(pSend,nSendlen-2);
    pSend[nSendlen-2] = (CRC>>8)&0xFF;//设置校验位
    pSend[nSendlen-1] = CRC&0xFF;
    _Sendto(strIp,RSend,nSendlen);
}

//60 智能家居一键查询---接收
void CUDPSERVER::Smartfurniture(unsigned char *buf,char *pFromIP)
{
    qDebug()<<"一键查询："<<QString((char *)buf)<<QString(pFromIP);
    QString IP = QString(buf[28]) + "." + QString(buf[29]) + "." + QString(buf[30]) + "." + QString(buf[31]);
    int lampNum = QString(buf[33]).toInt()*10 + QString(buf[34]).toInt();
    qDebug()<<"IP地址为："<<IP<<"灯的数量："<<lampNum;
}

//61 智能家居中转---接收
void CUDPSERVER::SmaetTransfer(unsigned char *buf, char *pFromIP)
{
    qDebug()<<"家居控制"<<QString((char *)buf)<<QString(pFromIP);
    QString strID = QString(buf[38]) + QString(buf[39]);
    qDebug()<<"园区编号为："<<strID;
}

//右键---智能家居
void CUDPSERVER::SlotDragSmart(QString Addr, QString strIp)
{
    QString IP = strIp;
    unsigned char send[36] = {0};
    memcpy(send, m_gHead, 6);
    send[6] = 60;   //命令60，智能家居一键查询
    send[7] = 1;    //命令类型1

    memcpy(send+8, Addr.toUtf8().data(), 20);//设备地址---地址编码

    QString str;
    str.clear();
    str = strIp.left(strIp.indexOf('.'));
    send[28] = str.toInt();

    str.clear();
    strIp = strIp.right(strIp.length() - strIp.indexOf('.')-1);
    str = strIp.left(strIp.indexOf('.'));
    send[29] = str.toInt();

    str.clear();
    strIp = strIp.right(strIp.length() - strIp.indexOf('.')-1);
    str = strIp.left(strIp.indexOf('.'));
    send[30] = str.toInt();

    str.clear();
    strIp = strIp.right(strIp.length() - strIp.indexOf('.')-1);
    str = strIp;
    send[31] = str.toInt();

    int tSendLen = 36;
    send[32] = (tSendLen>>8)&0xFF;
    send[33] = tSendLen&0xFF;
    int CRC = _CRCvelocity(send, tSendLen-2);
    send[34] = (CRC>>8)&0xFF;
    send[35] = CRC&0xFF;
    qDebug()<<"***coming***"<<QString((char *)send)<<QString(send[28]);
    _Sendto(IP, send, tSendLen);
}

/******测试函数*******/
void CUDPSERVER::SlotRevMessage(QString Addr, QString strIp, QString DeviceType, QString RoomCode, QString DeviceCode, QString ControlType, QString ControlCode)
{
    unsigned char send[42] = {0};
    memcpy(send, m_gHead, 6);   //包头

    send[6] = 61;
    send[7] = 1;

    QString strAddr = Addr;     //转换后的设备地址
    memcpy(send+8, strAddr.toAscii().data(), 20);

    QString IP = strIp;

    QString str;    //IP地址解析
    str.clear();
    str = strIp.left(strIp.indexOf('.'));
    send[28] = str.toInt();

    str.clear();
    strIp = strIp.right(strIp.length() - strIp.indexOf('.')-1);
    str = strIp.left(strIp.indexOf('.'));
    send[29] = str.toInt();

    str.clear();
    strIp = strIp.right(strIp.length() - strIp.indexOf('.')-1);
    str = strIp.left(strIp.indexOf('.'));
    send[30] = str.toInt();

    str.clear();
    strIp = strIp.right(strIp.length() - strIp.indexOf('.')-1);
    str = strIp;
    send[31] = str.toInt();

    send[32] = DeviceType.toInt();      //设备类型
    send[33] = RoomCode.toInt();        //房间编号
    send[34] = DeviceCode.toInt();      //房内设备编号

    send[35] = ControlType.toInt();     //控制类型
    send[36] = 0;                       //控制数据
    send[37] = ControlCode.toInt();

    int len  = 42;
    send[38] = (len>>8)&0xFF;
    send[39] = len&0xFF;
    int CRC  = _CRCvelocity(send, len-2);
    send[40] = (CRC>>8)&0xFF;
    send[41] = CRC&0xFF;
    QString test = QString((char *)send);
    qDebug()<<"测试："<<test.length()<<test;
    _Sendto(IP, send, len);
}

/******信号槽函数：发送---注销所有员工*****/
void CUDPSERVER::SlotDeleteAllStaff()
{
    qDebug()<<"信号槽函数：注销所有员工";

    NodeDevice *p = m_DeviceItem;
    while(p->next)      //注销所有设备上的所有员工信息
    {
        unsigned char send[32] = {0};
        memcpy(send, m_gHead, 6);
        send[6] = 85;
        send[7] = 1;

        memcpy(send+8, p->data.gcIntraAddr, 20);

        int len = 32;
        send[28] = (len>>8)&0xFF;
        send[29] = len&0xFF;

        int CRC = _CRCvelocity(send, len-2);
        send[30] = (CRC>>8)&0xFF;
        send[31] = CRC&0xFF;
        _Sendto(QString(p->data.gcIpAddr), send, len);
        p = p->next;
    }
}

/******信号槽函数：R注销所有员工*****/
void CUDPSERVER::_DeleteAllStaff()
{
    emit SigRecvDeleteStaff();
}
