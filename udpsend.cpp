#include "udpsend.h"

CUDPSEND::CUDPSEND(QObject *parent) :
    QThread(parent)
{
    memcpy(m_gHead,"XXXCID",6);
    QTimer *Timer1 = new QTimer();  //定时器???
    connect(Timer1,SIGNAL(timeout()), this, SLOT(SendIdToServer()) );
    Timer1->start(10000);
    Udpserver = new CUDPSERVER("m_sUser.strCityNum");
    Udpserver->start();
    connect(this,SIGNAL(SigUdpserTocli(QString,unsigned char *,int)),Udpserver,SLOT(SlotUdpserTocli(QString,unsigned char *,int)));
//    connect(Udpserver,SIGNAL(SigUdpCliToSer(QString,unsigned char *,int)),this,SLOT(SlotUpdcliToser(QString,unsigned char *,int)));
}

//8301到8300
void CUDPSEND::SendIdToServer()
{
    unsigned char pSend[14]={0};
    memcpy(pSend,m_gHead,6);
    pSend[6] = 90;
    pSend[7] = 1;
    int Id = 29;
    pSend[8] = (Id>>8)&0xFF;
    pSend[9] = Id&0xFF;
    int nSendLen = 14;
    pSend[10]=(nSendLen>>8)&0xFF;
    pSend[11]=nSendLen&0xFF;
    int CRC = _CRCvelocity(pSend,nSendLen-2);
    pSend[12] = (CRC>>8)&0xFF;
    pSend[13] = CRC&0xFF;
    QString strIp = "192.168.0.25";
    _Sendto(strIp,pSend,nSendLen);
}

//校验
int CUDPSEND::_CRCvelocity(BYTE far *pucMsg,BYTE ucMsgLen)
{
        int wCRC = 0xffff;
        int nPos=0;
        while( ucMsgLen-- )
        {
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
        wCRC = (wCRC%0x100)*0x100+(wCRC-wCRC%0x100)/0x100;
        return (wCRC);
}

void CUDPSEND::run()
{
    //进行版本协商，加载特定版本的socket动态链接库
    int retVal;
    int port = 8301;
    if((retVal = WSAStartup(MAKEWORD(2,2), &wsd)) != 0)
    {
        perror("socket");
        return ;
    }
    Fsocket = socket(PF_INET,SOCK_DGRAM,IPPROTO_IP);
    if(Fsocket == INVALID_SOCKET)
    {
        emit SigUdpOpenFail();
        perror("socket");
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
        emit SigUdpOpenFail();
        perror("socket");
        return;
    }

    while(1)
    {
        _Read();
    }
}

int CUDPSEND::_Sendto(QString strIp,unsigned char *pSend,int nSendlen)
{
    unsigned long nAddr=inet_addr(strIp.toStdString().data());
    To.sin_family=AF_INET;
    To.sin_port=htons(8300);
    To.sin_addr.S_un.S_addr=(int)nAddr;
    qDebug()<<"发送的数据为："<<QString((char *)pSend);
    if(::sendto(this->Fsocket,(char*)pSend,nSendlen,0,(struct sockaddr*)&To,sizeof(struct sockaddr))==SOCKET_ERROR)
    {
        qDebug() <<WSAGetLastError();
        return 0;
    }
    return 1;
}

//接收到的数据
int CUDPSEND::_Read()
{
    struct sockaddr_in from;
    int nLength = sizeof(struct sockaddr_in);
    unsigned char buf[8100] = {0};//8100有特殊含义？？？
    char gFromIp[20] = {0};
    int t_nRecvlen = ::recvfrom(this->Fsocket,(char*)buf,8100,0,(struct sockaddr * )&from,&nLength);
    if(!t_nRecvlen)//如果正确接收返回接收到的字节数，失败返回-1
    {
        qDebug() <<WSAGetLastError();
        return 0;
    }
    strcpy(gFromIp,inet_ntoa(from.sin_addr));
    qDebug()<<"接受到的数据为："<<QString((char *)buf);
    _Recv(buf,gFromIp,t_nRecvlen);
    return t_nRecvlen;
}

void CUDPSEND::_Recv(unsigned char *buf,char *pFromIP,int nRecvLen)
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
                        SigUdpserTocli(strIp,pSend,tSendLen);
                    }
                    break;

                    case 60:
                    {
                        QString strIp=QString(buf[28])+":"+QString(buf[29])+":"+QString(buf[30])+":"+QString(buf[31]);;
                        unsigned char pSend[36] = {0};
                        memcpy(pSend,buf,36);
                        int tSendLen =36;
                        SigUdpserTocli(strIp,pSend,tSendLen);
                    }
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

void CUDPSEND::SlotUpdcliToser(QString strIp,unsigned char *pSend,int nSendlen)
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

}
