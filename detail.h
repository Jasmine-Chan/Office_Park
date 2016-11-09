#ifndef DETAIL_H
#define DETAIL_H
/*******************************
  考勤明细表界面，加载在考勤主界面上
  ********************************/
#include <QWidget>
#include "struct.h"
#include "mysql.h"
#include <QAxObject>
class QAxObject;

namespace Ui {
    class CDETAIL;
}

class CDETAIL : public QWidget
{
    Q_OBJECT

public:
    explicit CDETAIL(QWidget *parent = 0);
    ~CDETAIL();
    void _UpdateDetail(QString strNo,int nFlag,QString strName);
    void _Update();

signals:
    void SigReturnMenu();


private slots:
    void on_QDepartment_clicked(const QModelIndex &index);
    void on_QpbtnLead_clicked();
    void on_Qpbtn_Left_clicked();
    void on_Qpbtn_Right_clicked();
    void on_QpbtnBack_clicked();
    void on_QpbtnSearch_clicked();
    void on_QpbtnStretching_clicked();
    void on_Button_Month_clicked();
    void on_Button_Year_clicked();

private:
    Ui::CDETAIL *ui;
    void _InitTree();
    void _InitDetail();
    void _SelectTarget(QString SelectName);
    int m_nFlag;
    QString FindName;
    QList<SList> m_List;

    bool isYear;
};

#endif // DETAIL_H
