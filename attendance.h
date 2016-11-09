#ifndef ATTENDANCE_H
#define ATTENDANCE_H
/*******************************
  考勤主界面，加载在园区办公界面上
  ********************************/
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QModelIndex>
#include "attendanceset.h"
#include "original.h"
#include "day.h"
#include "detail.h"
#include "monthly.h"
#include "askforleave.h"
#include "mysql.h"
#include "struct.h"
#include "dragwindows.h"
namespace Ui {
    class CATTENDANCE;
}

class CATTENDANCE : public QWidget
{
    Q_OBJECT

public:
    explicit CATTENDANCE( QWidget *parent = 0);
    ~CATTENDANCE();

signals:
    void SigFillcardDate(QDate fillcardDate);
    void SigFocusOut();

    void SigAcquisition(QString, QString);

private slots:
    void SlotDelete();
    void SlotSetHoliday();
    void SlotDelHoliday();
    void SlotAcquisition();
    void SlotProgress(int AllRacdNum,int number);
    void SlotFillout();
    void SlotFillcardRestType(QString strType);
    void SlotAttendanceClick();
    void SlotFocusOut();
    void SlotReturnMenu();

    void on_toolBox_2_currentChanged(int index);
    void on_QpbtnAttendanceSet_clicked();
    void on_QpbtnHolidaysSet_clicked();
    void on_QpbtnManually_clicked();
    void on_QpbtnLeave_clicked();
    void on_QpbtnCollect_clicked();
    void on_QpbtnOriginal_clicked();
    void on_QpbtnDetail_clicked();
    void on_QpbtnDay_clicked();
    void on_QpbtnMonthly_clicked();
    void on_QType_3_clicked(const QModelIndex &index);
    void on_QType_4_clicked(const QModelIndex &index);
    void on_FullCardDate_customContextMenuRequested(const QPoint &pos);
    void on_HolidayCalendarWidget_customContextMenuRequested(const QPoint &pos);
    void on_tableWidget_customContextMenuRequested(const QPoint &pos);
    void on_Fillcard_tree_customContextMenuRequested(const QPoint &pos);
    void on_FilloutCalendarWidget_clicked(const QDate &date);

    void on_QchkStime_clicked();
    void on_QchkEtime_clicked();
    void CheckSelf(QTreeWidgetItem*,int);

private:
    Ui::CATTENDANCE *ui;
    QVBoxLayout *layout;
    QHBoxLayout *Hlayout;
    QAction *DeleteAction;
    QAction *Holiday;
    QAction *DeleteHoliday;
    QAction *FilloutAction;
    QAction *deleteAction;
    QMenu *CardDataMenu;
    QMenu *HolidayMenu;
    QMenu *DelHolidayMenu;
    QMenu *FilloutMneu;
    QMenu *MapMenu;

    CORIGINAL *Original;
    CDAY *Day;
    CDETAIL *Detail;
    CMONTHLY *Monthly;
    askforleave *Askforleave;
    CATTENDANCESET *AttendanceSet;

private:
    void _InitTree3();
    void _InitTree4();
    void _InitFillcardTree();
    void _InitFullCardDate();
    void _InitDoorMachine();
    void _UpdateFullCardDate();
    void _UpdateDoorMachine();
    void _InitHolidayDate();
    void _UpdateHolidayDate();
    void _InitCardList();
    void _InitDray();
    void _SetToolBoxCurrentItem(int nIndex);
    int CardType;
    int HolidayTypeNum;//节假日设置的数量
    bool isholiday;    //补卡时判断当前时间是否为节假日的标志
    bool istakeworks;  //补卡时判断当前时间是否正常上班时间的标志
    QString restType;
    QDate currselecteDate;
    QList<SList> m_List;
    QList<QDate> n_List;
    QList<SFullCard> m_FullCard;
};

#endif // ATTENDANCE_H
