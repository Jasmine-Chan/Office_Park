//各类删除函数文件
#ifndef DELETE_H
#define DELETE_H
/*******************************
  删除功能操作界面
  ********************************/
#include <QDialog>
#include "struct.h"

namespace Ui {
    class CDELETE;
}

class CDELETE : public QDialog
{
    Q_OBJECT

public:
    explicit CDELETE(QWidget *parent = 0);
    ~CDELETE();
    void _Update(int nPage, SUser sUser);
private slots:
    void on_QpbtnDelete_clicked();
    void on_QpbtnClose_clicked();
signals:
    void SigDelete();
private:
    Ui::CDELETE *ui;
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    virtual void paintEvent(QPaintEvent *event);
    int m_nPage;
    SUser m_sUser;
    QPoint move_point; //移动的距离
    bool mouse_press; //按下鼠标左键

};

#endif // DELETE_H
