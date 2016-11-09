#include "mysql.h"
//#include "log.h"
#include <QtGui/QApplication>
#include <QMessageBox>
#include "windows.h"
CMYSQL::CMYSQL()
{
}

//int CMYSQL::Test(QString SQL)
//{
//    QSqlQuery query,query1;
//    if(QSqlDatabase::database().driver()->hasFeature(QSqlDriver::Transactions))
//    { //先判断该数据库是否支持事务操作
//        if(QSqlDatabase::database().transaction()) //启动事务操作
//        {
//            //下面执行各种数据库操作
////                QString SQL = "insert into test values (15, 12)";
////                QString SQL1 = "delete from test where id = 12";
//            query.clear();
//            int i = query.exec(SQL);
//            query1.clear();
////                int i1 = query1.exec(SQL1);
//            if(i)
////                if(i&&i1)
//            {
//                QSqlDatabase::database().commit();
//            }
//            else
//            {
//                QSqlDatabase::database().rollback();
//            }
//        }
//    }
//}
/*========================================================================
        Name:		打开数据库并连接。
    ----------------------------------------------------------
        returns:	返回数据库连接状况。
    ----------------------------------------------------------
        Remarks:
==========================================================================*/
bool CMYSQL::_OpenMySql()
{
    QString path;
    QDir dir;
    path = dir.currentPath() + "/DBSet1.ini";
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL"); // 使用mysql数据库驱动
    char buf[20] = {0};
    ::GetPrivateProfileStringA("Flag","IP","localhost",buf,20,path.toAscii().data());
    db.setHostName (QString(buf));
    memset(buf,0,20);

    ::GetPrivateProfileStringA("Flag","Name","sh",buf,20,path.toAscii().data());
    db.setDatabaseName (QString(buf));
    memset(buf,0,20);
    ::GetPrivateProfileStringA("Flag","User","root",buf,20,path.toAscii().data());
    db.setUserName (QString(buf));

    memset(buf,0,20);
    ::GetPrivateProfileStringA("Flag","Pass","anenda",buf,20,path.toAscii().data());
    db.setPassword (QString(buf));

    return db.open ();
}

bool CMYSQL::_OpenODBC()
{
    QString path;
    QDir dir;
    path=dir.currentPath() + "/DBSet1.ini";
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC"); // 使用mysql数据库驱动
    char buf[20] = {0};
//    ::GetPrivateProfileStringA("Flag","IP","localhost",buf,20,path.toAscii().data());
//    db.setHostName (QString(buf));
//    memset(buf,0,20);
    ::GetPrivateProfileStringA("Flag","Name","sh",buf,20,path.toAscii().data());
    db.setDatabaseName (QString(buf));
    memset(buf,0,20);
    ::GetPrivateProfileStringA("Flag","User","root",buf,20,path.toAscii().data());
    db.setUserName (QString(buf));
    memset(buf,0,20);
    ::GetPrivateProfileStringA("Flag","Pass","anenda",buf,20,path.toAscii().data());
    db.setPassword (QString(buf));
    return db.open ();
}

//插入新用户
bool CMYSQL::_InsertUser(QString strName,QString strUser,QString strPass,int nGrade)
{
    QString SQL = "insert into user (User_Name,User_User,User_Grade,User_Pass) VALUES ('"+strName+"','"+strUser+"','"+QString::number(nGrade)+"',MD5('"+strPass+"'))";
    QSqlQuery query;
    return query.exec(SQL);
}

//查找用户
 bool CMYSQL::_FindUser(QString strUser,QString strPass)
 {
     QString SQL = "select *from user where User_Pass = MD5('"+strPass+"') and User_User = '"+strUser+"'";
     QSqlQuery query;
     if(query.exec(SQL))
     {
         if(query.next())
         {
             return true;
         }else return false;
     }else return false;
 }

 //修改新用户
 bool CMYSQL::_UpdateUser(QString strOldUser,QString strName,QString strUser,QString strPass)
 {
     QString SQL = "update user set User_Name = '"+strName+"' ,User_User = '"+strUser+"',\
             User_Pass = MD5('"+strPass+"') where User_User = '"+strOldUser+"'";
     QSqlQuery query;
     return query.exec(SQL);
 }

 //删除用户
 bool CMYSQL::_DeleteUser(QString strName,QString strUser)
 {
     QString SQL = "delete from user where User_name = '"+strName+"' and User_user = '"+strUser+"'";
     QSqlQuery query;
     return query.exec(SQL);
 }

