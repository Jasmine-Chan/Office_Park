#ifndef ATTENDANCESET_H
#define ATTENDANCESET_H
/*******************************
  考勤设置界面，加载在考勤主界面上
  ********************************/
#include <QWidget>
#include <qsqlquery.h>
#include <qdebug.h>
#include <QDate>
#include <QMessageBox>
namespace Ui {
    class CATTENDANCESET;
}
class CATTENDANCESET : public QWidget
{
    Q_OBJECT

public:
    explicit CATTENDANCESET(QWidget *parent = 0);
    ~CATTENDANCESET();
     QDate m_date;
signals:
    void SigMonthlyrestType(QString m_restType);
    void SigDayrestType(QString m_restType);
    void SigFillcardRestType(QString m_fillcardresttype);

private slots:
    void SlotMonthlycardDate(int year,int month,int day);
    void SlotDaycardDate(int year,int month,int day);
    void SlotFillcardDate(QDate fillcardDate);
    void SlotFocusOut();

    void on_QpbtnOK_clicked();
    void on_normalLable_clicked();
    void on_restLable_clicked();
    void on_pbtnInstall_clicked();
    void on_weekdayLable_clicked();
    void on_twin_rest_clicked();
    void on_single_rest_clicked();
    void on_select_rest_clicked();
    void on_single_weekday_clicked();
    void on_twin_weekday_clicked();

private:
    Ui::CATTENDANCESET *ui;
    void _UpdateWorkTime();
    void _UpdateSQLWorkTime();

    bool NomalFalg;
    bool RestFalg;
    QString restType;//休息制度的类型到报表
    QString fillCardRestType;//休息制度的类型到补卡
};

#endif // ATTENDANCESET_H
