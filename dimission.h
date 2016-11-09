#ifndef DIMISSION_H
#define DIMISSION_H
/*******************************
  离职员工界面，加载在人事主界面上
  ********************************/
#include <QWidget>
#include <windows.h>
#include "mysql.h"
namespace Ui {
    class CDIMISSION;
}

class CDIMISSION : public QWidget
{
    Q_OBJECT

public:
    explicit CDIMISSION(QWidget *parent = 0);
    ~CDIMISSION();
    void _UpdateDimission(QString SQL);
    void _Update();

private slots:
    void SlotDelete();
    void SlotDetail();
    void SlotEdit();
    void SlotFocusOut();

    void on_Head_Photo_D_clicked();
    void on_QpbtnClose_clicked();
    void on_QpbtnSave_clicked();
    void on_QpbtnSearch_clicked();
    void on_QpbtnRetuen_clicked();
    void on_QpbtnLogout_clicked();
    void on_QpbtnEdit_clicked();
    void on_QDimission_customContextMenuRequested(const QPoint &pos);

    void on_QpbtnChange_clicked();

    void on_QDimissionList_customContextMenuRequested(const QPoint &pos);

private:
    Ui::CDIMISSION *ui;

    QMenu *DimissionMenu;
    QAction *DeleteAction;
    QAction *DetailAction;
    QAction *EditAction;


private:
    void _InitDimission();
    void _InitDimissionList();
    void _InitConcreteInformation();
    void _SelectStatus(int Status);
    void _InitDepartmentForQcombobox();
    void _SetFocusOrder();
    void _Change();
    int ti;
    int Row;
    int DetailOrEdit;
    bool dimissionIsEmpty;
    int m_nChangeNoList;
    QString m_strSql;
};

#endif // DIMISSION_H
