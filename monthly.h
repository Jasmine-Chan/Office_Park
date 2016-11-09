#ifndef MONTHLY_H
#define MONTHLY_H
/*******************************
  考勤月报表界面，加载在考勤主界面上
  ********************************/
#include <QWidget>
#include <qstandarditemmodel.h>
#include <QAxObject>
#include <qglobal.h>
#include "mysql.h"
#include "attendanceset.h"


namespace Ui {
    class CMONTHLY;
}

class CMONTHLY : public QWidget
{
    Q_OBJECT

public:
    explicit CMONTHLY(QWidget *parent = 0);
    ~CMONTHLY();
    void _Update();

private slots:

    void SlotrestType(QString m_restType);
    void on_QpbtnLead_clicked();
    void on_Month_Tree_clicked(const QModelIndex &index);
    void on_Qpbtn_Left_clicked();
    void on_Qpbtn_Right_clicked();
    void on_QpbtnSearch_clicked();
    void on_QpbtnBack_clicked();
    void on_QpbtnStretching_clicked();

signals:
    void SigcardDate(int year,int month,int day);
    void SigReturnMenu();

private:
    Ui::CMONTHLY *ui;
    CATTENDANCESET *m_Attendanceset;

private:
    void _InitData();
    void _InitTree();
    void _updataData(QString strNo,int nFlag,QString strName);
    void _SelectTarget(QString SelectName);
    int m_nFlag;
    int HolidayFate;
    int RealFate;
    QString FindName;
    QString flog;
    QString restType;
    QList<SList> m_List;
    QList<QDate> n_List;
};

#endif // MONTHLY_H
