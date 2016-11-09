#include "monthly.h"
#include "ui_monthly.h"

CMONTHLY::CMONTHLY(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CMONTHLY)
{
    ui->setupUi(this);
    m_Attendanceset = new CATTENDANCESET();
//    connect(m_Attendanceset,SIGNAL(SigrestType(QString)),this,SLOT(SlotrestType(QString)));
//    connect(this,SIGNAL(SigcardDate(int,int,int)),m_Attendanceset,SLOT(SlotcardDate(int,int,int)));
    HolidayFate = 0;
    RealFate = 0;
    ui->Line_Year->setText(QString::number(QDate::currentDate().year()));
    ui->Line_Year->setDisabled(true);
    ui->Line_Year->setAlignment(Qt::AlignCenter);

    ui->QpbtnBack->setToolTip(tr("返回"));
    ui->QpbtnLead->setToolTip(tr("导出报表"));

    _InitData();
    _InitTree();
}

CMONTHLY::~CMONTHLY()
{
    delete ui;
}

void CMONTHLY::_Update()
{
    _InitTree();
    ui->Data_tableWidget->setRowCount(0);
    FindName.clear();
    ui->SearchData->clear();
}

/*******初始化函数：月报表数据*********/
void CMONTHLY::_InitData()
{
    ui->Data_tableWidget->setColumnCount(16);
    ui->Data_tableWidget->setShowGrid(true);

    QStringList headers;
    headers<<"编号"<<"姓名"<<"年月"<<"应出勤天数"<<"实出勤天数"<<"缺勤天数"<<"迟到/分钟"<<"早退/分钟"<<"迟到/天数"<<"早退/天数"<<"少打卡次数"<<"节假/小时"<<"请假/小时"<<"工作小时"<<"节假加班/小时"<<"平时加班/小时";
    ui->Data_tableWidget->setHorizontalHeaderLabels(headers);
    ui->Data_tableWidget->resizeColumnsToContents();
    ui->Data_tableWidget->setFont(QFont("Helvetica"));
    ui->Data_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->Data_tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->Data_tableWidget->setColumnWidth(0,60);
    ui->Data_tableWidget->setColumnWidth(1,80);
    ui->Data_tableWidget->setColumnWidth(2,100);
    ui->Data_tableWidget->setColumnWidth(3,100);
    ui->Data_tableWidget->setColumnWidth(4,100);
    ui->Data_tableWidget->setColumnWidth(5,100);
    ui->Data_tableWidget->setColumnWidth(6,80);
    ui->Data_tableWidget->setColumnWidth(7,80);
    ui->Data_tableWidget->setColumnWidth(8,60);
    ui->Data_tableWidget->setColumnWidth(9,60);
    ui->Data_tableWidget->setColumnWidth(10,80);
    ui->Data_tableWidget->setColumnWidth(11,80);
    ui->Data_tableWidget->setColumnWidth(12,80);
    ui->Data_tableWidget->setColumnWidth(13,80);
    ui->Data_tableWidget->setColumnWidth(14,100);
    ui->Data_tableWidget->setColumnWidth(15,100);
}

/*******初始化函数：月报表员工树*********/
void CMONTHLY::_InitTree()
{
    m_List.clear();
    ui->Month_Tree->clear();
    ui->Month_Tree->setHeaderHidden(true);
    QString SQL = "SELECT * from department ORDER BY Department_No DESC";
    QSqlQuery query;
    if(query.exec(SQL))
    {
        int i = 1;
        while(query.next())
        {
            SList data;
            data.Id = query.value(0).toInt();
            data.strNo = query.value(1).toString();
            data.strName = query.value(2).toString();
            m_List.append(data);
            i++;
        }
    }

    QTreeWidgetItem* B1 = new QTreeWidgetItem(QStringList()<<"在职");
    ui->Month_Tree->addTopLevelItem(B1);

    QTreeWidgetItem* C = new QTreeWidgetItem(QStringList()<<"未分配");
    B1->addChild(C);
    QString unabsorbedSQL;
    QSqlQuery unabsorbedquery;
    unabsorbedSQL = "SELECT a.Staff_Name,a.Staff_No FROM `staff` a WHERE a.Staff_No NOT IN (SELECT b.Staff_No FROM staff b,department c WHERE b.Staff_Department_No = c.Department_No) ORDER BY a.Staff_No ASC";
    if(unabsorbedquery.exec(unabsorbedSQL))
    {
        while(unabsorbedquery.next())
        {
            QTreeWidgetItem *CB = new QTreeWidgetItem(QStringList()<<unabsorbedquery.value(0).toString());
            C->addChild(CB);
        }
    }

    QTreeWidgetItem* B[m_List.size()];
    for(int i = 0;i < m_List.size();i++)
    {
        B[i] = new QTreeWidgetItem(QStringList()<<m_List.at(i).strName);
        B1->addChild(B[i]);
        QString StaffSQL;
        QSqlQuery Staffquery;
        StaffSQL = "SELECT a.Staff_Name FROM `staff` a where  a.Staff_Department_No = '"+m_List.at(i).strNo+"' ORDER BY a.Staff_No ASC";
        if(Staffquery.exec(StaffSQL))
        {
            while(Staffquery.next())
            {
                QTreeWidgetItem *BiB = new QTreeWidgetItem(QStringList()<<Staffquery.value(0).toString());
                B[i]->addChild(BiB);
            }
        }

    }

    QTreeWidgetItem* B2 = new QTreeWidgetItem(QStringList()<<"离职");
    ui->Month_Tree->addTopLevelItem(B2);
    QString LeaveSQL;
    LeaveSQL = "SELECT a.Leave_Staff_Name FROM `leave_staff` a ORDER BY a.Leave_Staff_Id ASC";
    QSqlQuery Leavequery;
    if(Leavequery.exec(LeaveSQL))
    {
        while(Leavequery.next())
        {
            QTreeWidgetItem *B2B = new QTreeWidgetItem(QStringList()<<Leavequery.value(0).toString());
            B2->addChild(B2B);
        }
    }
}