//掉线存入设备日志
bool CMYSQL::_OnlineDevice(SDevice sDevice)
{
    QString str = QString(sDevice.gcAddr);
    QString str2 = QString(sDevice.gcMacAddr);
    QString str3 = QString(sDevice.gcIpAddr);
    QString str4 = QString(sDevice.gcType);
    QString str5 = QString(sDevice.gcIntraAddr);
    QString str10 = QString::number(sDevice.nFenceState);
    QString str6 = "断开";
    QDateTime dateTime=QDateTime::currentDateTime();
    QString str7 = dateTime.toString("yyyy-MM-dd hh:mm:ss");
    QString SQL1 = "insert into device_record (Device_Record_Addr,Device_Record_Type,Device_Record_Ip,Device_Record_Time) VALUES ('"+str+"','"+str6+"','"+str3+"','"+str7+"')";
    QSqlQuery query1;
    if(query1.exec(SQL1))
    {
        return true;
    }
    else
    {
        return false;
    }
}

//插入报警
bool CMYSQL::_InsertAlarm(SAlarmDevice sAlarmDevice)
{
    QString str = QString(sAlarmDevice.gcMacAddr);
    QString str1 = QString(sAlarmDevice.gcIpAddr);
    QString str2 = "防区" + QString::number(sAlarmDevice.nFenceId);
    QString str3;
    switch(sAlarmDevice.nAlarmType)
    {
        str3.clear();
        case 0:
            str3 = "特殊报警";
            break;
        case 2:
            str3 = "红外";
            break;
        case 3:
            str3 = "门铃";
            break;
        case 4:
            str3 = "烟感";
            break;
        case 6:
            str3 = "门磁1";
            break;
        case 7:
            str3 = "遥控SOS";
            break;
        case 8:
            str3 = "门磁2";
            break;
        case 10:
            str3 = "水浸";
            break;
        case 12:
            str3 = "煤气";
            break;
        case 13:
            str3 = "门磁3";
            break;
        case 14:
            str3 = "胁迫开门";
            break;
        case 15:
            str3 = "设备强拆";
            break;
        case 16:
            str3 = "开门超时";
        default :
            str3 = "错误";
            break;
    }

    QString str4 = QString(sAlarmDevice.gcStime);
    QString str5 = QString(sAlarmDevice.gcIntraAddr);
    QString str7 = QString(sAlarmDevice.gcPhone1);
    QString str9 = QString(sAlarmDevice.gcType);
    QString SQL = "INSERT INTO alarm (alarm.Alarm_Mac_Addr,alarm.Alarm_Ip_Addr,alarm.Alarm_Fence,alarm.Alarm_Type,alarm.Alarm_Stime,alarm.Alarm_Intra_Addr,alarm.Alarm_Device_Type)\
            VALUES ('"+str+"','"+str1+"','"+str2+"','"+str3+"','"+str4+"','"+str5+"','"+str9+"')";
    QSqlQuery query;
    return query.exec(SQL);
}

