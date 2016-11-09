#ifndef PERSONNEL_H
#define PERSONNEL_H
/*******************************
  人事主界面，加载在园区办公界面上
  ********************************/
#include <QWidget>
#include <QVBoxLayout>
#include "company.h"
#include "department.h"
#include "staff.h"
#include "dimission.h"
namespace Ui {
    class CPERSONNEL;
}

class CPERSONNEL : public QWidget
{
    Q_OBJECT

public:
    explicit CPERSONNEL(QWidget *parent = 0);
    ~CPERSONNEL();
    void _SetInit();
    void _UpdateToStaff();
    int pbtnAllSheet;

private slots:
    void SlotFocusOut();
//    void on_QpbtnParameter_clicked();

private slots:
    void SlotStaff_Staff_Leave(int Id);
    void SlotClose();
    void SlotPersonnelClick();
    void SlotDeleteAll();   //删除所有员工
    void SlotRecvDeleteAllStaff();

    void on_QType_2_clicked(const QModelIndex &index);
    void on_QpbtnCompany_clicked();
    void on_QpbtnDepartment_clicked();
    void on_QpbtnStaff_clicked();
    void on_QpbtnDimission_clicked();

signals:
    void SigPersonnel_Staff_Leave(int Id);
    void SigFocusOut();
    void SigDeleteAllStaff();   //删除所有员工
    void SigRecvDeleteAll();

private:
    Ui::CPERSONNEL *ui;
    QVBoxLayout *layout;
    CCOMPANY *Company;
    CDEPARTMENT *Department;
    CSTAFF *Staff;
    CDIMISSION *Dimission;

private:
    void InitType();
};

#endif // PERSONNEL_H
