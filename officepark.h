#ifndef OFFICEPARK_H
#define OFFICEPARK_H

#include <QMainWindow>
#include "basicwindows.h"
#include "personnel.h"
#include "attendance.h"
#include "equipment.h"
#include "udpserver.h"
//#include "user.h"
#include "system.h"
#include "find.h"
#include "delete.h"
#include "deal.h"
#include "about.h"
#include "dropshadowwidget.h"
#include <QPainter>
#include <QPixmap>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QModelIndex>
#include <QSystemTrayIcon>
#include <QBitmap>
#include <QDebug>
#define SHADOW_WIDTH            5
#define MAX_LENGTH  30      //每页记录的显示数量

namespace Ui {
    class COFFICEPARK;
}

class COFFICEPARK : public basicWindows
{
    Q_OBJECT

public:
    explicit COFFICEPARK(SUser sUser,basicWindows *parent = 0);
    ~COFFICEPARK();
    int ptbnNum;
    void paintEvent(QPaintEvent *event);
    void _UpdateLoading(SUser sUser);

private slots:
    void on_QpbtnShut_clicked();
    void on_QpbtnAbout_clicked();
    void on_QpbtnMin_clicked();
    void on_QType_clicked(const QModelIndex &index);
    void on_QpbtnEquipment_clicked();
    void on_QpbtnPersonnel_clicked();
    void on_QpbtnAttendance_clicked();
    void on_QpbtnSystem_clicked();
    void on_LogTable_clicked(const QModelIndex &index);
    void on_QpbtnRecord_clicked();
    void SlotFind(int nIndex,int nTime,QString strFind,QString strStime,QString strEtime);
    void IconIsActived(QSystemTrayIcon::ActivationReason reason);

private slots:
    void SlotUdpSendPass(QString,QString,QString);
    void SlotUdpReset(QString,QString,QString);
    void SlotUdpRegister(QString,QString);
    void SlotUdpSendLamp(char*,QString);
    void SlotAcquisition(QString, QString);     //考勤数据采集
    void SlotReset();
    void SlotUpdateAlarm();

    void ToInitDevice();
    void ToInsertAlarm();
    void CancleAlarm();
    void CancleAlarm2();

signals:
    void SigInitEquipmentDevice();
    void SigUpdateEquipmentDevice(QVariant);
    void SigOnlineEquipmentDevice(int,int);
    void SigInitEquipmentAlarm();

    void SigInitDevice2(NodeDevice *);
    void SigInsertAlarm2(NodeDevice*, NodeAlarm *);
    void SigCancleAlarm(NodeDevice*);   //更新取消设备链表
    void SigCancleAlarm2(NodeAlarm *);

signals:
    void SigLoading();
    void Sigquit();
    void SigPersonnelClick();
    void SigAttendanceClick();
    void SigEquipmentClick();
    void SigEquipmentFocusOut();
    void SigPersonnelFocusOut();
    void SigAttendanceFocusOut();
    void SigToServerProcotol(int);

private slots:
    void SlotFindRecord();//查找响应函数
    void SlotDeleteRecord();//删除响应函数
    void SlotUdpOpenFail();//udp启动失败
    void SlotDeleteSuc();//删除成功反馈
    void SlotPersonnel_Staff_Leave(int Id);
    void SlotDeal();
    void SetDealInformation();
    void SlotDealInformation(QString whetherMesIfo,QString dealHuman,QString Messages);
    void SlotRecvLogOut();
    void SlotSeleteUser(QString user);
    void SlotLogOut();
    void SlotNextPage();    //下一页
    void SlotPrevPage();    //上一页
//    void SlotRestoreAction();

    void on_QAlarmRecord_customContextMenuRequested(const QPoint &pos);
    void on_QDeviceRecord_customContextMenuRequested(const QPoint &pos);
    void on_QSystem_customContextMenuRequested(const QPoint &pos);
    void on_QpbtnHelp_clicked();
    void on_QIcon_clicked();
    void on_centralwidget_customContextMenuRequested(const QPoint &pos);
    void on_QpbtnAlarm_clicked();
    void on_QpbtnDevice_clicked();
    void on_QpbtnSystem_2_clicked();

private:
    Ui::COFFICEPARK *ui;

    CFIND *CFind;
    CDELETE *CDelete;
    CPERSONNEL *Personnel;
    CATTENDANCE *Attendance;
    equipment *Equipment;
    CUDPSERVER *CUdpServer;
    CSYSTEM *System;
    SUser m_sUser;
    deal *Deal;
    about *About;

    QVBoxLayout *layout;
    QSystemTrayIcon *TrayIcon;
    QMenu *TrayIconMenu;
    QAction *MinimizeAction;
    QAction *MaximizeAction;
    QAction *RestoreAction;
    QAction *TrayQuitAction;
    QAction *NextPage;      //下一页
    QAction *PrevPage;      //上一页

    QMenu *SystemRecordMenu;
    QMenu *AlarmRecordMenu;
    QMenu *DeviceRecordMenu;
    QMenu *CentralwidgetMenu;
    QAction *FindRecordAction;
    QAction *DeleteRecordAction;
    QAction *DealAction;
    QAction *Logout;
    QMenu *Menu_pallet;
    QAction *Action_min;
    QAction *Action_restore;
    QAction *Action_quit;
    QAction *Action_cancel;

private:
    void _InitTray();       //托盘初始化
    void _InitLogClass();
    void _InitSystem();
    void _UpdateSystem(QString SQL);
    void _InitAlarmRecord();
    void _UpdateAlarmRecord(QString SQL);
    void _InitDeviceRecord();
    void _UpdateDeviceRecord(QString SQL);
    void _InitPrivilegeClass();
    void SetAllButtenSheet(int m_ptbnNum);
    void resizeWidget();
    int curPos;
    int m_nRecord;      //日志类型：1.报警日志 2.设备日志 3.系统日志
    int AlarmCurPage;    //当前显示的页
    int DeviceCurPage;
    int SystemCurPage;
    int m_nCount;
    QPoint pLast;
    QString FindSQL;
    SAlarmDevice sAlarmDevice;  //警报设备结构体（包含一些警报信息）
    QMouseEvent *event;
};

#endif
