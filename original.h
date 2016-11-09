#ifndef ORIGINAL_H
#define ORIGINAL_H
/*******************************
  考勤原始表界面，加载在考勤主界面上
  ********************************/
#include <QWidget>
#include <QAxObject>
#include "mysql.h"
#include "struct.h"
namespace Ui {
    class CORIGINAL;
}

class CORIGINAL : public QWidget
{
    Q_OBJECT

public:
    explicit CORIGINAL(QWidget *parent = 0);
    ~CORIGINAL();
    void _UpdateOriginalAttendance(QString strNo,int nFlag,QString strName);
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
    void _InitOriginalAttendance();
    bool eventFilter(QObject *obj, QEvent *ev);
private:
    Ui::CORIGINAL *ui;
    void _InitTree();
    void _SelectTarget(QString SelectName);
    int m_nFlag;
    QString FindName;
    QList<SList> m_List;  //存储容器

    bool isYear;
};

#endif // ORIGINAL_H
