#ifndef LOG_H
#define LOG_H
#include <QFile>
class CLOG
{
public:
    CLOG();
    int _Write(QString strMessage);
    void _Remove();
};

#endif // LOG_H