/*******更新函数：月报表数据*********/
void CMONTHLY::_updataData(QString strNo,int nFlag,QString strName)
{
    QDate StartDate(ui->Line_Year->text().toInt(),1,1);
    QDate EndDate(ui->Line_Year->text().toInt(),12,1);
    EndDate.setYMD(EndDate.year(),EndDate.month(),EndDate.dayOfWeek());
    QString strSTime = StartDate.toString("yyyy-MM-dd");
    QString strETime = EndDate.toString("yyyy-MM-dd");

    QDate C_date = QDate::currentDate();
    QDate OldDate(0,0,0);
    QDate OldDateYMD(0,0,0);
    QString strNO1 = "";
    QString strNO2;
    QString SQL,data_SQL,Holid_SQL,leave_SQL;

    if(nFlag == 0)
    {
        //离职
        SQL = "SELECT a.Leave_Staff_No,a.Leave_Staff_Name,b.Card_Record_Date,b.Card_Record_Time,b.Card_Record_Device_No,b.Card_Record_Type FROM `leave_staff` a,card_record b WHERE "
                "a.Leave_Staff_No = b.Card_Record_Staff_No and b.Card_Record_Date >= a.Leave_Staff_Enter_Day  and b.Card_Record_Date <= a.Leave_Deal_Day "
                " and b.Card_Record_Date >= '"+strSTime+"'  and b.Card_Record_Date <= '"+strETime+"'  ORDER BY a.Leave_Staff_No,b.Card_Record_Date,b.Card_Record_Time ASC";
    }
    else if(nFlag == 1)
    {
        //部门
        SQL = "SELECT a.Staff_No,a.Staff_Name,b.Card_Record_Date,b.Card_Record_Time,b.Card_Record_Device_No,b.Card_Record_Type FROM `staff` a,card_record b WHERE "
                "a.Staff_No = b.Card_Record_Staff_No and a.Staff_Department_No = '"+strNo+"' and b.Card_Record_Date >= a.Staff_Enter_Day  "
                " and b.Card_Record_Date >= '"+strSTime+"'  and b.Card_Record_Date <= '"+strETime+"' ORDER BY a.Staff_No,b.Card_Record_Date,b.Card_Record_Time ASC";
    }
    else if(nFlag == 2)
    {
        //在职
        SQL = "SELECT a.Staff_No,a.Staff_Name,b.Card_Record_Date,b.Card_Record_Time,b.Card_Record_Device_No,b.Card_Record_Type FROM `staff` a,card_record b WHERE "
                "b.Card_Record_Date >= a.Staff_Enter_Day and b.Card_Record_Date >= '"+strSTime+"'  and b.Card_Record_Date <= '"+strETime+"' "
                "and  a.Staff_No = b.Card_Record_Staff_No ORDER BY a.Staff_No,b.Card_Record_Date,b.Card_Record_Time ASC";
    }
    else if(nFlag == 3)
    {
        //姓名
        if(m_nFlag)
        {
            if(strName == "")
            {
                SQL =" SELECT a.Staff_No,a.Staff_Name,b.Card_Record_Date,b.Card_Record_Time,b.Card_Record_Device_No,b.Card_Record_Type FROM"
                        "`staff` a,card_record b WHERE a.Staff_No = b.Card_Record_Staff_No and a.Staff_Name is NULL and "
                        "b.Card_Record_Date >= a.Staff_Enter_Day and b.Card_Record_Date >= '"+strSTime+"'  and b.Card_Record_Date <= '"+strETime+"' "
                        "ORDER BY a.Staff_No,b.Card_Record_Date,b.Card_Record_Time ASC";
            }
            else
            {
                SQL =" SELECT a.Staff_No,a.Staff_Name,b.Card_Record_Date,b.Card_Record_Time,b.Card_Record_Device_No,b.Card_Record_Type FROM"
                        "`staff` a,card_record b WHERE a.Staff_No = b.Card_Record_Staff_No and a.Staff_Name = '"+strName+"' and "
                        "b.Card_Record_Date >= a.Staff_Enter_Day and b.Card_Record_Date >= '"+strSTime+"'  and b.Card_Record_Date <= '"+strETime+"' "
                        "ORDER BY a.Staff_No,b.Card_Record_Date,b.Card_Record_Time ASC";
            }
        }
        else
        {
            SQL =" SELECT a.Leave_Staff_No,a.Leave_Staff_Name,b.Card_Record_Date,b.Card_Record_Time,b.Card_Record_Device_No,b.Card_Record_Type FROM"
                    "`leave_staff` a,card_record b WHERE a.Leave_Staff_No = b.Card_Record_Staff_No and a.Leave_Staff_Name = '"+strName+"' "
                    "and b.Card_Record_Date >= a.Leave_Staff_Enter_Day  and b.Card_Record_Date <= a.Leave_Deal_Day "
                    " and b.Card_Record_Date >= '"+strSTime+"'  and b.Card_Record_Date <= '"+strETime+"' "
                    "ORDER BY a.Leave_Staff_No,b.Card_Record_Date,b.Card_Record_Time ASC";
        }
    }
    else if(nFlag == 4)
    {
        //部门不存在
        SQL = "SELECT a.Staff_No,a.Staff_Name,b.Card_Record_Date,b.Card_Record_Time,b.Card_Record_Device_No,b.Card_Record_Type FROM (SELECT d.Staff_Name,d.Staff_No,"
                "d.Staff_Enter_Day,d.Staff_Department_No FROM (SELECT f.Staff_No,f.Staff_Name,f.Staff_Enter_Day,f.Staff_Department_No FROM `staff`"
                " f WHERE f.Staff_No NOT IN (SELECT e.Staff_No FROM staff e,department g WHERE e.Staff_Department_No = g.Department_No)) d) a,card_record b WHERE "
                "a.Staff_No = b.Card_Record_Staff_No  and b.Card_Record_Date >= a.Staff_Enter_Day  "
                " and b.Card_Record_Date >= '"+strSTime+"'  and b.Card_Record_Date <= '"+strETime+"' ORDER BY a.Staff_No,b.Card_Record_Date,b.Card_Record_Time ASC";
    }

    data_SQL="SELECT c.SetAttendance_Attendance_Time,c.SetAttendance_Closeing_Time,SetAttendance_Saturday_Attendance_Time,SetAttendance_Saturday_Closeing_Time,c.SetAttendance_Sunday_Attendance_Time,c.SetAttendance_Sunday_Closeing_Time FROM setattendance c";
    Holid_SQL = "SELECT * FROM holiday ORDER BY Holiday_StartDay";

    QSqlQuery qeruy,data_qeruy,holid_query;

    bool bSQL = qeruy.exec(SQL);
    data_qeruy.exec(data_SQL); 
    if(bSQL)
    {
        int ti=0;
        int num = 0;
        int staffN = 0;
        int Htime = 0;
        int Mtime = 0;
        int Gaptime = 0;
        int Hleave =0;
        int Mleave = 0;
        int Gapleave = 0;
        int lateday=0;
        int leaveday=0;
        int workHous=0;
        int workMinute=0;
        int holidayHous=0;
        int holidayMinute=0;
        int workM=0;
        int workN=0;
        int overtimeH=0;
        int overtimeM=0;
        double HolidayH=0;
        double calssday=0;
        int Card_Type=0;
        bool temp=true;
        QTime OldTime;
        QTime OldLeave;
        ui->Data_tableWidget->setRowCount(ti);
        if(data_qeruy.exec(data_SQL))
        {
            while(!data_qeruy.next());
        } 
        while(qeruy.next())
        {
            if( strNO1 != QString(qeruy.value(0).toByteArray().data()) || OldDate.year() != qeruy.value(2).toDate().year()||OldDate.month() != qeruy.value(2).toDate().month())
            {
                HolidayH=0;
                HolidayFate = 0;
                RealFate=0;
                temp=true;
                emit SigcardDate(qeruy.value(2).toDate().year(),qeruy.value(2).toDate().month(),qeruy.value(2).toDate().day());
                ui->Data_tableWidget->setRowCount(ti+1);

                QTableWidgetItem *tableItem0 = new QTableWidgetItem(QString(qeruy.value(0).toString()));
                tableItem0->setTextAlignment(Qt::AlignCenter);
                ui->Data_tableWidget->setItem(ti,0,tableItem0);

                QTableWidgetItem *tableItem1 = new QTableWidgetItem(QString(qeruy.value(1).toByteArray().data()));
                tableItem1->setTextAlignment(Qt::AlignCenter);
                ui->Data_tableWidget->setItem(ti,1,tableItem1);

                QTableWidgetItem *tableItem2 = new QTableWidgetItem(QString(qeruy.value(2).toDate().toString("yyyy-MM")));
                tableItem2->setTextAlignment(Qt::AlignCenter);
                ui->Data_tableWidget->setItem(ti,2,tableItem2);
                if(holid_query.exec(Holid_SQL))
                {
                    while(holid_query.next())
                    {
                        if(qeruy.value(2).toDate().toString("yyyy")==holid_query.value(2).toString())
                        {
                            for(int i = 0;i < holid_query.value(4).toInt();i++)
                            {
                                if(holid_query.value(3).toDate().addDays(i).toString("yyyy-MM")==qeruy.value(2).toDate().toString("yyyy-MM"))

                                    HolidayFate++;

                            }
                            switch(holid_query.value(1).toInt())
                            {
                                case 0:
                                case 2:
                                case 3: RealFate = HolidayFate;
                                    break;
                                case 4:
                                case 5:
                                {
                                    RealFate = HolidayFate-1;
                                    n_List.append(holid_query.value(3).toDate().addDays(3));
                                }
                                    break;
                                case 1:
                                case 6:
                                {
                                    RealFate = HolidayFate-2;
                                    n_List.append(holid_query.value(3).toDate().addDays(-1));
                                    n_List.append(holid_query.value(3).toDate().addDays(7));
                                }
                                    break;
                                default:
                                    break;
                            }
                        }
                    }
                }

                //请假时间的运算
                QString monthStart = QString::number(qeruy.value(2).toDate().year())+"/"+QString::number(qeruy.value(2).toDate().month())+"/"+QString::number(1);
                QString monthEnd = QString::number(qeruy.value(2).toDate().year())+"/"+QString::number(qeruy.value(2).toDate().month())+"/"+QString::number(31);
                leave_SQL = "SELECT c.Leave_Staff_No,c.Leave_SDateTime,c.Leave_EDateTime,c.Leave_ManHour FROM `leave` c WHERE c.Leave_Staff_No = '"+qeruy.value(0).toString()+"' and c.Leave_SDateTime >= '"+monthStart+"' and c.Leave_EDateTime <= '"+monthEnd+"'";
                QSqlQuery Leave_query;
                if(Leave_query.exec(leave_SQL))
                {
                    double leavetime=0;

                    while(Leave_query.next())
                    {
                        leavetime+=Leave_query.value(3).toDouble();
                    }
                    HolidayH = leavetime*10/1*0.1;

                }
                OldDate.setDate(qeruy.value(2).toDate().year(),qeruy.value(2).toDate().month(),qeruy.value(2).toDate().day());
                strNO1 = QString(qeruy.value(0).toByteArray().data());
                strNO2 =QString(qeruy.value(0).toByteArray().data());
                ti++;
                num=0;
                Mtime=0;
                Htime=0;
                Mleave=0;
                Hleave=0;
                lateday=0;
                leaveday=0;
                calssday=0;
                workHous=0;
                workMinute=0;
                overtimeH=0;
                overtimeM=0;
                holidayHous=0;
                holidayMinute=0;
            }

            switch(qeruy.value(2).toDate().dayOfWeek())
            {
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                {
                    OldTime = data_qeruy.value(0).toTime();
                    OldLeave = data_qeruy.value(1).toTime();
                    break;
                }
                case 6:
                {
                    OldTime = data_qeruy.value(2).toTime();
                    OldLeave = data_qeruy.value(3).toTime();
                    break;
                }
                 case 7:
                 {
                     OldTime = data_qeruy.value(4).toTime();
                     OldLeave = data_qeruy.value(5).toTime();
                     break;
                 }
                 default:
                     break;

             }

             if(strNO2 == QString(qeruy.value(0).toByteArray().data())&&!(OldDateYMD==qeruy.value(2).toDate()&&Card_Type==qeruy.value(5).toInt()))
             { 
                int n_NormalSpaceMinute;
                n_NormalSpaceMinute=(OldTime.secsTo(OldLeave)-5400)/60;
                //上班卡
                if(qeruy.value(5).toInt()==1)//上班卡,计算上班时间
                {
                    workM=0;
                    calssday+=0.5;
                    num++;
                    QTime RoudTime = qeruy.value(3).toTime();
                    int JudgeTime = OldTime.secsTo(RoudTime);
                    workM=(JudgeTime/60<=240?JudgeTime/60:240);
                    workM=(JudgeTime/60>=0?JudgeTime/60:0);

                    workMinute=workMinute+(n_NormalSpaceMinute/2-workM)%60;
                    workHous=workHous+(n_NormalSpaceMinute/2-workM)/60+workMinute/60;
                    workMinute=workMinute%60;

                    //迟到时间
                    if(workM>=1)
                    {
                        bool isholiday = true;
                        if(restType=="4"||restType=="5"||restType=="2")
                        {
                            for(int i = 0;i<n_List.size();i++)
                            {
                                if(n_List.at(i).toString("yyyy-MM-dd")==qeruy.value(2).toDate().toString("yyyy-MM-dd"))
                                {
                                    isholiday = false;
                                }

                            }
                            if(isholiday)
                            {
                                Mtime+=0;
                                Htime+=0;
                            }
                            else
                            {
                                lateday++;
                                Mtime += workM%60;
                                Htime += workM/60;
                            }
                        }
                        if(restType=="3"||restType=="6"||restType=="1")
                        {
                            lateday++;
                            Mtime += workM%60;
                            Htime += workM/60;
                        }
                    }
                 }
                 else if(qeruy.value(5).toInt()==2)//下班卡,计算上班时间
                 {
                    workN=0;
                    num++;
                    calssday+=0.5;                    
                    QTime LastTime = qeruy.value(3).toTime();
                    int JudgeTime = LastTime.secsTo(OldLeave);
                    workN=(JudgeTime/60<=240?JudgeTime/60:240);
                    workMinute=workMinute+(n_NormalSpaceMinute/2-workN)%60;
                    workHous=workHous+(n_NormalSpaceMinute/2-workN)/60+workMinute/60;
                    workMinute=workMinute%60;
                    //早退时间
                    if(workN>=1)
                    {
                        bool isholiday = true;
                        if(restType=="4"||restType=="5"||restType=="2")
                        {
                            for(int i = 0;i<n_List.size();i++)
                            {
                                if(n_List.at(i).toString("yyyy-MM-dd")==qeruy.value(2).toDate().toString("yyyy-MM-dd"))
                                {
                                    isholiday = false;
                                }

                            }
                            if(isholiday)
                            {
                                Mleave+=0;
                                Hleave+=0;
                            }
                            else
                            {
                                leaveday++;
                                Mleave+=workN%60;
                                Hleave+=workN/60;
                            }
                        }
                        if(restType=="3"||restType=="6"||restType=="1")
                        {
                            leaveday++;
                            Mleave+=workN%60;
                            Hleave+=workN/60;
                        }
                    }
                    //加班时间
                    if(workN<0)
                    {
                        bool isholiday = true;

                        if(restType=="4"||restType=="5"||restType=="2")
                        {
                            for(int i = 0;i<n_List.size();i++)
                            {
                                if(n_List.at(i).toString("yyyy-MM-dd")==qeruy.value(2).toDate().toString("yyyy-MM-dd"))
                                {
                                    isholiday = false;

                                }
                            }
                            if(isholiday)
                            {
                                bool isRedletterday = false;
                                if(holid_query.exec(Holid_SQL))
                                {
                                    while(holid_query.next())
                                    {
                                        for(int i =0;i<holid_query.value(4).toInt();i++)
                                        {
                                            if(holid_query.value(3).toDate().addDays(i).toString("dd")==qeruy.value(2).toDate().toString("dd"))
                                            {
                                                isRedletterday = true;
                                                break;
                                            }
                                        }
                                    }
                                }
                                if(isRedletterday)
                                {
                                    holidayMinute+=(n_NormalSpaceMinute-workM-workN)%60;
                                    holidayHous+=(n_NormalSpaceMinute-workM-workN)/60+overtimeM/60;
                                    holidayMinute=holidayMinute%60;

                                }
                                else
                                {
                                    overtimeM+=(n_NormalSpaceMinute-workM-workN)%60;
                                    overtimeH+=(n_NormalSpaceMinute-workM-workN)/60+overtimeM/60;
                                    overtimeM=overtimeM%60;

                                }
                            }
                            else
                            {
                                overtimeM+=workN%60;
                                overtimeH+=(workN/60)+overtimeM/60;
                                overtimeM=overtimeM%60;

                            }
                        }
                        if(restType=="3"||restType=="6"||restType=="1")
                        {   overtimeM+=workN%60;
                            overtimeH+=(workN/60)+overtimeM/60;
                            overtimeM=overtimeM%60;
                        }
                    }
                 }
//                if(holid_query.exec(Holid_SQL))
//                {
//                    while(holid_query.next())
//                    {   int ti=0;
//                        if(holid_query.value(3).toDate().addDays(ti).toString("dd")==qeruy.value(2).toDate().toString("dd"))
//                        {
//                            if(qeruy.value(5).toInt()==1)
//                            {   int workM = 0;
//                                int holidSec = OldTime.secsTo(qeruy.value(3).toTime());
//                                workM=(holidSec/60<=240?holidSec/60:240);
//                                workM=(holidSec/60>=0?holidSec/60:0);

//                                holidayMinute=holidayMinute+(n_NormalSpaceMinute/2-workM)%60;
//                                holidayHous=holidayHous+(n_NormalSpaceMinute/2-workM)/60+holidayMinute/60;
//                                holidayMinute=holidayMinute%60;

//                            }
//                            else if(qeruy.value(5).toInt()==2)
//                            {   int workM = 0;
//                                int holidSec = qeruy.value(3).toTime().secsTo(OldLeave);
//                                workM=(holidSec/60<=240?holidSec/60:240);
//                                workM=(holidSec/60>=0?holidSec/60:0);

//                                holidayMinute=holidayMinute+(n_NormalSpaceMinute/2-workM)%60;
//                                holidayHous=holidayHous+(n_NormalSpaceMinute/2-workM)/60+holidayMinute/60;
//                                holidayMinute=holidayMinute%60;

//                            }

//                        }
//                        ti++;
//                    }

//                }

              }
             Card_Type=qeruy.value(5).toInt();
             OldDateYMD=qeruy.value(2).toDate();
             strNO2 = QString(qeruy.value(0).toByteArray().data());

            QTableWidgetItem *tableItem4 = new QTableWidgetItem(QString::number(calssday));
            tableItem4->setTextAlignment(Qt::AlignCenter);
            ui->Data_tableWidget->setItem(ti-1,4,tableItem4);

            QTableWidgetItem *tableItem3;
            int NotWorkDay = 0,LackCard = 0;
            if(restType=="1")
            {
                tableItem3= new QTableWidgetItem(QString::number(qeruy.value(2).toDate().daysInMonth()-4-RealFate));
                NotWorkDay = qeruy.value(2).toDate().daysInMonth()-4-RealFate-calssday;
                LackCard = (qeruy.value(2).toDate().daysInMonth()-4-RealFate)*2-num;
                
            }
            else if(restType=="2")
            {
                tableItem3= new QTableWidgetItem(QString::number(qeruy.value(2).toDate().daysInMonth()-8-RealFate));
                NotWorkDay = qeruy.value(2).toDate().daysInMonth()-8-RealFate-calssday;
                LackCard = (qeruy.value(2).toDate().daysInMonth()-8-RealFate)*2-num;
                

            }
            else
            {
                tableItem3= new QTableWidgetItem(QString::number(qeruy.value(2).toDate().daysInMonth()-6-RealFate));
                NotWorkDay = qeruy.value(2).toDate().daysInMonth()-6-RealFate-calssday;
                LackCard = (qeruy.value(2).toDate().daysInMonth()-6-RealFate)*2-num;
            }
            tableItem3->setTextAlignment(Qt::AlignCenter);
            ui->Data_tableWidget->setItem(ti-1,3,tableItem3);
            if(NotWorkDay < 0)
                NotWorkDay = 0;
            QTableWidgetItem *tableItem5 = tableItem5 = new QTableWidgetItem(QString::number(NotWorkDay));
            tableItem5->setTextAlignment(Qt::AlignCenter);
            ui->Data_tableWidget->setItem(ti-1,5,tableItem5);


            QTableWidgetItem *tableItem6 = new QTableWidgetItem(QString::number(Htime)+"时"+QString::number(Mtime)+"分");
            tableItem6->setTextAlignment(Qt::AlignCenter);
            ui->Data_tableWidget->setItem(ti-1,6,tableItem6);

            QTableWidgetItem *tableItem7 = new QTableWidgetItem(QString::number(Hleave)+"时"+QString::number(Mleave)+"分");
            tableItem7->setTextAlignment(Qt::AlignCenter);
            ui->Data_tableWidget->setItem(ti-1,7,tableItem7);


            QTableWidgetItem *tableItem8 = new QTableWidgetItem(QString::number(lateday));
            tableItem8->setTextAlignment(Qt::AlignCenter);
            ui->Data_tableWidget->setItem(ti-1,8,tableItem8);

            QTableWidgetItem *tableItem9 = new QTableWidgetItem(QString::number(leaveday));
            tableItem9->setTextAlignment(Qt::AlignCenter);
            ui->Data_tableWidget->setItem(ti-1,9,tableItem9);

            if(LackCard<0)
                LackCard = 0;
            QTableWidgetItem *tableItem10 = new QTableWidgetItem(QString::number(LackCard));
            tableItem10->setTextAlignment(Qt::AlignCenter);
            ui->Data_tableWidget->setItem(ti-1,10,tableItem10);

            QTableWidgetItem *tableItem11 = new QTableWidgetItem(QString::number(HolidayFate*8)+"时");
            tableItem11->setTextAlignment(Qt::AlignCenter);
            ui->Data_tableWidget->setItem(ti-1,11,tableItem11);

            QTableWidgetItem *tableItem12 = new QTableWidgetItem(QString::number(HolidayH)+"时"/*+QString::number(HolidayM)+"分"*/);
            tableItem12->setTextAlignment(Qt::AlignCenter);
            ui->Data_tableWidget->setItem(ti-1,12,tableItem12);

            QTableWidgetItem *tableItem13 = new QTableWidgetItem(QString::number(workHous)+"时"+QString::number(workMinute)+"分");
            tableItem13->setTextAlignment(Qt::AlignCenter);
            ui->Data_tableWidget->setItem(ti-1,13,tableItem13);

            QTableWidgetItem *tableItem14 = new QTableWidgetItem(QString::number(abs(holidayHous))+"时"+QString::number(abs(holidayMinute))+"分");
            tableItem14->setTextAlignment(Qt::AlignCenter);
            ui->Data_tableWidget->setItem(ti-1,14,tableItem14);

            QTableWidgetItem *tableItem15 = new QTableWidgetItem(QString::number(abs(overtimeH))+"时"+QString::number(abs(overtimeM))+"分");
            tableItem15->setTextAlignment(Qt::AlignCenter);
            ui->Data_tableWidget->setItem(ti-1,15,tableItem15);
            for(int i =4;i<=15;i++)
            {
                QString context = ui->Data_tableWidget->item(ti-1,i)->text();
                if(i==6||i == 7||i==13||i==14||i==15)
                {
                    if(context.left(1)=="0"&&context.left(3).right(1)=="0")
                        ui->Data_tableWidget->item(ti-1,i)->setBackgroundColor(QColor(185,235,200));
                    else if(i==6||i == 7)
                        ui->Data_tableWidget->item(ti-1,i)->setBackgroundColor(QColor(235,193,184));
                }
                else
                {
                    if(context == "0")
                        ui->Data_tableWidget->item(ti-1,i)->setBackgroundColor(QColor(185,235,200));
                }
            }
        }
    }
}

