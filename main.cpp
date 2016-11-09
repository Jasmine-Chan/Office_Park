//#include <QtGui/QApplication>
#include <QtGui>
#include <QFile>
#include <QDesktopWidget>
#include <QTextCodec>
#include <QDebug>
#include <QTextCodec>
#include "windows.h"
#include "mysql.h"
#include "loading.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
//    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);
    CLOADING Loading;

    CMYSQL MySql;
    if(MySql._OpenMySql())
    {
        Loading.move ((QApplication::desktop()->width() - Loading.width())/2,(QApplication::desktop()->height() - Loading.height())/2);
        Loading.show();
    }
    else
    {
        QMessageBox::information(NULL,"提示","数据库连接失败无法启动");
        return 0;
    }
    QFile file("://test.qss");
    bool i = file.open(QFile::ReadOnly);    //登录界面皮肤设置
    QString strStyle = QLatin1String(file.readAll());
    a.setStyleSheet(strStyle);
//    a.setQuitOnLastWindowClosed(false);
    return a.exec();
}
