#ifndef UDPCLIENT_H
#define UDPCLIENT_H
#include <QThread>
#include <windows.h>
#include <Winsock2.h>
#include <WS2TCPIP.h>
#include <QtNetwork>
#include <QTimer>
#include <udpserver.h>
class udpclient
{
public:
    udpclient();
    CUDPSERVER *Udpserver;
protected:
    void run();
signals:
    void SigUdpOpenFail();
    void SigUdpserTocli(QString strIp,unsigned char *pSend,int nSendlen);
public slots:
    void _SendIdToServer();
    void SlotUpdcliToser(QString strIp,unsigned char *pSend,int nSendlen);
private :
    SOCKET Fsocket;
    WSADATA wsd;
    struct sockaddr_in To;
    char m_gHead[6];
    int  _Read();
    void _Recv(unsigned char *buf,char *pFromIP,int nRecvLen);

    int _Sendto(QString strIp,unsigned char *send,int nSendlen);//发送
    int _CRCvelocity(BYTE far *pucMsg,BYTE ucMsgLen);
};

#endif // UDPCLIENT_H
