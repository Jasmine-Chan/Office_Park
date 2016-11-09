#ifndef DEPARTMENT_H
#define DEPARTMENT_H
/*******************************
  部门设置界面，加载在人事主界面上
  ********************************/
#include <QWidget>
#include "mysql.h"
#include <QAction>
#include <QMenu>
namespace Ui {
    class CDEPARTMENT;
}

class CDEPARTMENT : public QWidget
{
    Q_OBJECT

public:
    explicit CDEPARTMENT(QWidget *parent = 0);
    ~CDEPARTMENT();
    void _SelectStatus(int status);
    void _Update();

private slots:
    void SlotUpdateDepament();
    void SlotDeleteDepament();

    void on_QpbtnSave_clicked();
    void on_QpbtnClose_clicked();
    void on_departmentName_clicked(const QModelIndex &index);
    void on_departmentName_customContextMenuRequested(const QPoint &pos);

signals:
    void SigClose();

private:
    Ui::CDEPARTMENT *ui; 
    QAction *UpdateAction;
    QAction *DeleteAction;
    QMenu *DepartmentMenu;

private:
    void _UpdateDepartment();
    void _InitDepartment();
    void _List();
    void _SetFocusOrder();

    int ti;         //部门数量
    int AddRow;
    int AddColumn;
    int UpdateRow;
    int UpdateColumn;
    int m_nFlag;
    QString m_strNo,m_strName,m_strPrincipal;
    QList<SList> m_List;    //SList：结构体，ID，编号，姓名

};

#endif // DEPARTMENT_H
