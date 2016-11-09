#ifndef FIND_H
#define FIND_H
/*******************************
  查找功能操作界面
  ********************************/
#include <QDialog>
#include <QDebug>
#include <QMessageBox>
#include <QPainter>
#include <QMouseEvent>
#include <qmath.h>

namespace Ui {
    class CFIND;
}

class CFIND : public QDialog
{
    Q_OBJECT

public:
    explicit CFIND(QWidget *parent = 0);
    ~CFIND();
    void _Update(int nPage);

private slots:
    void on_QpbtnFind_clicked();
    void on_QpbtnClose_clicked();
    void on_QchkFind_activated(int index);
    void on_QrbtnTime_clicked();

signals:
    void SigFind(int nIndex,int nTime,QString strFind,QString strStime,QString strEtime);

private:
    Ui::CFIND *ui;
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    virtual void paintEvent(QPaintEvent *event);
    QPoint move_point;      //移动的距离
    bool mouse_press;       //按下鼠标左键
    int m_nPage,m_nIndex,m_nTime;
};

#endif // FIND_H
