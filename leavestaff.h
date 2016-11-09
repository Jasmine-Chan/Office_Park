#ifndef LEAVESTAFF_H
#define LEAVESTAFF_H

#include <QWidget>
#include "struct.h"
#include "mysql.h"
namespace Ui {
    class CLEAVESTAFF;
}

class CLEAVESTAFF : public QWidget
{
    Q_OBJECT

public:
    explicit CLEAVESTAFF(QWidget *parent = 0);
    ~CLEAVESTAFF();
    void _Update(SStaff sStaff);

private slots:
    void on_QpbtnOK_clicked();
    void on_QpbtnClose_clicked();

signals:
    void SigLeaveStaff(QString strNo);

private:
    Ui::CLEAVESTAFF *ui;
    SStaff m_sStaff;

private:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    virtual void paintEvent(QPaintEvent *event);
    QPoint move_point; //移动的距离
    bool mouse_press; //按下鼠标左键
};

#endif // LEAVESTAFF_H
