#ifndef UDPSEND_H
#define UDPSEND_H

#include <QThread>
#include <windows.h>
#include <Winsock2.h>
#include <WS2TCPIP.h>
#include <QtNetwork>
#include <QTimer>
#include <udpserver.h>
#define  SEND_TIME 60*10
class CUDPSEND : public QThread
{
    Q_OBJECT
public:
    explicit CUDPSEND(QObject *parent = 0);
    CUDPSERVER *Udpserver;

protected:
    void run();

signals:
    void SigUdpOpenFail();
    void SigUdpserTocli(QString strIp,unsigned char *pSend,int nSendlen);

public slots:
    void SendIdToServer();
    void SlotUpdcliToser(QString strIp,unsigned char *pSend,int nSendlen);

private :   
    int  _Read();
    void _Recv(unsigned char *buf,char *pFromIP,int nRecvLen);
    int _Sendto(QString strIp,unsigned char *send,int nSendlen);//发送
    int _CRCvelocity(BYTE far *pucMsg,BYTE ucMsgLen);
    SOCKET Fsocket;
    WSADATA wsd;
    struct sockaddr_in To;
    char m_gHead[6];
};
#endif // UDPSEND_H
