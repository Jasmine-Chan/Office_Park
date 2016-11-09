#ifndef BASICWINDOWS_H
#define BASICWINDOWS_H
/*******************************
  园区办公界面的父界面
  ********************************/
#include <QMainWindow>
#include <QString>
#include <QWidget>
#include <QDebug>
#include <QMouseEvent>
#include <QDesktopServices>
#include <QBitmap>
#include <QPainter>
#define  MARGIN 5

namespace Ui {
    class basicWindows;
}

class basicWindows : public QMainWindow
{
    Q_OBJECT

public:
    explicit basicWindows(QWidget *parent = 0);
    ~basicWindows();
    int countFlag(QPoint p, int row);
    void setCursorType(int flag);
    int countRow(QPoint p);
    void _Init(int x,int y);
    void _Interface();

private:
    Ui::basicWindows *ui;
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
   // void mouseDoubleClickEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *event);

private:
   bool isLeftPressed;
   int curPos;
   QPoint pLast;

signals:
    void movesignal();

private slots:
    void Double();

};

#endif // BASICWINDOWS_H