/*****报表导出：生成EXCL文件*****/
void CMONTHLY::on_QpbtnLead_clicked()
{
    if(ui->Data_tableWidget->rowCount()>0)
    {
        QString filepath = QFileDialog::getSaveFileName(this,tr("save day"),".",tr("Microsoft Office 2003 (*.xls)"));
        if(!filepath.isEmpty())
        {
            QAxObject *excel = new QAxObject(this);
            excel->setControl("Excel.Application");
            excel->dynamicCall("SetVisibel (bool Visibel)","false");
            excel->setProperty("DisplayAlerts",false);

            QAxObject *workbooks = excel->querySubObject("WorkBooks");
            workbooks->dynamicCall("Add");
            QAxObject *workbook = excel->querySubObject("ActiveWorkBook");
            QAxObject *worksheets = workbook->querySubObject("Sheets");
            QAxObject *worksheet = worksheets->querySubObject("Item(int)",1);

            worksheet->setProperty("Name","day");

            QAxObject *cell[11];
            for(int i =1;i<=11;i++)
            {
                cell[i-1] =  worksheet->querySubObject("Cells(int,int)",1,i);
                QString str = "";
                switch(i)
                {
                case 1: str="编号";
                    break;
                case 2: str="姓名";
                    break;
                case 3: str="年月";
                    break;
                case 4: str="应出勤天数";
                    break;
                case 5: str="实出勤天数";
                    break;
                case 6: str="缺勤天数";
                    break;
                case 7: str="迟到/分钟";
                    break;
                case 8: str="早退/分钟";
                    break;
                case 9: str="迟到/天数";
                    break;
                case 10: str="早退/天数";
                    break;
                case 11: str="少打卡次数";
                    break;
                default:
                    break;
                }

                cell[i-1]->setProperty("Value", str);

                if(i==3)
                {
                    cell[i-1]->setProperty("ColumnWidth", 15);
                }
                cell[i-1]->setProperty("HorizontalAlignment", -4108); //左对齐（xlLeft）：-4131  居中（xlCenter）：-4108  右对齐（xlRight）：-4152
                cell[i-1]->setProperty("VerticalAlignment", -4108);  //上对齐（xlTop）-4160 居中（xlCenter）：-4108  下对齐（xlBottom）：-4107
                cell[i-1]->setProperty("WrapText", true);  //内容过多，自动换行
            }
            QAxObject *NewCell[ui->Data_tableWidget->rowCount()][11];
            for(int i=1;i<=ui->Data_tableWidget->rowCount();i++)
            {
                for(int j=1;j<=11;j++)
                {
                    NewCell[i-1][j-1] = worksheet->querySubObject("Cells(int,int)",i+1,j);
                    NewCell[i-1][j-1]->setProperty("Value",ui->Data_tableWidget->item(i-1,j-1)->text());
                    NewCell[i-1][j-1]->setProperty("HorizontalAlignment", -4108); //左对齐（xlLeft）：-4131  居中（xlCenter）：-4108  右对齐（xlRight）：-4152
                    NewCell[i-1][j-1]->setProperty("VerticalAlignment", -4108);  //上对齐（xlTop）-4160 居中（xlCenter）：-4108  下对齐（xlBottom）：-4107
                    NewCell[i-1][j-1]->setProperty("WrapText", true);  //内容过多，自动换行
                }
            }
            workbook->dynamicCall("SaveAs(const QString&)",QDir::toNativeSeparators(filepath));//保存至filepath，注意一定要用QDir::toNativeSeparators将路径中的"/"转换为"\"，不然一定保存不了。
            workbook->dynamicCall("Close()");//关闭工作簿
            excel->dynamicCall("Quit()");//关闭excel
            delete excel;
            excel=NULL;
        }
    }
    else
    {
        QMessageBox::information(this,"提示","没有记录");

    }
}

