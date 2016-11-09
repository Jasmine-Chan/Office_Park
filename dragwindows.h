#ifndef DRAGWINDOWS_H
#define DRAGWINDOWS_H
/*******************************
  画图部分，画图操作界面
  ********************************/
#include <QWidget>
#include <windows.h>
#include <QLabel>
#include <QFrame>
#include <vector>
#include <QSqlQuery>
#include <QMenu>
#include <QAction>
#include <iterator>
#include <QList>
#include <QDir> //设置读写配置文件的路径
#include <QFileDialog>
#include <QPainter>
#include <QPixmap>
#include <QPushButton>

namespace Ui {
    class dragwindows;
}

class dragwindows : public QWidget
{
    Q_OBJECT

public:
    explicit dragwindows(QWidget *parent = 0);
//    void paintEvent(QPaintEvent *event);
    void _InitBackground();
    void _InitMap();
    void _Init(bool IsRecv);
    ~dragwindows();


private:
    Ui::dragwindows *ui;
    void mousePressEvent(QMouseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
    void flushWidget(int i, QString Path);//刷新控件区
    QString strLocation; //鼠标在控件区图标上的坐标
    QPoint PointLocction;
    bool IsIconicRecv;  //画图区状态
    int widget_ID;

    QPoint DeletePoint;
    QRect UpRect;   //控件相交检测
    QRect DownRect;
    QRect UpdateRect;
    QRect OrignalRect;
    QString BackgroundStr;  //保存画图区背景图片
    int temp;   //记录控件区刷新时，Y轴坐标
    QString LabelPath;
    int WidgetId;//保存从配置文件中读取到的控件的编号

private:
    QMenu *MapMenu;
    QAction *deleteAction;
    QAction *addAction;
    QAction *uploadAction;
    QString strPath;

signals:
    void SigLocation(QString location);

private slots:
    void SlotLocation(QString location);
    void SlotDeleteWidget();
    void SlotAddWidget();
    void SlotUpLoadBackground();
    void on_tableWidget_customContextMenuRequested(const QPoint &pos);

};

#endif // DRAGWINDOWS_H
