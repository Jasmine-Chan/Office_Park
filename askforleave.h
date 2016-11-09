#ifndef ASKFORLEAVE_H
#define ASKFORLEAVE_H
/*******************************
  请/休假界面，加载在考勤主界面上
  ********************************/
#include <QWidget>
#include <QSqlQuery>
#include <qdebug.h>
#include <QAction>
#include <QMenu>
#include <qmessagebox.h>
#include <pseudomonas.h>
#include <QTextCharFormat>
#include <QAction>
#include <QGridLayout>
#include <struct.h>

namespace Ui {
    class askforleave;
}

class askforleave : public QWidget
{
    Q_OBJECT

public:
    explicit askforleave(QWidget *parent = 0);
    ~askforleave();

    void _InitLeaveTree();

signals:
    void SigLeaveDate(QDate nLeaveDate, SStaff staff);

private slots:
    void SlotDelete();
    void SlotShowPseudomonas();
    void SlotAddSuccess();
    void SlotFocusOut();

    void on_leaveData_customContextMenuRequested(const QPoint &pos);
    void on_Leavetree_customContextMenuRequested(const QPoint &pos);

    void on_QchkSTime_clicked();    //利用系统自带的自动关联功能，按照规定写槽函数名即可
    void on_QchkETime_clicked();

    void CheckSelf(QTreeWidgetItem*,int);

private:
    Ui::askforleave *ui;
    Pseudomonas *m_Pseudomonas;
    QAction *DeleteAction;
    QAction *LeaveAction;
    QMenu *DataMenu;
    QMenu *CalendarMenu;
    QMenu *LeavetreeMenu;

    QList<SList> m_List;
    QList<QDate> n_List;

private:
    void _InitLeaveData();
    void _UpdateLeaveData();
};

#endif // ASKFORLEAVE_H
