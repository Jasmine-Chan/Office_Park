//更新函数文件

#ifndef UDPSERVER_H
#define UDPSERVER_H

#include"Item.h"

#include <QThread>
#include <windows.h>
#include "mysql.h"
#include <Winsock2.h>
#include <QDebug>
#include <WS2TCPIP.h>
#include <QtNetwork>
#include <QtScript/QtScript>
//#include <QtScript/QScriptEngine>
#include <QVariant>
#include <QTimer>

class CUDPSERVER : public QThread
{
    Q_OBJECT

public:
    explicit CUDPSERVER(QString strCityNum,QObject *parent = 0);
    CMYSQL m_Mysql;
    NodeDevice *m_DeviceItem;
    NodeMiddle *m_MiddleItem;
    NodeAlarm *m_AlarmItem;
    SWeather sWeather;
    QString m_strCity;
    int dataNUM;

public:
    void run();
    int  _CRCvelocity(BYTE far *pucMsg,BYTE ucMsgLen);
    void _StaffLeave(int Id);//离职
    void _Acqkuisition(QString Addr,QString IpAddr);    //考勤数据采集

    void _SendPass(QString strAddr,QString strMac,QString strIp);   //管理机初始化密码
    void _Reset(QString strIp,QString strAddr,QString strType);
    void _Register(QString strAddr,QString strIp);
    void _SendLamp(char *pBuf,QString strIp);

signals:
    void SigUdpOpenFail();
    void SigInitDevice();
    void SigDateTime(int nFlag,int nDay);
    void SigUpdateWeatherNew();
    void SigUpdateDevice(QVariant dataVar);
    void SigOnlineDevice(int Id,int Alarm);
//    void SigOnlineDevice(QString IP, int Alarm);
    void SigInsertAlarm();
    void SigRecvLogOut();
    void SigProgress(int AllRacdNum,int Number);
//    void SigUdpCliToSer(QString strIp,unsigned char *pSend,int nSendlen);
    void SigCRCToEquipment(int);
    void SigReset();

    void SigRecvDeleteStaff();
    void SigRecvInitPass(QString, int);

public slots:
    void SlotReadyRead();
    void SlotReadyRead1();
    void _UpdateWeather();
    void SlotUdpserTocli(QString strIp,unsigned char *pSend,int nSendlen);

    void SlotDragSmart(QString Addr, QString IP);       //右键菜单---智能家居
    void SlotRevMessage(QString, QString, QString, QString, QString, QString, QString);    //测试

    void SlotDeleteAllStaff();

    void SlotCancleAlarm(NodeDevice *);
    void SlotCancleAlarm2(NodeAlarm *);

private :
    int number;
    int recordTotal;
    QString PackSQL;
    QString m_strLocalIP,m_strMulticastIP;
    QSqlQuery Packquery;
    char m_gHead[6];
    QNetworkRequest m_NetworkRequest;
    QNetworkReply *m_NetworkReply;
    QNetworkAccessManager *m_NetworkManager;
    QNetworkRequest m_NetworkRequest1;
    QNetworkReply *m_NetworkReply1;
    QNetworkAccessManager *m_NetworkManager1;
    SOCKET Fsocket;
    WSADATA wsd;
    struct sockaddr_in To;

private :
    void _Init();//初始化成员
    void _QueryNowWeather();//天气获取
    void _UpdateLoading(QString strCityNum);
    void _QueryDayWeather();
    void _ReadConfig();//有效期
    void _InitDevice();
    int  _Read();//接收数据
    void _Recv(unsigned char *buf,char *pFromIP);
    int _Sendto(QString strIp,unsigned char *send,int nSendlen);//发送
    //取消报警
    void _CancelAlarm(unsigned char *buf,char *pFromIP);
    void _AskCancelAlarm(unsigned char *buf,char *pFromIP);
    //报警
    void _DeviceAlarm(unsigned char *buf,char *pFromIP);
    void _AskAlarm(unsigned char *buf,char *pFromIP);
    //设备在线
    void _DeviceOnline(unsigned char *buf,char *pFromIP);
    void _AskDeviceOnline(unsigned char *buf,char *pFromIP);
    //注册
    void _RegisterStaff(unsigned char *buf,char *pFromIP);

    //刷卡
    void _SwipeCard(unsigned char *buf,char *pFromIP);
    void _IntraToAddr(QString strIntra,char *pAddr,char *pType);
    //上传门口机的打卡信息
    void _UpInformationCard(unsigned char *buf,QString strIp);
    void _UpPackCard(unsigned char *buf1,QString pFromIP);
    void _UpDateServer(unsigned char *buf1);
    QString _AddrExplainToIntraAddr(QString strExplain);

    void SigUdpCliToSer(QString strIp,unsigned char *pSend,int nSendlen);

    void Smartfurniture(unsigned char *buf,char *pFromIP);  //智能家居一键查询
    void SmaetTransfer(unsigned char *buf, char *pFromIP);  //智能家居中转

    void _DeleteAllStaff();

    //离职反馈
    void _RecvLeaveStaff(unsigned char *buf,char *pFromIp);
    void _RecvInitPass(unsigned char *buf,char *pFromIP);       //接收初始化密码回复
};

#endif // UDPSERVER_H
