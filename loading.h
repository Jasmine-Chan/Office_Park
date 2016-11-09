#ifndef LOADING_H
#define LOADING_H

#include <QDialog>
#include "officepark.h"
#include <QSystemTrayIcon>
#include <QTimer>
namespace Ui {
    class CLOADING;
}

class CLOADING : public QDialog
{
    Q_OBJECT
public:
    explicit CLOADING(QWidget *parent = 0);
    ~CLOADING();
//    void _Update();
//    void closeEvent( QCloseEvent * event );
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    QTimer *Timer1;
    QList<QString> T;

private slots:
    void Login();
    void on_QpbtnLoad_clicked();
    void on_QpbtnRevert_clicked();
    void on_QchkPassWord_clicked();
    void on_QchkAutoLogin_clicked();

public slots:
    void SlotLoading();

signals :
    void SigQuit();
    void SigUser(QString user);

private:
    Ui::CLOADING *ui;   
    COFFICEPARK *COfficePark;
    QMouseEvent *event;
    QSystemTrayIcon *TrayIcon;
    QString strPath;
    int m_nFlag;
    QPoint pos;

private:
    void _Interface();
    void _ReadString();
    void _WriteString();
    void _Encode(char *buf);
    void _Decode(char *buf);

};

#endif // LOADING_H