void CMONTHLY::SlotrestType(QString m_restType)
{
    restType=m_restType;
}

/*****选择员工，显示当月的月报表数据*****/
void CMONTHLY::_SelectTarget(QString SelectName)
{
    m_nFlag = 1;
    QString strNo = "";
    QString strName = SelectName;
    if(strName == "在职")
    {
        _updataData(strNo,2,"");
    }
    else if(strName == "离职")
    {
        m_nFlag = 0;
        _updataData(strNo,0,"");
    }
    else
    {
        int j = 0;
        for(int i = 0;i < m_List.size();i++)
        {
            if(strName == m_List.at(i).strName)
            {
                strNo = m_List.at(i).strNo;
                j = 1;
                break;
            }
        }
        if(j)
        {
            _updataData(strNo,1,"");
        }
        else if(strName == "未分配")
        {
            _updataData(strNo,4,"");
        }
        else
        {
            _updataData(0,3,strName);
        }
    }
}

void CMONTHLY::on_Month_Tree_clicked(const QModelIndex &index)
{
    FindName.clear();
    FindName = ui->Month_Tree->currentItem()->text(0);
    _SelectTarget(FindName);
}

void CMONTHLY::on_Qpbtn_Left_clicked()
{
    int Year = ui->Line_Year->text().toInt();
    Year--;
    ui->Line_Year->setText(QString::number(Year));
    ui->Line_Year->setText(QString::number(Year));
    if(!FindName.isEmpty())
        _SelectTarget(FindName);
}

