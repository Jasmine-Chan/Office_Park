#include "udpclient.h"
#include <QMessageBox>
#include"mysql.h"
#include <QDebug>
udpclient::udpclient()
{
    memcpy(m_gHead,"XXXCID",6);
    QTimer *Timer1 = new QTimer();
   // connect(Timer1,SIGNAL(timeout()), this, SLOT(_SendIdToServer()) );
    Timer1->start(10000);
    Udpserver = new CUDPSERVER("m_sUser.strCityNum");
    Udpserver->start();
//    connect(this,SIGNAL(SigUdpserTocli(/*QString,unsigned char *,int*/)),Udpserver,SLOT(SlotUdpserTocli(/*QString,unsigned char *,int*/)));
//    connect(Udpserver,SIGNAL(SigUdpcliToser(/*QString,unsigned char *,int*/)),this,SLOT(SlotUpdcliToser(/*QString,unsigned char *,int*/)));
}

void udpclient::_SendIdToServer()
{
    unsigned char pSend[14]={0};
    memcpy(pSend,m_gHead,6);
    pSend[6]=90;
    pSend[7]=1;
    int Id=29;
    pSend[8]=(Id>>8)&0xFF;
    pSend[9]=Id&0xFF;
    int nSendLen = 14;
    pSend[10]=(nSendLen>>8)&0xFF;
    pSend[11]=nSendLen&0xFF;
    int CRC = _CRCvelocity(pSend,nSendLen-2);
    pSend[12] = (CRC>>8)&0xFF;
    pSend[13] = CRC&0xFF;
    QString strIp = "192.168.0.25";
    _Sendto(strIp,pSend,nSendLen);
}

int udpclient::_CRCvelocity(BYTE *pucMsg, BYTE ucMsgLen)
{
    int wCRC = 0xffff;
    int nPos=0;
    while( ucMsgLen-- )
    {
//                qDebug()<<*pucMsg<<ucMsgLen<<wCRC;
            wCRC ^= *pucMsg++;
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
//        qDebug()<<wCRC;
    wCRC = (wCRC%0x100)*0x100+(wCRC-wCRC%0x100)/0x100;
    return (wCRC);
}

int udpclient::_Sendto(QString strIp, unsigned char *pSend, int nSendlen)
{
    unsigned long nAddr=inet_addr(strIp.toStdString().data());
    To.sin_family=AF_INET;
    To.sin_port=htons(8301);
    To.sin_addr.S_un.S_addr=(int)nAddr;
    if(::sendto(this->Fsocket,(char*)pSend,nSendlen,0,(struct sockaddr*)&To,sizeof(struct sockaddr))==SOCKET_ERROR)
    {
        qDebug() <<WSAGetLastError();
        return 0;
    }
    return 1;
}

void udpclient::run()
{
    int retVal;
    int port = 8300;
    if((retVal = WSAStartup(MAKEWORD(2,2), &wsd)) != 0)
    {
        perror("socket");
//        qDebug() << "WSAStartup() failed";
        return ;
    }
    Fsocket = socket(PF_INET,SOCK_DGRAM,IPPROTO_IP);
    if(Fsocket == INVALID_SOCKET)
    {
   //     emit SigUdpOpenFail();
        perror("socket");
//        qDebug()<<WSAGetLastError();
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
 //       emit SigUdpOpenFail();
        perror("socket");
//        qDebug() <<WSAGetLastError();
        return;
    }

    while(1)
    {
        _Read();
    }
}

int udpclient::_Read()
{
    struct sockaddr_in from;
    int nLength=sizeof(struct sockaddr_in);
    unsigned char buf[8100] = {0};
    char gFromIp[20] = {0};
    int t_nRecvlen =::recvfrom(this->Fsocket,(char*)buf,8100,0,(struct sockaddr * )&from,&nLength);
    if(!t_nRecvlen)
    {
//        qDebug() <<WSAGetLastError();
        return 0;
    }
    strcpy(gFromIp,inet_ntoa(from.sin_addr));
    _Recv(buf,gFromIp,t_nRecvlen);
    return t_nRecvlen;
}

void udpclient::_Recv(unsigned char *buf, char *pFromIP, int nRecvLen)
{
    qDebug()<<nRecvLen<<"接收"<<memcmp(buf,m_gHead,6);
    if((memcmp(buf,m_gHead,6) == 0) && (nRecvLen == (buf[nRecvLen-4]*256 + buf[nRecvLen-3]))&&
         (_CRCvelocity(buf,nRecvLen-2) == (buf[nRecvLen-2]*256 + buf[nRecvLen-1])))
    {
        switch(buf[7])
        {
        case 1:
            switch(buf[6])
            {
                case 6:
                {
                    QString strIp=QString(buf[28])+":"+QString(buf[29])+":"+QString(buf[30])+":"+QString(buf[31]);;
                    unsigned char pSend[42] = {0};
                    memcpy(pSend,buf,42);
                    int tSendLen =42;
                    SigUdpserTocli(/*strIp,pSend,tSendLen*/);
                }
                    break;

            }
            break;
        case 2:
            switch(buf[6])
            {
                case 90:
                    break;
            }
            break;
        default:
            break;
        }
    }
}

void udpclient::SlotUpdcliToser(/*QString strIp,unsigned char *pSend,int nSendlen*/)
{
//    _Sendto(strIp,pSend,nSendlen);
}






