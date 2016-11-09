#ifndef DRAGSMARTCONTORL_H
#define DRAGSMARTCONTORL_H

#include <QDialog>
#include <windows.h>
#include <QLabel>
#include <QFrame>
#include <vector>
#include <QSqlQuery>
#include <QMenu>
#include <QAction>
#include <QHBoxLayout>
#include <QDebug>
#include "dragwindows.h"
#include <QDir>
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>

namespace Ui {
    class dragsmartContorl;
}

class dragsmartContorl : public QDialog
{
    Q_OBJECT

public:
    explicit dragsmartContorl(QWidget *parent = 0);
    void _Init();
    ~dragsmartContorl();

private:
    Ui::dragsmartContorl *ui;

    void _InitDray();

    dragwindows *IconicoSend;   //控件区
    dragwindows *Iconicorecv;   //画图区
    QWidget *InputCode;     //家电编码输入
    QHBoxLayout *Hlayout;

    QString strPath;

    QPushButton *button;    //测试控件
    QLineEdit   *LineType;
    QLineEdit   *LineRoomCode;  //房间编号
    QLineEdit   *LineDeviceCode;//房间内设备编号
    QLineEdit   *controlType;
    QLineEdit   *controlCode;
    QVBoxLayout *Vlayout;

signals:        //测试用信号槽函数
    void sendMessageToUdp(QString, QString, QString, QString, QString);

private slots:
    void button_clicked();
};

#endif // DRAGSMARTCONTORL_H
