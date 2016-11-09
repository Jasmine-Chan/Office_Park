#ifndef MYSQL_H
#define MYSQL_H
#include <QtSql>

#include <QSqlDatabase>
#include "struct.h"
class CMYSQL
{
private:
     QString m_strDBName,m_strDBUser,m_strDBPass;
     int number;
     int recordTotal;
public:
    CMYSQL();

    bool _OpenODBC();
    bool _OpenMySql();
    bool _IsOpen();
    //Loading
    bool _GetAllUser(QList<QString> *T);
    QString _GetCityNum(QString strCity);
    bool _GetUser(SUser &sUser);


    bool _UpdateDevice(int nMark,SDevice sDevice);
    bool _InsertDevice(SDevice sDevice);
    bool _InsertCardRecord(int nID,QString strAddrExplain,char* gccardTime);
    bool _InsertRegisterStaff(int nID,QString strAddrExplain);
    bool _SelectNo(QString strNo);
    bool _InsertStaff(SStaff sStaff);
    bool _UpdateStaff(SStaff sStaff,QString strNo,QString strEnter);
    bool _DeleteStaff(QString strNo,QString strEnter);
    bool _LeaveStaff(SStaff sStaff,QString strDeal,QString strDealMessage);
    bool _InsertAlarm(SAlarmDevice sAlarmDevice);
    bool _UpdateAllRacd(QString strAddrExplain,unsigned char* buf);
    bool _OnlineDevice(SDevice sDevice);
    bool _LogOutIC(int nId);
    bool _SelectLeaveToNo(QString strNo);
    bool _UpdateDealAlarm(SAlarmDevice sAlarmDevice,QString strAlarmStatu,QString strDealHuman,QString strMessage);
    bool _DeleteDevice(QString strMac,QString strIp,QString strAddr,QString strType);
    bool _UpdateAlarm(SAlarmDevice sAlarmDevice);
    bool _InsertUser(QString strName,QString strUser,QString strPass,int nGrade);
    bool _FindUser(QString strUser,QString strPass);
    bool _DeleteUser(QString strName,QString strUser);
    bool _UpdateUser(QString strOldUser,QString strName,QString strUser,QString strPass);
    bool _InsertSystem(int nType,QString strUser,QString strName);
    bool _DeleteAlarmRecord(QString QAddr,QString QStime,QString QEtime);
    bool _DeleteDeviceRecord(QString QAddr,QString QStime,QString QEtime);
    bool _DeleteSystemRecord(QString QAddr,QString QStime,QString QEtime);
    bool _IsHaveNo(QString strNo);

//    int Test(QString SQL);
};

#endif // MYSQL_H