void CMONTHLY::on_Qpbtn_Right_clicked()
{
    int Year = ui->Line_Year->text().toInt();
    Year++;
    ui->Line_Year->setText(QString::number(Year));
    if(!FindName.isEmpty())
        _SelectTarget(FindName);
}

void CMONTHLY::on_QpbtnSearch_clicked()
{
    FindName.clear();
    FindName = ui->SearchData->text();
    if(!FindName.isEmpty())
        _SelectTarget(FindName);
}

void CMONTHLY::on_QpbtnBack_clicked()
{
    emit SigReturnMenu();
}

void CMONTHLY::on_QpbtnStretching_clicked()
{
    int posX = 160-ui->QpbtnStretching->width();
    int posY = ui->QpbtnStretching->y();
    int nwidth = ui->QpbtnStretching->width();
    int nheight = ui->QpbtnStretching->height();
    int DayMinWidht =0;
    int DayMaxWidht = 160;
    if(ui->QpbtnStretching->pos().x()==posX)
    {
        ui->Month_Tree->setGeometry(ui->Month_Tree->x(),ui->Month_Tree->y(),DayMinWidht,ui->Month_Tree->height());
        ui->QpbtnStretching->setGeometry(0,posY,nwidth,nheight);
        ui->QpbtnStretching->setStyleSheet("background-image:url(:/images/StretchingR.png)");
    }
    else if(ui->QpbtnStretching->pos().x()==0)
    {
        ui->Month_Tree->setGeometry(ui->Month_Tree->x(),ui->Month_Tree->y(),DayMaxWidht,ui->Month_Tree->height());
        ui->QpbtnStretching->setGeometry(posX,posY,nwidth,nheight);
        ui->QpbtnStretching->setStyleSheet("background-image:url(:/images/StretchingL.png)");
    }
}


