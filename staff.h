#ifndef STAFF_H
#define STAFF_H
/*******************************
  员工设置界面，加载在人事主界面上
  ********************************/
#include <QWidget>
#include <windows.h>
#include "mysql.h"
#include "struct.h"
#include "leavestaff.h"

namespace Ui {
    class CSTAFF;
}

class CSTAFF : public QWidget
{
    Q_OBJECT

public:
    explicit CSTAFF(QWidget *parent = 0);
    ~CSTAFF();
    void _Update();
    void _UpdateToStaff();
    int Row;
    int tj;
    void whetherEnabled(int sign);
    void clearLineEdit();

private slots:
    void SlotLeaveStaff(QString strNo);
//    void SlotHeaderClicked(int nColumn);
    void onUserClickListener(const QModelIndex &index);  
    void SlotLogOut();
    void SlotUpdata();
    void SlotDetails();
    void SlotFocusOut();
    void SlotLogOutAll();
    void SlotRecvDeleteAll();
    void SlotAddStaff();

    void on_QpbtnClose_clicked();
    void on_pushButton_clicked();
    void on_Depament_clicked(const QModelIndex &index);
    void on_QStaff_clicked(const QModelIndex &index);
    void on_QpbtnSave_clicked();
    void on_QpbtnLogout_clicked();
    void on_QpbtnRetuen_clicked();
    void on_QpbtnEdit_clicked();
    void on_QStaff_customContextMenuRequested(const QPoint &pos);
    void on_QpbtnChange_clicked();
    void on_QStaffList_customContextMenuRequested(const QPoint &pos);

signals:
    void SigStaff_Staff_Leave(int Id);
    void SigClose();

    void SigDeleteAll();

private:
    Ui::CSTAFF *ui;
    QString m_strDepament_Id;
    CLEAVESTAFF *CLeaveStaff;
    CMYSQL *CMysql;
    QMenu *menu;
    QMenu *menu2;
    QAction *LogOutAction;
    QAction *UpdataAction;
    QAction *Detailed;
    QAction *LogOutAllAction;
    QAction *AddStaffAction;

private:
    void _Change();
    void _InitStaff();
    void _InitStaffList();
    void _UpdateStaff();
    void _UpdateStaffList();
    void _UpdateStaffInformation(QString Depament_Id);
    void _InitDepament();
    void _UpdateDepament();
    void _UpdateConcreteInformation();
    bool _InitStructSStaff();
    void clickDepartmentType(QModelIndex index);
    void _InitDepartmentForQcombobox();
    void _SetFocusOrder();
    int InsertOrUpdate;//判断是插入员工还是更新员工数据的标志 0表示插入 1表示更新
    int isFalseImages;
    QList<SList> m_List;
    QString m_strSql;
    QString Path;
    bool isAddStaff;
    int m_nChangeNoList;

};

#endif // STAFF_H
