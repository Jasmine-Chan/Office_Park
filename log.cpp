#include "log.h"
#include <QDebug>
CLOG::CLOG()
{
}
void CLOG::_Remove()
{
    QFile file1("LogMessage.txt");
    file1.remove();
}

int CLOG::_Write(QString strMessage)
{
    QFile file1("LogMessage.txt");
    if(!file1.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Append))
           return 0;
    QTextStream out(&file1);
    out <<strMessage << "\n";
    file1.close();
    return 1;
}