//打开数据库
bool CMYSQL::_IsOpen()
{
    QSqlDatabase db = QSqlDatabase::addDatabase ("QODBC");//通过ODBC数据源连接数据库
    char buf[16] = {0};
    ::GetPrivateProfileStringA("Flag","IP","localhost",buf,16,"C:\\WINDOWS\\DBSet.ini");
    db.setHostName (QString(buf));
    memset(buf,0,16);
    ::GetPrivateProfileStringA("Flag","Name","sh",buf,16,"C:\\WINDOWS\\DBSet.ini");
    db.setDatabaseName (QString(buf));
    memset(buf,0,16);
    ::GetPrivateProfileStringA("Flag","User","root",buf,16,"C:\\WINDOWS\\DBSet.ini");
    db.setUserName (QString(buf));
    memset(buf,0,16);
    ::GetPrivateProfileStringA("Flag","Pass","root",buf,16,"C:\\WINDOWS\\DBSet.ini");
    db.setPassword (QString(buf));
    return db.open ();
}
/*========================================================================
        Name:		查询城市的编码。
    ----------------------------------------------------------
        returns:	返回城市的编码。
    ----------------------------------------------------------
        Remarks:
    ----------------------------------------------------------
        Params:         strCity 城市的名称
==========================================================================*/
QString CMYSQL::_GetCityNum(QString strCity)
{
    QString SQL = "Select * From City where City_name = '"+strCity+"'";
    QSqlQuery query;
    query.clear();
    QString strCityNum;
    strCityNum.clear();
    if(query.exec(SQL))
    {
        if(query.first())
        {
            strCityNum = query.value(2).toString();
        }
    }
    return strCityNum;
}

/*========================================================================
        Name:		获取所有的用户名。
    ----------------------------------------------------------
        returns:	返回获取的用户的成功。
    ----------------------------------------------------------
        Remarks:
    ----------------------------------------------------------
        Params:         用户名链表容器
==========================================================================*/
bool CMYSQL::_GetAllUser(QList<QString> *T)
{
    QString SQL = "select User_user from user";
    QSqlQuery query;
     if(query.exec(SQL))
     {
         int i = 0;
         while(query.next())
         {
             (*T).insert(i,QString(query.value(0).toByteArray().data()));
             i++;
         }
         return true;
     }else
         return false;
}
/*========================================================================
        Name:		判定用户名密码是否正确并查询城市的编码。
    ----------------------------------------------------------
        returns:	返回城市的编码。
    ----------------------------------------------------------
        Remarks:        查询用户名密码是否正确，正确则查询城市对应的编码
    ----------------------------------------------------------
        Params:         sUser 用户的结构体
==========================================================================*/
bool CMYSQL::_GetUser(SUser &sUser)
{
    QString SQL = "Select * From user where User_User = '"+sUser.strUser+"' and User_Pass = MD5('"+sUser.strPass+"')";
    QSqlQuery query;
    if(query.exec(SQL))
    {
        if(query.numRowsAffected () > 0)
        {
            query.next();
            sUser.strName = query.value(3).toString();
            sUser.nGrade = query.value(4).toUInt(); //权限
            SQL.clear();
            SQL = "Select * From city where City_Name = '"+sUser.strCity+"'";
            query.clear();
            if(query.exec(SQL))
            {
                if(query.first())
                {
                    sUser.strCityNum = query.value(2).toString();
                }
                return true;
            }
            else
                return false;

        }
    }
    else
    {
        return false;
    }
}

//添加系统日志
bool CMYSQL::_InsertSystem(int nType,QString strUser,QString strName)
{
    QString strType,strTime;
    strType.clear();
    strTime.clear();
    switch(nType)
    {
        case 1:
            strType = "登陆";
            break;
        case 2:
            strType = "锁定";
            break;
        case 3:
            strType = "退出";
            break;
        case 4:
            strType = "解锁";
            break;
        case 5:
            strType = "注销";
            break;
        default :
            break;
    }
    QDateTime dateTime = QDateTime::currentDateTime();
    strTime = dateTime.toString("yyyy-MM-dd hh:mm:ss");
    QString SQL = "insert into system_record (System_Record_Name,System_Record_Human,System_Record_Type,System_Record_Time)values ('"+strUser+"','"+strName+"','"+strType+"','"+strTime+"')";
    QSqlQuery query;
    return query.exec(SQL);
}

