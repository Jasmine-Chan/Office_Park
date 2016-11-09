#ifndef DAY_H
#define DAY_H
/*******************************
  考勤日报表界面，加载在考勤主界面上
  ********************************/
#include <QWidget>
#include "struct.h"
#include "mysql.h"
#include <QAxObject>

namespace Ui {
    class CDAY;
}

class CDAY : public QWidget
{
    Q_OBJECT

public:
    explicit CDAY(QWidget *parent = 0);
    ~CDAY();
    void _UpdateDetail(QString strNo,int nFlag,QString strName);
    void _Update();
signals:
    void SigcardDate(int,int,int);
    void SigReturnMenu();

private slots:
    void SlotrestType(QString m_restType);
    void on_Day_treeWidget_clicked(const QModelIndex &index);
    void on_QpbtnLead_clicked();
    void on_Qpbtn_Left_clicked();
    void on_Qpbtn_Right_clicked();
    void on_QpbtnBack_clicked();
    void on_QpbtnSearch_clicked();
    void on_QpbtnStretching_clicked();
    void on_Button_Month_clicked();
    void on_Button_Year_clicked();

private:
    Ui::CDAY *ui;
    void _InitDay();
    void _InitTree();
    void _SelectTarget(QString SelectName);
    bool isholiday;
    bool isTakeWorks;
    int m_nFlag;    //在职的判断标志
    QString restType;
    QString FindName;
    QList<SList> m_List;
    QList<QDate> n_List;

    bool isYear;
};

#endif // DAY_H
