#ifndef OFFICEPARKS_H
#define OFFICEPARKS_H

#include <QMainWindow>
#include <QModelIndex>
#include <QHBoxLayout>
#include "personnel.h"
#include "udpserver.h"
#include "attendance.h"
#include "smartcontrol.h"
#include "user.h"
#include "system.h"
#include "delete.h"
#include "find.h"
namespace Ui {
    class COFFICEPARKS;
}

class COFFICEPARKS : public QMainWindow
{
    Q_OBJECT

public:
    explicit COFFICEPARKS(SUser sUser,QWidget *parent = 0);
    ~COFFICEPARKS();
    void closeEvent( QCloseEvent * event );
signals:
    void SigLoading();
    void Sigquit();
private slots:
    void SlotFindRecord();
    void SlotDeleteRecord();
    void SlotUdpOpenFail();
    void SlotSmartControl();
    void SlotInitPass();
    void SlotDelete();
    void SlotPing();
    void SlotReset();
    void SlotRegister();
    void SlotInitDevice();
    void SlotUpdateDevice(QVariant dataVar);
    void SlotPersonnel_Staff_Leave(int Id);
    void SlotOnlineDevice(int nId,int Alarm);
    void SlotDeal();
    void SlotInsertAlarm();
    void SlotRecvLogOut();
    void SlotSendLamp(char *buf,QString strIp);
private slots:
    void on_QpbtnEquipment_clicked();
    void on_QpbtnPersonnel_clicked();
    void on_QpbtnAttendance_clicked();
    void on_QpbtnSystem_clicked();
    void on_QpbtnRecord_clicked();
    void on_QDevice_customContextMenuRequested(const QPoint &pos);

    void on_QpbtnAlarm_clicked();
    void on_QpbtnDeviceRecord_clicked();
    void on_QpbtnSystemRecord_clicked();

    void on_QAlarm_customContextMenuRequested(const QPoint &pos);

    void on_QAlarmRecord_customContextMenuRequested(const QPoint &pos);

    void on_QSystem_customContextMenuRequested(const QPoint &pos);

    void on_QDeviceRecord_customContextMenuRequested(const QPoint &pos);

private:
    Ui::COFFICEPARKS *ui;
    SUser m_sUser;
    int m_nRecord;
    CFIND *CFind;
    CDELETE *CDelete;
    CUDPSERVER *CUdpServer;
    CSMARTCONTROL *CSmartControl;
    CSYSTEM *CSystem;
    QVBoxLayout *layout;
    CPERSONNEL *Personnel;
    CATTENDANCE *Attendance;

    QSystemTrayIcon *TrayIcon;
    QMenu *TrayIconMenu;
    QAction *MinimizeAction;
    QAction *MaximizeAction;
    QAction *RestoreAction;
    QAction *TrayQuitAction;

    QMenu *SystemRecordMenu;
    QMenu *AlarmRecordMenu;
    QMenu *DeviceRecordMenu;
    QAction *FindRecordAction;
    QAction *DeleteRecordAction;

    QMenu *DeviceMenu;
    QAction *RegisterAction;
    QAction *DeleteAction;
    QAction *ResetAction;
    QAction *PingAction;
    QAction *InitPassAction;
    QAction *SmartControlAction;

    QMenu *AlarmMenu;
    QAction *DealAction;

    void _InitTray();
    void _InitAlarm();
    void _InitDevice();
    void _InitSystem();
    void _InitAlarmRecord();
    void _InitDeviceRecord();


    void _UpdateDevice(NodeDevice *ItemDevice);
    void _UpdateAlarm(NodeAlarm *ItemAlarm);
    void _UpdateAlarmRecord();
    void _UpdateDeviceRecord();
    void _UpdateSystemRecord();
    QString _AddrExplainToAddr(QString AddrExplain,QString strType);
};

#endif // OFFICEPARKS_H
