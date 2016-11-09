#ifndef HANDLE_H
#define HANDLE_H
/*******************************
  员工添加界面,<现已不用>
  ********************************/
#include <QWidget>
#include "mysql.h"
namespace Ui {
    class CHANDLE;
}

class CHANDLE : public QWidget
{
    Q_OBJECT

public:
    explicit CHANDLE(QWidget *parent = 0);
    ~CHANDLE();
    void _Update();
    void _Update(SStaff sStaff);

private slots:
    void on_QpbtnOK_clicked();
    void on_QpbtnClose_clicked();

signals:
    void SigInit();

private:
        CMYSQL *CMysql;

private:
    Ui::CHANDLE *ui;
    void _InitDepartment();
    void _UpdateStaff(SStaff *sStaff);
    QString m_strNO,m_strEnterTime;
    int m_nFlag;
    QList<SList> m_List;
};

#endif // HANDLE_H
