#ifndef EQUIPMENT_H
#define EQUIPMENT_H
/*******************************
  设备主界面，加载在园区办公界面上
  ********************************/
#include <QWidget>
#include <QModelIndex>
#include <QVBoxLayout>
#include "struct.h"
#include "udpserver.h"
#include "smartcontrol.h"
#include "dragsmartcontorl.h"
#include "mysql.h"

namespace Ui {
    class equipment;
}

class equipment : public QWidget
{
    Q_OBJECT

public:
    explicit equipment(NodeDevice *device, NodeAlarm *m_AlarmItem, QWidget *parent = 0);
    ~equipment();
    void _UpdateDevice(NodeDevice *ItemDevice);
    void _UpdateAlarm(NodeAlarm *ItemAlarm);
    NodeDevice *m_Device;
    NodeAlarm  *m_Alarm;

private slots:
    void SlotDragSmartControl();    //
    void SlotInitPass();
    void SlotDelete();
    void SlotPing();
    void SlotReset();
    void SlotRegister();
//    void SlotInitDevice(NodeDevice *Device);
    void SlotInitDevice();
    void SlotUpdateDevice(QVariant dataVar);
    void SlotOnlineDevice(int nId,int Alarm);
    void SlotDeal();
    void SlotInsertAlarm(NodeDevice *Device, NodeAlarm *Alarm);
    void SlotSendLamp(char *buf,QString strIp);
    void SlotEquipmentClick();
    void SlotFocusOut();
    void SlotMessageFromDrag(QString, QString, QString, QString, QString); //测试
    void SlotRecvInitPass(QString strAddr,int nFlag);//接收到初始化密码的回复

    void on_QType_clicked(const QModelIndex &index);
    void on_QDevice_customContextMenuRequested(const QPoint &pos);
    void on_QAlarm_customContextMenuRequested(const QPoint &pos);
    void on_QpbtnS_clicked();
    void on_QpbtnM_clicked();
    void on_QpbtnZ_clicked();
    void on_QpbtnAll_clicked();

signals:
    void SigUdpSendPass(QString strAddr, QString strMac, QString strIP);
    void SigUdpReset(QString strIp, QString strAddr, QString strType);
    void SigUdpRegister(QString strAddr, QString strIp);
    void SigUdpSendLamp(char *buf, QString strIp);
    void SigUdpserTocli(QString strIp, unsigned char *send, int len);
    void SigDragSmart(QString Addr, QString IP);

    void SigMessage(QString, QString, QString, QString, QString, QString, QString);  //测试

    void SigCancleAlarm();
    void SigCancleAlarm2();

private:
    Ui::equipment *ui;

    CMYSQL mysql;
    dragsmartContorl *dragsmart;    //智能家居
    QAction *RegisterAction;
    QAction *DeleteAction;
    QAction *ResetAction;
    QAction *PingAction;
    QAction *InitPassAction;
    QAction *SmartControlAction;
    QAction *DragSmartControlAction;
    QAction *DealAction;
    QMenu *DeviceMenu;
    QMenu *AlarmMenu;

private:
    void _InitTree();
    void _InitDevice();
    void _InitAlarm();
//    void _InitDray();
    QString _AddrExplainToAddr(QString AddrExplain,QString strType);
    int deviceType;
};

#endif // EQUIPMENT_H
