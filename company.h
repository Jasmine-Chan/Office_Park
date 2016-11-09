#ifndef COMPANY_H
#define COMPANY_H
/*******************************
  公司设置界面，加载在人事主界面上
  ********************************/
#include <QWidget>
#include <windows.h>
#include "mysql.h"
#include <QPixmap>
namespace Ui {
    class CCOMPANY;
}

class CCOMPANY : public QWidget
{
    Q_OBJECT

public:
    explicit CCOMPANY(QWidget *parent = 0);
    ~CCOMPANY();
    void _Init();

private slots:
    void on_QpbtnAddCompany_clicked();
    void on_QpbtnSave_clicked();
    void on_QpbtnClose_clicked();
    void on_camp_pushButton_clicked();   
signals:
    void SigClose();

private:
    Ui::CCOMPANY *ui;
    int m_nFlag;
    QString LogoPath;//公司Logo的地址
    void _InitLine(int n);
    void _SetFocusOrder();
};

#endif // COMPANY_H
