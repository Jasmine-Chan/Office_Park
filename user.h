#ifndef USER_H
#define USER_H

#include <QDialog>
#include <QMouseEvent>
#include "mysql.h"
#include<QMessageBox>

namespace Ui {
    class CUSER;
}

class CUSER : public QDialog
{
    Q_OBJECT

public:
    explicit CUSER(QWidget *parent = 0);
    ~CUSER();
    void _Update(QString strName,QString strUser,int nFlag,int nGrade);

private slots:
    void on_QpbtnClose_clicked();
    void on_QcomGrade_activated(int index);
    void on_QpbtnOK_clicked();

signals:
    void SigUpdateUser();

private:
    Ui::CUSER *ui;
    QString m_strName,m_strUser;
    int m_nFlag,m_nGrade;

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
//    virtual void paintEvent(QPaintEvent *event);
    QPoint move_point;      //移动的距离
    bool mouse_press;       //按下鼠标左键
};

#endif // USER_H
