#ifndef BASICWINDOW_H
#define BASICWINDOW_H
#include<QString>
#include<QWidget>
#include<QDebug>
#include<QMouseEvent>
#include<QDesktopServices>
#include <QBitmap>
#include <QPainter>
#define MARGIN 5

namespace Ui {
class BasicWindow;
}

class BasicWindow : public QWidget
{
    Q_OBJECT
    
public:
    explicit BasicWindow(QWidget *parent = 0);
    ~BasicWindow();

    bool isLeftPressed;
    int curPos;
    QPoint pLast;

    int countFlag(QPoint p, int row);
    void setCursorType(int flag);
    int countRow(QPoint p);
    void Double();
    void _Init(int x,int y);
private:
    Ui::BasicWindow *ui;
    int m_nIsMax;
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void _Interface();
signals:
    void movesignal();
};

#endif // BASICWINDOW_H