//更新设备
bool CMYSQL::_UpdateDevice(int nMark,SDevice sDevice)
{
    QString strAddr  = QString(sDevice.gcAddr);
    QString strMacAddr = QString(sDevice.gcMacAddr);
    QString strIpAddr = QString(sDevice.gcIpAddr);
    QString strType = QString(sDevice.gcType);
    QString strIntraAddr = QString(sDevice.gcIntraAddr);

    QString str9 = "连接";
    QString strFenceState = QString::number(sDevice.nFenceState);
    QDateTime dateTime = QDateTime::currentDateTime();
    QString strdateTime = dateTime.toString("yyyy-MM-dd hh:mm:ss");
    QString SQL = "UPDATE device SET Device_Addr = '"+strAddr+"',Device_Ip_Addr = '"
            +strIpAddr+"',Device_Type = '"+strType+"',Device_Intra_Addr = '"+strIntraAddr+"',"
            "Device_Fence_Statu = '"+strFenceState+"' where Device_Mac_Addr = '"+strMacAddr+"'";
    QSqlQuery query,query1;
    QString SQL1;
    int j = 0;
    int m = 0;

    SQL1.clear();
    SQL1 = "insert into device_record (Device_Record_Addr,Device_Record_Type,Device_Record_Ip,Device_Record_Time) VALUES ('"+strAddr+"','"+str9+"','"+strIpAddr+"','"+strdateTime+"')";
    j = query1.exec(SQL1);

    if(sDevice.nOldFenceState == sDevice.nFenceState)
    {
        m = 1;
    }
    else
    {
        QString str12;
        str12.clear();
        switch(sDevice.nFenceState)
        {
            case 1:
                str12 = "外出布防";
                break;
            case 2:
                str12 = "在家布防";
                break;
            case 0:
                str12 = "撤防";
                break;
            default :
                break;
        }

        SQL1.clear();
        SQL1 = "insert into device_record (Device_Record_Addr,Device_Record_Type,Device_Record_Ip,Device_Record_Time) VALUES ('"+strAddr+"','"+str12+"','"+strIpAddr+"','"+strdateTime+"')";
        m = query1.exec(SQL1);
    }
    if(query.exec(SQL) && j && m)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//插入中间设备表
bool CMYSQL::_InsertDevice(SDevice sDevice)
{
    QString strAddr = QString(sDevice.gcAddr);
    QString strMacAddr = QString(sDevice.gcMacAddr);
    QString strIpAddr = QString(sDevice.gcIpAddr);
    QString strType = QString(sDevice.gcType);
    QString strIntraAddr = QString(sDevice.gcIntraAddr);
    QString strFenceState = QString::number(sDevice.nFenceState);//此时为0
    QString str6 = "连接";
    QDateTime dateTime = QDateTime::currentDateTime();
    QString strdateTime = dateTime.toString("yyyy-MM-dd hh:mm:ss");
    QString SQL = "insert into device (Device_Addr,Device_Ip_Addr,Device_Mac_Addr,Device_Fence_Statu,Device_Intra_Addr,Device_Type)VALUES ('"+
            strAddr+"','"+strIpAddr+"','"+strMacAddr+"','"+strFenceState+"','"+strIntraAddr+"','"+strType+"')";
    QString str12;
    str12.clear();
    switch(sDevice.nFenceState)
    {
        case 1:
            str12 = "外出布防";
            break;
        case 2:
            str12 = "在家布防";
            break;
        case 0:
            str12 = "撤防";
            break;
        default :
            break;
    }
    QString  SQL2 = "insert into device_record (Device_Record_Addr,Device_Record_Type,Device_Record_Ip,Device_Record_Time) VALUES ('"+strAddr+"','"+str12+"','"+strIpAddr+"','"+strdateTime+"')";

    QSqlQuery query;
    if(query.exec(SQL))
    {
        query.exec(SQL2);
        qDebug()<<"str12:"<<str12<<strIpAddr;
        return true;
    }
    else
    {
        return false;
    }
}

//添加员工的入职时间
bool CMYSQL::_InsertRegisterStaff(int nID,QString strAddrExplain)
{
    QDateTime dateTime=QDateTime::currentDateTime();
    QString str = dateTime.toString("yyyy-MM-dd hh:mm:ss");

    QString strNo = QString::number(nID);
    switch(strNo.length())
    {
        case 1:
            strNo = "0000" + strNo;
            break;
        case 2:
            strNo = "000" + strNo;
            break;
        case 3:
            strNo = "00" + strNo;
            break;
        case 4:
            strNo = "0" + strNo;
            break;
        case 5:
            strNo = strNo;
            break;
    }

    QString SQL = "Select * From  staff where Staff_No = "+strNo+"";
    QSqlQuery query;
    if(query.exec(SQL))
    {
        if(query.next())
        {
            return false;
        }
        else
        {
            QString  SQL1 = "insert into staff (Staff_No,Staff_Enter_Day) VALUES ('"+strNo+"','"+str+"')";
            return query.exec(SQL1);
        }
    }
    else
        return false;
}

//添加刷卡记录
bool CMYSQL::_InsertCardRecord(int nID,QString strAddrExplain,char* gccardTime)
{
    QDateTime dateTime = QDateTime::currentDateTime();
    int cardYear = QDate::currentDate().year();

    QDate cardDate;     //日期
    cardDate.setYMD(cardYear,gccardTime[2],gccardTime[3]);

    QTime cardTime;     //时间
    cardTime.setHMS(gccardTime[4],gccardTime[5],gccardTime[6]);

    QDateTime cardDateTime;
    cardDateTime.setDate(cardDate);
    cardDateTime.setTime(cardTime);

    QString strDate = cardDate.toString("yyyy-MM-dd");
    QString strTime = cardTime.toString("hh:mm:ss");
    QString strNo = QString::number(nID);
    switch(strNo.length())
    {
        case 1:
            strNo = "0000" + strNo;
            break;
        case 2:
            strNo = "000" + strNo;
            break;
        case 3:
            strNo = "00" + strNo;
            break;
        case 4:
            strNo = "0" + strNo;
            break;
        case 5:
            strNo = strNo;
            break;
    }
    if(cardDateTime.secsTo(dateTime)<=60)
    {
        QString  SQL = "insert into card_record (Card_Record_Staff_No,Card_Record_Date,Card_Record_Time,Card_Record_Device_No) VALUES ('"+strNo+"','"+strDate+"','"+strTime+"','"+strAddrExplain+"')";
        QSqlQuery query;
        return query.exec(SQL);
    }
    else
        return false;
}

//注销
bool CMYSQL::_LogOutIC(int nId)
{
    QString  SQL = "UPDATE staff SET Staff_IC_Statu = 1 where Staff_No = '"+QString::number(nId)+"'";
    QSqlQuery query;
    return query.exec(SQL);
}

//处理报警
bool CMYSQL::_UpdateDealAlarm(SAlarmDevice sAlarmDevice,QString strAlarmStatu,QString strDealHuman,QString strMessage)
{
    QString strDtime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString strStime = QString(sAlarmDevice.gcStime);
    QString SQL = "update  alarm set Alarm_Dtime = '"+strDtime+"',Alarm_Status = '"+strAlarmStatu+"',\
            Alarm_Deal_Human = '"+strDealHuman+"',Alarm_deal_message = '"+strMessage+"' where Alarm_Stime = '"+strStime+"'";
    QSqlQuery query;
    return query.exec(SQL);
}

//删除设备
bool CMYSQL::_DeleteDevice(QString strMac,QString strIp,QString strAddr,QString strType)
{
    QString SQL = "delete from  device  where Device_Addr = '"+strAddr+"' and Device_Mac_Addr = '"+strMac+"' and \
            Device_Ip_Addr = '"+strIp+"' and Device_Type = '"+strType+"' ";
    QSqlQuery query;
    bool i = query.exec(SQL);
    qDebug()<<SQL<<i;
    return i;
}

//更新报警结束时间
bool CMYSQL::_UpdateAlarm(SAlarmDevice sAlarmDevice)
{
    QString str = QString(sAlarmDevice.gcIpAddr);
    QString str1 = QString(sAlarmDevice.gcEtime);
    QString str2 = QString(sAlarmDevice.gcIntraAddr);
    QString SQL = "update  alarm set Alarm_Etime = '"+str1+"' where Alarm_Ip_Addr = '"+str+"' and Alarm_Intra_Addr = '"+str2+"' and Alarm_Etime is null";
    QSqlQuery query;
    return query.exec(SQL);
}

bool CMYSQL::_SelectLeaveToNo(QString strNo)
{
    QString  SQL = "Select Staff_IC_Statu From  staff where Staff_No = '"+strNo+"'";
    QSqlQuery query;
    if(query.exec(SQL))
    {
        query.next();
        return query.value(0).toInt();
    }
    else
    {
        QMessageBox::information(NULL,"提示","数据库连接失败");
        return false;
    }
}

bool CMYSQL::_SelectNo(QString strNo)
{
    QString  SQL = "Select count(*) From  staff where Staff_No = '"+strNo+"'";
    QSqlQuery query;
    if(query.exec(SQL))
    {
        query.next();
        return query.value(0).toInt();
    }
    else
    {
        QMessageBox::information(NULL,"提示","数据库连接失败");
        return false;
    }
}

//删除员工
bool CMYSQL::_DeleteStaff(QString strNo,QString strEnter)
{
    QString  SQL = "delete from staff  WHERE Staff_No = '"+strNo+"' AND Staff_Enter_Day = '"+strEnter+"'";
    QSqlQuery query;
    return query.exec(SQL);
}

//将离职员工添加进离职表
bool CMYSQL::_LeaveStaff(SStaff sStaff,QString strDeal,QString strDealMessage)
{
    QString strDate = QDate::currentDate().toString("yyyy-MM-dd");
    QString DepartmentId = "";
    QString DepartmentSQL = "SELECT * FROM department  WHERE Department_Name = '"+sStaff.strDepartment+"'";
    QSqlQuery query,query1,departmentquery;
    departmentquery.exec(DepartmentSQL);
    if(departmentquery.next())
        DepartmentId = departmentquery.value(1).toString();
    if(QSqlDatabase::database().driver()->hasFeature(QSqlDriver::Transactions))
    {
        //先判断该数据库是否支持事务操作
        if(QSqlDatabase::database().transaction()) //启动事务操作
        {
            //下面执行各种数据库操作
            QString SQL = "delete from staff  WHERE Staff_No = '"+sStaff.strNo+"' AND Staff_Enter_Day = '"+sStaff.strEnter+"'";
            QString SQL1 = "INSERT INTO `leave_staff` (leave_staff.Leave_Deal,leave_staff.Leave_Day,leave_staff.Leave_Deal_Day,leave_staff.Leave_Reasons,leave_staff.Leave_Staff_Department_Id,"
                    "leave_staff.Leave_Staff_Enter_Day,leave_staff.Leave_Staff_Id_Card,leave_staff.Leave_Staff_Name,"
                    "leave_staff.Leave_Staff_No,leave_staff.Leave_Staff_Post,leave_staff.Leave_Staff_Sex,leave_staff.Leave_Image_Path) VALUES ('"+strDeal+"','"+strDate+"','"+strDate+"','"+strDealMessage+"','"+DepartmentId+"',"
                    "'"+sStaff.strEnter+"','"+sStaff.strIdCard+"','"+sStaff.strName+"','"+sStaff.strNo+"','"+sStaff.strPost+"','"+QString::number(sStaff.nSex)+"','"+sStaff.strImagePath+"');";
            if(query.exec(SQL)&&query1.exec(SQL1))
            {
                QSqlDatabase::database().commit();
                return true;
            }
            else
            {
                QSqlDatabase::database().rollback();
                return false;
            }
        }
    }
}

//更新员工信息
bool CMYSQL::_UpdateStaff(SStaff sStaff,QString strNo,QString strEnter)
{
    QSqlQuery query;
    QString  SQL = "UPDATE staff SET Staff_Addr = '"+sStaff.strAdd+"',Staff_Birthday = '"+sStaff.strBirthday+"',Staff_Department_No = '"+sStaff.strDepartment+"',Staff_Education_Id = '"+QString::number(sStaff.nEducation)+"',Staff_Email = '"+sStaff.strEmail+"',"
            "Staff_Enter_Day = '"+sStaff.strEnter+"',Staff_Graduation_Date = '"+sStaff.strGraduationDate+"',Staff_Graduation_School = '"+sStaff.strGraduationSchool+"',Staff_Id_Card = '"+sStaff.strIdCard+"',Staff_Major = '"+sStaff.strMajor+"',Staff_Marital_Status = '"+QString::number(sStaff.nMaritalStatus)+"',"
            "Staff_Name = '"+sStaff.strName+"',Staff_Nation_Id = '"+QString::number(sStaff.nNation)+"',Staff_Native_Place = '"+sStaff.strNative+"',Staff_No = '"+sStaff.strNo+"',Staff_Phone = '"+sStaff.strPhone+"',Staff_Politics_Status = '"+QString::number(sStaff.nPolitics)+"',"
            "Staff_Post = '"+sStaff.strPost+"',Staff_Remark = '"+sStaff.strRemark+"',Staff_Sex = '"+QString::number(sStaff.nSex)+"',Staff_Image_Path = '"+sStaff.strImagePath+"'  WHERE Staff_No = '"+strNo+"' AND Staff_Enter_Day = '"+strEnter+"'";

    return query.exec(SQL);
}

//添加员工
bool CMYSQL::_InsertStaff(SStaff sStaff)
{
    QSqlQuery query;

    QString  SQL = "insert into staff (staff.Staff_Addr,staff.Staff_Birthday,staff.Staff_Department_No,"
            "staff.Staff_Education_Id,staff.Staff_Email,staff.Staff_Enter_Day,staff.Staff_Graduation_Date,"
            "staff.Staff_Graduation_School,staff.Staff_Id_Card,staff.Staff_Major,staff.Staff_Marital_Status,"
            "staff.Staff_Name,staff.Staff_Nation_Id,staff.Staff_Native_Place,staff.Staff_No,staff.Staff_Phone,"
            "staff.Staff_Politics_Status,staff.Staff_Post,staff.Staff_Remark,staff.Staff_Sex,Staff_Image_Path) VALUES"
            "('"+sStaff.strAdd+"','"+sStaff.strBirthday+"','"+sStaff.strDepartment+"','"+QString::number(sStaff.nEducation)+"','"+sStaff.strEmail+"',"
            "'"+sStaff.strEnter+"','"+sStaff.strGraduationDate+"','"+sStaff.strGraduationSchool+"','"+sStaff.strIdCard+"','"+sStaff.strMajor+"',"
            "'"+QString::number(sStaff.nMaritalStatus)+"','"+sStaff.strName+"','"+QString::number(sStaff.nNation)+"','"+sStaff.strNative+"',"
            "'"+sStaff.strNo+"','"+sStaff.strPhone+"','"+QString::number(sStaff.nPolitics)+"','"+sStaff.strPost+"','"+sStaff.strRemark+"','"+QString::number(sStaff.nSex)+"','"+sStaff.strImagePath+"')";

    if(query.exec(SQL))
        return true;
    else
        return false;
}

//删除报警记录
bool CMYSQL::_DeleteAlarmRecord(QString QAddr, QString QStime, QString QEtime)
{
    QString SQL;
    if(QAddr.length() > 0)
        SQL="DELETE FROM alarm WHERE Alarm_Intra_Addr = '"+QAddr+"' and Alarm_Stime >= '"+QStime+"' and Alarm_Stime <='"+QEtime+"'";
    else if(QAddr.length()<=0)
        SQL="DELETE FROM alarm WHERE Alarm_Stime >= '"+QStime+"' and Alarm_Stime <='"+QEtime+"'";
    QSqlQuery query;
    return query.exec(SQL);
}

//删除设备布防状态
bool CMYSQL::_DeleteDeviceRecord(QString QAddr, QString QStime, QString QEtime)
{
    QString SQL;
    if(QAddr.length()>0)
        SQL="DELETE FROM device_record WHERE Device_Record_Addr = '"+QAddr+"' and Device_Record_Time >= '"+QStime+"' and Device_Record_Time <='"+QEtime+"'";
    else if(QAddr.length()<=0)
        SQL="DELETE FROM device_record WHERE Device_Record_Time >= '"+QStime+"' and Device_Record_Time <='"+QEtime+"'";
    QSqlQuery query;
    qDebug()<<SQL;
    return query.exec(SQL);
}

//查找工号
 bool CMYSQL::_IsHaveNo(QString strNo)
 {
     switch(strNo.length()){
     case 1:
         strNo = "0000" + strNo;
         break;
     case 2:
         strNo = "000" + strNo;
         break;
     case 3:
         strNo = "00" + strNo;
         break;
     case 4:
         strNo = "0" + strNo;
         break;
     default:
         break;
     }

     QString strSql = "SELECT * FROM `staff` a WHERE a.Staff_No = '"+strNo+"' ";
     QSqlQuery query;
     if(query.exec(strSql))
     {
         if(query.numRowsAffected() != 0)
             return true;
     }else{
         return true;
     }
     return false;

 }

//删除系统日志
bool CMYSQL::_DeleteSystemRecord(QString QAddr, QString QStime, QString QEtime)
{
    QString SQL;
    if(QAddr.length()>0)    //用户名若为null，则删除改时间端内的所有记录
        SQL="DELETE FROM system_record WHERE System_Record_Name = '"+QAddr+"' and System_Record_Time >= '"+QStime+"' and System_Record_Time <='"+QEtime+"'";
    else if(QAddr.length()<=0)
        SQL="DELETE FROM system_record WHERE System_Record_Time >= '"+QStime+"' and System_Record_Time <='"+QEtime+"'";
    QSqlQuery query;
    return query.exec(SQL);
}

//更新所有的打卡记录
bool CMYSQL::_UpdateAllRacd(QString strAddrExplain,unsigned char* buf)
{
    QString SELSQL;
    SELSQL = "SELECT *	FROM card_record a ORDER BY a.Card_Record_Staff_No,a.Card_Record_Date,a.Card_Record_Time";
    QSqlQuery query1;
    QString Racd_Id="";
    QDate Old_date(0,0,0);
    QTime Old_time(0,0,0);
    QString Racd_Addr="";
    if(query1.exec(SELSQL))
    {
        while(query1.next())
        {
            QString strId = query1.value(1).toString();
            QDate strDate = query1.value(2).toDate();
            QTime strTime = query1.value(3).toTime();
            QString strAddr = query1.value(4).toString();
            if(strId == Racd_Id && strDate == Old_date && strTime == Old_time && strAddr == Racd_Addr)
            {
                QString DELSQL;
                DELSQL = "DELETE FROM card_record WHERE Card_Record_Staff_No = '"+strId+"' AND Card_Record_Date = '"+strDate.toString("yyyy-MM-dd")+"' AND Card_Record_Time = '"+strTime.toString("hh:mm:ss")+"' AND Card_Record_Device_No = '"+strAddr+"'";
                QSqlQuery query2;
                qDebug()<<"query insert"<<query2.exec(DELSQL)<<DELSQL<<endl;
                QString INSSQL;
                INSSQL = "INSERT INTO card_record (card_record.Card_Record_Staff_No,card_record.Card_Record_Date,card_record.Card_Record_Time,card_record.Card_Record_Device_No) VALUES ('"+strId+"','"+strDate.toString("yyyy-MM-dd")+"','"+strTime.toString("hh:mm:ss")+"','"+strAddr+"')";
                QSqlQuery query3;
                qDebug()<<"query insert"<<query3.exec(INSSQL)<<INSSQL<<endl;
            }
            Racd_Id = strId;
            Old_date = query1.value(2).toDate();
            Old_time = query1.value(3).toTime();
            Racd_Addr = strAddr;

        }
    }
    return true;
}

